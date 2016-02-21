// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/buffer_vulkan.h"
#include "pomdog/gpu/buffer_bind_flags.h"
#include "pomdog/gpu/buffer_desc.h"
#include "pomdog/gpu/memory_usage.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstring>
#include <optional>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::vulkan {
namespace {

[[nodiscard]] std::optional<u32>
findMemoryType(VkPhysicalDevice device, u32 typeFilter, VkMemoryPropertyFlags propertyFlags) noexcept
{
    VkPhysicalDeviceMemoryProperties props;
    vkGetPhysicalDeviceMemoryProperties(device, &props);

    for (u32 i = 0; i < props.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && ((props.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)) {
            return i;
        }
    }
    return std::nullopt;
}

[[nodiscard]] std::tuple<VkBuffer, VkDeviceMemory, std::unique_ptr<Error>>
createNativeBuffer(
    ::VkDevice device,
    ::VkPhysicalDevice physicalDevice,
    u32 sizeInBytes,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags propertyFlags) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(physicalDevice != nullptr);

    VkBufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = sizeInBytes;
    info.usage = usageFlags;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer = nullptr;
    auto result = vkCreateBuffer(device, &info, nullptr, &buffer);
    if (result != VK_SUCCESS) {
        return std::make_tuple(nullptr, nullptr, errors::make("failed to create VkBuffer"));
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

    const auto memoryTypeIndex = findMemoryType(
        physicalDevice,
        memoryRequirements.memoryTypeBits,
        propertyFlags);
    if (memoryTypeIndex == std::nullopt) {
        vkDestroyBuffer(device, buffer, nullptr);
        return std::make_tuple(nullptr, nullptr, errors::make("failed to find suitable memory type for VkBuffer"));
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = *memoryTypeIndex;

    VkDeviceMemory bufferMemory = nullptr;
    result = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
    if (result != VK_SUCCESS) {
        vkDestroyBuffer(device, buffer, nullptr);
        return std::make_tuple(nullptr, nullptr, errors::make("failed to allocate buffer memory"));
    }

    result = vkBindBufferMemory(device, buffer, bufferMemory, 0);
    if (result != VK_SUCCESS) {
        vkFreeMemory(device, bufferMemory, nullptr);
        vkDestroyBuffer(device, buffer, nullptr);
        return std::make_tuple(nullptr, nullptr, errors::make("failed to bind buffer memory"));
    }

    return std::make_tuple(buffer, bufferMemory, nullptr);
}

} // namespace

BufferVulkan::~BufferVulkan()
{
    if (nativeBuffer_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkDestroyBuffer(device_, nativeBuffer_, nullptr);
    }
    if (nativeBufferMemory_ != nullptr) {
        POMDOG_ASSERT(device_ != nullptr);
        vkFreeMemory(device_, nativeBufferMemory_, nullptr);
    }
}

void BufferVulkan::getData(
    u32 offsetInBytes, std::span<u8> destination) const
{
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    POMDOG_ASSERT(device_ != nullptr);

    void* data = nullptr;
    auto result = vkMapMemory(device_, nativeBufferMemory_, offsetInBytes, destination.size(), 0, &data);
    if (result != VK_SUCCESS) {
        return;
    }

    std::memcpy(destination.data(), data, destination.size());
    vkUnmapMemory(device_, nativeBufferMemory_);
}

void BufferVulkan::setData(
    u32 offsetInBytes, std::span<const u8> source)
{
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    POMDOG_ASSERT(device_ != nullptr);

    void* data = nullptr;
    auto result = vkMapMemory(device_, nativeBufferMemory_, offsetInBytes, source.size(), 0, &data);
    if (result != VK_SUCCESS) {
        return;
    }

    std::memcpy(data, source.data(), source.size());
    vkUnmapMemory(device_, nativeBufferMemory_);
}

VkBuffer
BufferVulkan::getBuffer() const noexcept
{
    return nativeBuffer_;
}

std::unique_ptr<Error>
BufferVulkan::initialize(
    ::VkDevice device,
    ::VkPhysicalDevice physicalDevice,
    const BufferDesc& desc,
    std::span<const u8> initialData) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(physicalDevice != nullptr);

    if (!initialData.empty()) {
        if (initialData.size() < desc.sizeInBytes) {
            return errors::make("initialData size must be >= desc.sizeInBytes");
        }
    }

    device_ = device;
    nativeBuffer_ = nullptr;
    nativeBufferMemory_ = nullptr;

    VkBufferUsageFlags usageFlags = 0;
    if (hasFlag(desc.bindFlags, BufferBindFlags::VertexBuffer)) {
        usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    if (hasFlag(desc.bindFlags, BufferBindFlags::IndexBuffer)) {
        usageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }
    if (hasFlag(desc.bindFlags, BufferBindFlags::ConstantBuffer)) {
        usageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }

    // NOTE: All buffers use HOST_VISIBLE memory for simplicity.
    // A future optimization could use DEVICE_LOCAL memory with a staging buffer
    // for GpuOnly buffers.
    const VkMemoryPropertyFlags memoryProperties =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    {
        std::unique_ptr<Error> err;
        std::tie(nativeBuffer_, nativeBufferMemory_, err) = createNativeBuffer(
            device_,
            physicalDevice,
            desc.sizeInBytes,
            usageFlags,
            memoryProperties);
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create native buffer");
        }
    }

    if (!initialData.empty()) {
        void* data = nullptr;
        auto result = vkMapMemory(device_, nativeBufferMemory_, 0, desc.sizeInBytes, 0, &data);
        if (result != VK_SUCCESS) {
            return errors::make("failed to map buffer memory for initial data upload");
        }
        std::memcpy(data, initialData.data(), desc.sizeInBytes);
        vkUnmapMemory(device_, nativeBufferMemory_);
    }

    return nullptr;
}

std::span<u8> BufferVulkan::map(u32 offsetInBytes, u32 sizeInBytes)
{
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    POMDOG_ASSERT(device_ != nullptr);

    void* data = nullptr;
    auto result = vkMapMemory(device_, nativeBufferMemory_, offsetInBytes, sizeInBytes, 0, &data);
    if (result != VK_SUCCESS) {
        return {};
    }
    return std::span<u8>{static_cast<u8*>(data), sizeInBytes};
}

void BufferVulkan::unmap()
{
    POMDOG_ASSERT(device_ != nullptr);
    vkUnmapMemory(device_, nativeBufferMemory_);
}

} // namespace pomdog::gpu::detail::vulkan
