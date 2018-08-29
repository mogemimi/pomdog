// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GraphicsDeviceVulkan.hpp"
#include "BufferVulkan.hpp"
#include "EffectReflection.hpp"
#include "PipelineStateVulkan.hpp"
#include "RenderTexture2DVulkan.hpp"
#include "SamplerStateVulkan.hpp"
#include "ShaderVulkan.hpp"
#include "Texture2DVulkan.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Basic/Version.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <vulkan/vulkan.h>
#include <array>
#include <tuple>

namespace Pomdog {
namespace Detail {
namespace Vulkan {
namespace {

constexpr std::uint32_t GetUint32EngineVersion() noexcept
{
    return POMDOG_MAJOR_VERSION * 10000
        + POMDOG_MINOR_VERSION * 100
        + POMDOG_PATCH_VERSION;
}

std::tuple<VkInstance, VkResult> CreateVulkanInstance() noexcept
{
    std::vector<const char*> layerNames;
    std::vector<const char*> extensionNames;

#if defined(_WIN32)
    extensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    extensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#if defined(POMDOG_PLATFORM_LINUX)
    extensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    extensionNames.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "pomdog";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = "pomdog";
    appInfo.engineVersion = GetUint32EngineVersion();
    appInfo.apiVersion = VK_API_VERSION;

    VkInstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = nullptr;
    instanceCreateInfo.flags = 0;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledLayerCount = static_cast<std::uint32_t>(layerNames.size());
    instanceCreateInfo.ppEnabledLayerNames = (layerNames.empty() ? nullptr : layerNames.data());
    instanceCreateInfo.enabledExtensionCount = static_cast<std::uint32_t>(extensionNames.size());
    instanceCreateInfo.ppEnabledExtensionNames = (extensionNames.empty() ? nullptr : extensionNames.data());

    VkInstance instance = nullptr;
    auto result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
    return std::make_tuple(instance, result);
}

std::vector<VkQueueFamilyProperties>
GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    std::uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);

    POMDOG_ASSERT(queueCount >= 1);
    std::vector<VkQueueFamilyProperties> queueFamilyProps;
    queueFamilyProps.resize(queueCount);
    return queueFamilyProps;
}

//void GetPhysicalDeviceProperties(VkPhysicalDevice device) noexcept
//{
//    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queueFamilyProps.data());
//    POMDOG_ASSERT(queueCount >= 1);
//
//    VkPhysicalDeviceMemoryProperties memoryProps;
//    vkGetPhysicalDeviceMemoryProperties(device, &memoryProps);
//
//    VkPhysicalDeviceProperties deviceProps;
//    vkGetPhysicalDeviceProperties(device, &deviceProps);
//}

std::tuple<std::vector<VkPhysicalDevice>, VkResult> EnumerateDevices(VkInstance instance) noexcept
{
    POMDOG_ASSERT(instance != nullptr);

    std::uint32_t physicalDeviceCount = 0;
    auto result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to call vkEnumeratePhysicalDevices");
    }

    POMDOG_ASSERT(physicalDeviceCount >= 1);
    std::vector<VkPhysicalDevice> physicalDevices;
    physicalDevices.resize(physicalDeviceCount);

    result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
    if (result != VK_SUCCESS) {
        // Error: Failed to enumerate physical devices and GPUs
        return std::make_tuple(std::move(physicalDevices), result);
    }

    POMDOG_ASSERT(physicalDeviceCount >= 1);
    POMDOG_ASSERT(!physicalDevices.empty());
    return std::make_tuple(std::move(physicalDevices), result);
}

} // unnamed namespace

class GraphicsDeviceVulkan::Impl final {
public:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;

public:
    Impl();

    ~Impl();
};

GraphicsDeviceVulkan::Impl::Impl()
    : instance(nullptr)
    , physicalDevice(nullptr)
    , device(nullptr)
{
    VkResult result = VK_SUCCESS;

    std::tie(instance, result) = CreateVulkanInstance();
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create VkInstance");
    }

    std::vector<VkPhysicalDevice> devices;
    std::tie(devices, result) = EnumerateDevices(instance);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to enumerate physical devices and GPUs");
    }
    if (devices.empty()) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "This list of physical devices is empty.");
    }

    physicalDevice = devices.front();

    std::array<float, 1> queuePriorities = { 0.0f };

    VkDeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = nullptr;
    queueCreateInfo.flags = 0;
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePriorities.data();

    auto queueProps = GetPhysicalDeviceQueueFamilyProperties(physicalDevice);
    POMDOG_ASSERT(queueProps.size() >= 1);
    for (std::uint32_t index = 0; index < queueProps.size(); index++) {
        if ((queueProps[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            queueCreateInfo.queueFamilyIndex = index;
            break;
        }
    }

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
    deviceCreateInfo.pEnabledFeatures = nullptr;

    result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create Vulkan device.");
    }
}

GraphicsDeviceVulkan::Impl::~Impl()
{
    if (device != nullptr) {
        vkDestroyDevice(device, nullptr);
    }
    if (instance != nullptr) {
        vkDestroyInstance(instance, nullptr);
    }
}

GraphicsDeviceVulkan::GraphicsDeviceVulkan()
    : impl(std::make_unique<Impl>())
{
}

GraphicsDeviceVulkan::~GraphicsDeviceVulkan() = default;

ShaderLanguage GraphicsDeviceVulkan::GetSupportedLanguage() const
{
    return ShaderLanguage::GLSL;
}

std::unique_ptr<NativeGraphicsCommandList>
GraphicsDeviceVulkan::CreateGraphicsCommandList()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::unique_ptr<Shader>
GraphicsDeviceVulkan::CreateShader(
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nullptr);
    return std::make_unique<ShaderVulkan>(
        impl->device, shaderBytecode, compileOptions);
}

std::unique_ptr<NativeBuffer>
GraphicsDeviceVulkan::CreateBuffer(
    std::size_t sizeInBytes, BufferUsage bufferUsage, BufferBindMode)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nullptr);
    return std::make_unique<BufferVulkan>(
        impl->device, sizeInBytes, bufferUsage);
}

std::unique_ptr<NativeBuffer>
GraphicsDeviceVulkan::CreateBuffer(
    const void* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    BufferBindMode)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nullptr);
    return std::make_unique<BufferVulkan>(
        impl->device, sourceData, sizeInBytes, bufferUsage);
}

std::unique_ptr<NativeSamplerState>
GraphicsDeviceVulkan::CreateSamplerState(const SamplerDescription& description)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nullptr);
    return std::make_unique<SamplerStateVulkan>(impl->device, description);
}

std::unique_ptr<NativePipelineState>
GraphicsDeviceVulkan::CreatePipelineState(const PipelineStateDescription& description)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nullptr);
    return std::make_unique<PipelineStateVulkan>(impl->device, description);
}

std::unique_ptr<NativeEffectReflection>
GraphicsDeviceVulkan::CreateEffectReflection(
    const PipelineStateDescription& description,
    NativePipelineState & pipelineState)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::unique_ptr<NativeTexture2D>
GraphicsDeviceVulkan::CreateTexture2D(
    std::int32_t width,
    std::int32_t height,
    std::int32_t mipmapLevels,
    SurfaceFormat format)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nullptr);
    return std::make_unique<Texture2DVulkan>(
        impl->device, width, height, mipmapLevels, format);
}

std::unique_ptr<NativeRenderTarget2D>
GraphicsDeviceVulkan::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height,
    std::int32_t mipmapLevels,
    SurfaceFormat format,
    DepthFormat depthStencilFormat,
    std::int32_t multiSampleCount)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nullptr);
    return std::make_unique<RenderTarget2DVulkan>(
        impl->device, width, height, mipmapLevels,
        format, depthStencilFormat, multiSampleCount);
}

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
