// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/graphics_device_vulkan.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/version.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/gpu/backends/texture_helper.h"
#include "pomdog/gpu/buffer_bind_flags.h"
#include "pomdog/gpu/buffer_desc.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/texture_desc.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/gpu/vulkan/buffer_vulkan.h"
#include "pomdog/gpu/vulkan/command_list_vulkan.h"
#include "pomdog/gpu/vulkan/depth_stencil_buffer_vulkan.h"
#include "pomdog/gpu/vulkan/pipeline_state_vulkan.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/gpu/vulkan/render_target2d_vulkan.h"
#include "pomdog/gpu/vulkan/sampler_state_vulkan.h"
#include "pomdog/gpu/vulkan/shader_vulkan.h"
#include "pomdog/gpu/vulkan/swap_chain_vulkan.h"
#include "pomdog/gpu/vulkan/texture2d_vulkan.h"
#include "pomdog/gpu/vulkan/vulkan_format_helper.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

#if defined(_WIN32)
#include <vulkan/vulkan_win32.h>
#elif defined(POMDOG_PLATFORM_LINUX)
#include <vulkan/vulkan_xlib.h>
#elif defined(POMDOG_PLATFORM_MACOSX)
#include <vulkan/vulkan_macos.h>
#endif

namespace pomdog::gpu::detail::vulkan {
namespace {

[[nodiscard]] MemoryUsage
toMemoryUsage(BufferUsage bufferUsage) noexcept
{
    return (bufferUsage == BufferUsage::Immutable) ? MemoryUsage::GpuOnly : MemoryUsage::CpuToGpu;
}

[[nodiscard]] constexpr u32
getUint32EngineVersion() noexcept
{
    return POMDOG_MAJOR_VERSION * 10000 +
           POMDOG_MINOR_VERSION * 100 +
           POMDOG_PATCH_VERSION;
}

[[nodiscard]] std::tuple<VkInstance, VkResult>
createVulkanInstance() noexcept
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "pomdog";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "pomdog";
    appInfo.engineVersion = getUint32EngineVersion();
    appInfo.apiVersion = VK_API_VERSION_1_2;

    std::array<const char*, 1> const validationLayers = {{
        "VK_LAYER_KHRONOS_validation",
    }};

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    constexpr bool enableValidationLayers = true;
#else
    constexpr bool enableValidationLayers = false;
#endif

    std::vector<VkLayerProperties> availableLayers;

    if constexpr (enableValidationLayers) {
        u32 layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        availableLayers.resize(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        const auto foundLayer = [&]() -> bool {
            for (const auto* layerName : validationLayers) {
                for (const auto& props : availableLayers) {
                    constexpr std::size_t n = sizeof(props.layerName);
                    static_assert(n > 0);
                    static_assert(n <= 256);
                    if (::strncmp(layerName, props.layerName, n) == 0) {
                        return true;
                    }
                }
            }
            return false;
        }();

        if (!foundLayer) {
            return std::make_tuple(nullptr, VK_INCOMPLETE);
        }
    }

    std::vector<const char*> extensionNames;
    extensionNames.reserve(4);
#if defined(_WIN32)
    extensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    extensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(POMDOG_PLATFORM_LINUX)
    extensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    extensionNames.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(POMDOG_PLATFORM_MACOSX)
    extensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    extensionNames.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#else
#error "Platform undefined or not supported."
#endif
    if constexpr (enableValidationLayers) {
        extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;

    if (!extensionNames.empty()) {
        createInfo.enabledExtensionCount = static_cast<u32>(extensionNames.size());
        createInfo.ppEnabledExtensionNames = extensionNames.data();
    }
    else {
        createInfo.enabledExtensionCount = 0;
        createInfo.ppEnabledExtensionNames = nullptr;
    }

    if constexpr (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<u32>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    VkInstance instance = nullptr;
    auto result = vkCreateInstance(&createInfo, nullptr, &instance);
    return std::make_tuple(instance, result);
}

[[nodiscard]] std::vector<VkQueueFamilyProperties>
getPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    u32 queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);

    POMDOG_ASSERT(queueCount >= 1);
    std::vector<VkQueueFamilyProperties> queueFamilyProps;
    queueFamilyProps.resize(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queueFamilyProps.data());
    return queueFamilyProps;
}

[[nodiscard]] std::tuple<std::vector<VkPhysicalDevice>, std::unique_ptr<Error>>
enumerateDevices(VkInstance instance) noexcept
{
    POMDOG_ASSERT(instance != nullptr);
    std::vector<VkPhysicalDevice> physicalDevices = {};

    u32 physicalDeviceCount = 0;
    auto result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if (result != VK_SUCCESS) {
        return std::make_tuple(std::move(physicalDevices), errors::make("vkEnumeratePhysicalDevices() failed with error code: " + std::to_string(result)));
    }
    if (physicalDeviceCount <= 0) {
        return std::make_tuple(std::move(physicalDevices), errors::make("failed to find any Vulkan-compatible GPU"));
    }

    POMDOG_ASSERT(physicalDeviceCount >= 1);
    physicalDevices.resize(physicalDeviceCount);

    result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
    if (result != VK_SUCCESS) {
        return std::make_tuple(std::move(physicalDevices), errors::make("vkEnumeratePhysicalDevices() failed with error code: " + std::to_string(result)));
    }

    POMDOG_ASSERT(physicalDeviceCount >= 1);
    POMDOG_ASSERT(!physicalDevices.empty());
    return std::make_tuple(std::move(physicalDevices), nullptr);
}

[[nodiscard]] int
computeScoreDeviceSuitability(
    const VkPhysicalDeviceProperties& props,
    const VkPhysicalDeviceFeatures& features) noexcept
{
    int score = 0;
    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }
    score += props.limits.maxImageDimension2D;
    if (features.geometryShader) {
        score += 50;
    }
    if (features.tessellationShader) {
        score += 50;
    }
    return score;
}

[[nodiscard]] VkPhysicalDevice
findSuitableDevice(const std::vector<VkPhysicalDevice>& devices)
{
    VkPhysicalDevice suitableDevice = nullptr;
    int maxScore = 0;

    for (const auto& device : devices) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device, &props);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        const auto score = computeScoreDeviceSuitability(props, features);
        if (score > maxScore) {
            suitableDevice = device;
            maxScore = score;
        }
    }
    return suitableDevice;
}

class GraphicsDeviceVulkanImpl final : public GraphicsDeviceVulkan {
private:
    VkInstance instance_ = nullptr;
    VkPhysicalDevice physicalDevice_ = nullptr;
    VkDevice device_ = nullptr;
    VkCommandPool commandPool_ = nullptr;
    VkQueue graphicsQueue_ = nullptr;
    u32 graphicsQueueFamilyIndex_ = 0;
    unsafe_ptr<SwapChainVulkan> swapChain_ = nullptr;
    PresentationParameters presentationParams_ = {};

public:
    GraphicsDeviceVulkanImpl() = default;

    ~GraphicsDeviceVulkanImpl() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize() noexcept;

    [[nodiscard]] GraphicsBackend
    getBackendKind() const noexcept override;

    [[nodiscard]] PresentationParameters
    getPresentationParameters() const noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<CommandList>, std::unique_ptr<Error>>
    createCommandList() noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
    createBuffer(const BufferDesc& desc) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
    createBuffer(const BufferDesc& desc, std::span<const u8> initialData) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    createVertexBuffer(const void* vertices, u32 vertexCount, u32 strideBytes, BufferUsage bufferUsage) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    createVertexBuffer(u32 vertexCount, u32 strideBytes, BufferUsage bufferUsage) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    createIndexBuffer(IndexFormat elementSize, const void* indices, u32 indexCount, BufferUsage bufferUsage) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    createIndexBuffer(IndexFormat elementSize, u32 indexCount, BufferUsage bufferUsage) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    createConstantBuffer(const void* sourceData, u32 sizeInBytes, BufferUsage bufferUsage) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    createConstantBuffer(u32 sizeInBytes, BufferUsage bufferUsage) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
    createPipelineState(const PipelineDesc& descriptor) noexcept override;

    [[nodiscard]] std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
    createShader(std::span<const u8> shaderBytecode, const detail::ShaderCompileOptions& compileOptions) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    createRenderTarget2D(i32 width, i32 height) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    createRenderTarget2D(i32 width, i32 height, bool generateMipmap, PixelFormat format) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
    createDepthStencilBuffer(i32 width, i32 height, PixelFormat depthStencilFormat) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
    createSamplerState(const SamplerDesc& descriptor) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
    createTexture2D(i32 width, i32 height) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
    createTexture2D(i32 width, i32 height, bool mipMap, PixelFormat format) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<gpu::Texture>, std::unique_ptr<Error>>
    createTexture(const TextureDesc& desc) noexcept override;

    [[nodiscard]] ::VkDevice
    getDevice() const noexcept override;

    [[nodiscard]] ::VkQueue
    getGraphicsQueue() const noexcept override;

    [[nodiscard]] ::VkInstance
    getInstance() const noexcept override;

    [[nodiscard]] ::VkPhysicalDevice
    getPhysicalDevice() const noexcept override;

    [[nodiscard]] ::VkCommandPool
    getCommandPool() const noexcept override;

    [[nodiscard]] u32
    getGraphicsQueueFamilyIndex() const noexcept override;

    void
    setSwapChain(unsafe_ptr<SwapChainVulkan> swapChain) noexcept override;

    void
    setPresentationParameters(const PresentationParameters& params) noexcept override;
};

std::unique_ptr<Error>
GraphicsDeviceVulkanImpl::initialize() noexcept
{
    VkResult result = VK_SUCCESS;

    std::tie(instance_, result) = createVulkanInstance();
    if (result != VK_SUCCESS) {
        return errors::make("createVulkanInstance() failed with error code: " + std::to_string(result));
    }

    std::vector<VkPhysicalDevice> devices = {};
    if (auto [devicesResult, enumerateErr] = enumerateDevices(instance_); enumerateErr != nullptr) {
        return errors::wrap(std::move(enumerateErr), "failed to enumerate physical devices and GPUs");
    }
    else {
        devices = std::move(devicesResult);
    }

    if (devices.empty()) {
        return errors::make("no Vulkan-compatible GPU found");
    }

    physicalDevice_ = findSuitableDevice(devices);
    if (physicalDevice_ == nullptr) {
        return errors::make("failed to find a suitable GPU");
    }

    std::array<f32, 1> queuePriorities = {0.0f};

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = nullptr;
    queueCreateInfo.flags = 0;
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePriorities.data();

    auto queueProps = getPhysicalDeviceQueueFamilyProperties(physicalDevice_);
    POMDOG_ASSERT(queueProps.size() >= 1);
    for (u32 index = 0; index < queueProps.size(); index++) {
        if ((queueProps[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            queueCreateInfo.queueFamilyIndex = index;
            graphicsQueueFamilyIndex_ = index;
            break;
        }
    }

    // NOTE: Enable swap chain device extension
    std::array<const char*, 1> deviceExtensions = {{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    }};

    // NOTE: Enable descriptor indexing features (partially bound descriptors)
    VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures = {};
    indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;

    // NOTE: Enable device features
    VkPhysicalDeviceFeatures enabledFeatures = {};
    enabledFeatures.fillModeNonSolid = VK_TRUE;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = &indexingFeatures;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = static_cast<u32>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

    result = vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &device_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create Vulkan device with error code: " + std::to_string(result));
    }

    // NOTE: Retrieve the graphics queue
    vkGetDeviceQueue(device_, graphicsQueueFamilyIndex_, 0, &graphicsQueue_);

    // NOTE: Create a command pool for the graphics queue family
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamilyIndex_;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    result = vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool_);
    if (result != VK_SUCCESS) {
        return errors::make("failed to create Vulkan command pool with error code: " + std::to_string(result));
    }

    return nullptr;
}

GraphicsDeviceVulkanImpl::~GraphicsDeviceVulkanImpl()
{
    if (commandPool_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyCommandPool(device_, commandPool_, nullptr);
    }
    if (device_ != nullptr) {
        vkDestroyDevice(device_, nullptr);
    }
    if (instance_ != nullptr) {
        vkDestroyInstance(instance_, nullptr);
    }
}

GraphicsBackend GraphicsDeviceVulkanImpl::getBackendKind() const noexcept
{
    return GraphicsBackend::Vulkan;
}

PresentationParameters GraphicsDeviceVulkanImpl::getPresentationParameters() const noexcept
{
    return presentationParams_;
}

std::tuple<std::shared_ptr<CommandList>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createCommandList() noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(commandPool_ != nullptr);

    auto commandList = std::make_shared<CommandListVulkan>(
        device_,
        commandPool_);

    if (commandList->getCommandBuffer() == nullptr) {
        return std::make_tuple(nullptr, errors::make("failed to allocate Vulkan command buffer"));
    }

    // NOTE: Inject swap chain so the command list can begin render passes
    if (swapChain_ != nullptr) {
        commandList->setSwapChain(swapChain_);
    }

    return std::make_tuple(std::move(commandList), nullptr);
}

std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createBuffer(const BufferDesc& desc) noexcept
{
    return createBuffer(desc, std::span<const u8>{});
}

std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createBuffer(const BufferDesc& desc, std::span<const u8> initialData) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(desc.sizeInBytes > 0);

    auto nativeBuffer = std::make_shared<BufferVulkan>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->initialize(
            device_,
            physicalDevice_,
            desc,
            initialData);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize BufferVulkan"));
    }

    return std::make_tuple(std::move(nativeBuffer), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createVertexBuffer(
    const void* vertices,
    u32 vertexCount,
    u32 strideBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(vertices != nullptr);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideBytes > 0);

    const auto sizeInBytes = vertexCount * strideBytes;

    BufferDesc desc = {};
    desc.sizeInBytes = sizeInBytes;
    desc.memoryUsage = toMemoryUsage(bufferUsage);
    desc.bindFlags = BufferBindFlags::VertexBuffer;

    auto [buffer, err] = createBuffer(
        desc, std::span<const u8>(static_cast<const u8*>(vertices), sizeInBytes));
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create vertex buffer"));
    }

    auto vertexBuffer = std::make_shared<VertexBuffer>(
        std::move(buffer), vertexCount, strideBytes, bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createVertexBuffer(
    u32 vertexCount,
    u32 strideBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideBytes > 0);

    const auto sizeInBytes = vertexCount * strideBytes;

    BufferDesc desc = {};
    desc.sizeInBytes = sizeInBytes;
    desc.memoryUsage = toMemoryUsage(bufferUsage);
    desc.bindFlags = BufferBindFlags::VertexBuffer;

    auto [buffer, err] = createBuffer(desc);
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create vertex buffer"));
    }

    auto vertexBuffer = std::make_shared<VertexBuffer>(
        std::move(buffer), vertexCount, strideBytes, bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createIndexBuffer(
    IndexFormat elementSize,
    const void* indices,
    u32 indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(indexCount > 0);

    const auto sizeInBytes = indexCount * detail::BufferHelper::toIndexElementOffsetBytes(elementSize);

    BufferDesc desc = {};
    desc.sizeInBytes = sizeInBytes;
    desc.memoryUsage = toMemoryUsage(bufferUsage);
    desc.bindFlags = BufferBindFlags::IndexBuffer;

    auto [buffer, err] = createBuffer(
        desc, std::span<const u8>(static_cast<const u8*>(indices), sizeInBytes));
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create index buffer"));
    }

    auto indexBuffer = std::make_shared<IndexBuffer>(
        std::move(buffer), elementSize, indexCount, bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createIndexBuffer(
    IndexFormat elementSize,
    u32 indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(indexCount > 0);

    const auto sizeInBytes = indexCount * detail::BufferHelper::toIndexElementOffsetBytes(elementSize);

    BufferDesc desc = {};
    desc.sizeInBytes = sizeInBytes;
    desc.memoryUsage = toMemoryUsage(bufferUsage);
    desc.bindFlags = BufferBindFlags::IndexBuffer;

    auto [buffer, err] = createBuffer(desc);
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create index buffer"));
    }

    auto indexBuffer = std::make_shared<IndexBuffer>(
        std::move(buffer), elementSize, indexCount, bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createConstantBuffer(
    const void* sourceData,
    u32 sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(sizeInBytes > 0);

    BufferDesc desc = {};
    desc.sizeInBytes = sizeInBytes;
    desc.memoryUsage = toMemoryUsage(bufferUsage);
    desc.bindFlags = BufferBindFlags::ConstantBuffer;

    auto [buffer, err] = createBuffer(
        desc, std::span<const u8>(static_cast<const u8*>(sourceData), sizeInBytes));
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create constant buffer"));
    }

    auto constantBuffer = std::make_shared<ConstantBuffer>(
        std::move(buffer), sizeInBytes, bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createConstantBuffer(
    u32 sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(sizeInBytes > 0);

    BufferDesc desc = {};
    desc.sizeInBytes = sizeInBytes;
    desc.memoryUsage = toMemoryUsage(bufferUsage);
    desc.bindFlags = BufferBindFlags::ConstantBuffer;

    auto [buffer, err] = createBuffer(desc);
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create constant buffer"));
    }

    auto constantBuffer = std::make_shared<ConstantBuffer>(
        std::move(buffer), sizeInBytes, bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createPipelineState(
    const PipelineDesc& descriptor) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);

    auto pipelineState = std::make_shared<PipelineStateVulkan>();

    if (auto err = pipelineState->initialize(device_, physicalDevice_, descriptor); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize PipelineStateVulkan"));
    }

    return std::make_tuple(std::move(pipelineState), nullptr);
}

std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createShader(
    std::span<const u8> shaderBytecode,
    const detail::ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);

    auto shader = std::make_unique<ShaderVulkan>();

    if (auto err = shader->initialize(device_, shaderBytecode, compileOptions); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize ShaderVulkan"));
    }

    return std::make_tuple(std::move(shader), nullptr);
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createRenderTarget2D(
    i32 width,
    i32 height) noexcept
{
    return createRenderTarget2D(
        width,
        height,
        false,
        PixelFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createRenderTarget2D(
    i32 width,
    i32 height,
    bool generateMipmap,
    PixelFormat format) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    const auto levelCount = generateMipmap
                                ? detail::TextureHelper::computeMipmapLevelCount(width, height)
                                : 1;

    // TODO: MSAA is not implemented yet.
    constexpr i32 multiSampleCount = 1;

    auto renderTarget = std::make_shared<RenderTarget2DVulkan>();

    // NOTE: Resolve the depth format so RT render passes match pipeline render passes
    const auto depthFormat = (presentationParams_.depthStencilFormat != PixelFormat::Invalid)
                                 ? chooseDepthStencilFormat(physicalDevice_, presentationParams_.depthStencilFormat)
                                 : VK_FORMAT_UNDEFINED;

    if (auto err = renderTarget->initialize(
            device_,
            physicalDevice_,
            commandPool_,
            graphicsQueue_,
            width,
            height,
            levelCount,
            format,
            multiSampleCount,
            depthFormat);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize RenderTarget2DVulkan"));
    }

    return std::make_tuple(std::move(renderTarget), nullptr);
}

std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createDepthStencilBuffer(
    i32 width,
    i32 height,
    PixelFormat depthStencilFormat) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    // TODO: MSAA is not implemented yet.
    constexpr i32 multiSampleCount = 1;

    auto depthStencilBuffer = std::make_shared<DepthStencilBufferVulkan>();

    if (auto err = depthStencilBuffer->initialize(
            device_,
            physicalDevice_,
            width,
            height,
            depthStencilFormat,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize DepthStencilBufferVulkan"));
    }

    return std::make_tuple(std::move(depthStencilBuffer), nullptr);
}

std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createSamplerState(
    const SamplerDesc& descriptor) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);

    auto samplerState = std::make_shared<SamplerStateVulkan>();

    if (auto err = samplerState->initialize(device_, descriptor); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize SamplerStateVulkan"));
    }

    return std::make_tuple(std::move(samplerState), nullptr);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createTexture2D(
    i32 width,
    i32 height) noexcept
{
    return createTexture2D(
        width,
        height,
        false,
        PixelFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createTexture2D(
    i32 width,
    i32 height,
    bool mipMap,
    PixelFormat format) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    const auto levelCount = mipMap
                                ? detail::TextureHelper::computeMipmapLevelCount(width, height)
                                : 1;

    auto texture = std::make_shared<Texture2DVulkan>();

    if (auto err = texture->initialize(
            device_,
            physicalDevice_,
            commandPool_,
            graphicsQueue_,
            width,
            height,
            levelCount,
            format);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize Texture2DVulkan"));
    }

    return std::make_tuple(std::move(texture), nullptr);
}

std::tuple<std::shared_ptr<gpu::Texture>, std::unique_ptr<Error>>
GraphicsDeviceVulkanImpl::createTexture(const TextureDesc& desc) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(desc.width > 0);
    POMDOG_ASSERT(desc.height > 0);

    const auto hasRenderTarget = (static_cast<u8>(desc.usage) & static_cast<u8>(TextureUsage::RenderTarget)) != 0;
    const auto hasDepthStencil = (static_cast<u8>(desc.usage) & static_cast<u8>(TextureUsage::DepthStencil)) != 0;

    if (hasDepthStencil) {
        // TODO: MSAA is not implemented yet.
        constexpr i32 multiSampleCount = 1;

        auto depthStencilBuffer = std::make_shared<DepthStencilBufferVulkan>();
        if (auto err = depthStencilBuffer->initialize(
                device_,
                physicalDevice_,
                desc.width,
                desc.height,
                desc.format,
                multiSampleCount);
            err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize DepthStencilBufferVulkan"));
        }
        return std::make_tuple(std::move(depthStencilBuffer), nullptr);
    }

    if (hasRenderTarget) {
        // TODO: MSAA is not implemented yet.
        constexpr i32 multiSampleCount = 1;

        auto renderTarget = std::make_shared<RenderTarget2DVulkan>();

        const auto depthFormat = (presentationParams_.depthStencilFormat != PixelFormat::Invalid)
                                     ? chooseDepthStencilFormat(physicalDevice_, presentationParams_.depthStencilFormat)
                                     : VK_FORMAT_UNDEFINED;

        if (auto err = renderTarget->initialize(
                device_,
                physicalDevice_,
                commandPool_,
                graphicsQueue_,
                desc.width,
                desc.height,
                desc.mipLevels,
                desc.format,
                multiSampleCount,
                depthFormat);
            err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize RenderTarget2DVulkan"));
        }
        return std::make_tuple(std::move(renderTarget), nullptr);
    }

    auto texture = std::make_shared<Texture2DVulkan>();
    if (auto err = texture->initialize(
            device_,
            physicalDevice_,
            commandPool_,
            graphicsQueue_,
            desc.width,
            desc.height,
            desc.mipLevels,
            desc.format);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize Texture2DVulkan"));
    }
    return std::make_tuple(std::move(texture), nullptr);
}

VkDevice GraphicsDeviceVulkanImpl::getDevice() const noexcept
{
    return device_;
}

VkQueue GraphicsDeviceVulkanImpl::getGraphicsQueue() const noexcept
{
    return graphicsQueue_;
}

VkInstance GraphicsDeviceVulkanImpl::getInstance() const noexcept
{
    return instance_;
}

VkPhysicalDevice GraphicsDeviceVulkanImpl::getPhysicalDevice() const noexcept
{
    return physicalDevice_;
}

VkCommandPool GraphicsDeviceVulkanImpl::getCommandPool() const noexcept
{
    return commandPool_;
}

u32 GraphicsDeviceVulkanImpl::getGraphicsQueueFamilyIndex() const noexcept
{
    return graphicsQueueFamilyIndex_;
}

void GraphicsDeviceVulkanImpl::setSwapChain(unsafe_ptr<SwapChainVulkan> swapChain) noexcept
{
    swapChain_ = swapChain;
}

void GraphicsDeviceVulkanImpl::setPresentationParameters(const PresentationParameters& params) noexcept
{
    presentationParams_ = params;
}

} // namespace

std::tuple<std::shared_ptr<GraphicsDeviceVulkan>, std::unique_ptr<Error>>
GraphicsDeviceVulkan::create() noexcept
{
    auto device = std::make_shared<GraphicsDeviceVulkanImpl>();
    if (auto err = device->initialize(); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize GraphicsDeviceVulkanImpl"));
    }
    return std::make_tuple(std::move(device), nullptr);
}

} // namespace pomdog::gpu::detail::vulkan
