// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "BufferVulkan.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Vulkan {
namespace {

//VkSharingMode ToSharingMode(BufferUsage bufferUsage)
//{
//    switch (bufferUsage) {
//    case BufferUsage::Immutable: return ;
//    case BufferUsage::Dynamic: return ;
//    }
//    return VK_SHARING_MODE_EXCLUSIVE;
//}

VkBufferCreateInfo ToVkBufferCreateInfo(
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    VkBufferUsageFlags usageFlags)
{
    VkBufferCreateInfo info;
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.size = sizeInBytes;
    info.usage = usageFlags;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    return info;
}

} // unnamed namespace

BufferVulkan::BufferVulkan(
    ::VkDevice deviceIn,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    VkBufferUsageFlags usageFlags)
    : device(deviceIn)
    , nativeBuffer(nullptr)
    , deviceMemory(nullptr)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(usageFlags == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        || usageFlags == VK_BUFFER_USAGE_INDEX_BUFFER_BIT
        || usageFlags == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    const auto info = ToVkBufferCreateInfo(sizeInBytes, bufferUsage, usageFlags);
    const auto result = vkCreateBuffer(device, &info, nullptr, &nativeBuffer);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create VkBuffer");
    }
}

BufferVulkan::BufferVulkan(
    ::VkDevice deviceIn,
    const void* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    VkBufferUsageFlags usageFlags)
    : device(deviceIn)
    , nativeBuffer(nullptr)
    , deviceMemory(nullptr)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(usageFlags == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        || usageFlags == VK_BUFFER_USAGE_INDEX_BUFFER_BIT
        || usageFlags == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    auto info = ToVkBufferCreateInfo(sizeInBytes, bufferUsage, usageFlags);
    auto result = vkCreateBuffer(device, &info, nullptr, &nativeBuffer);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create VkBuffer");
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, nativeBuffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = 0;

    result = vkAllocateMemory(device, &allocInfo, nullptr, &deviceMemory);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to call vkAllocateMemory()");
    }

    std::uint8_t* dataBegin = nullptr;
    result = vkMapMemory(device, deviceMemory, 0, memoryRequirements.size, 0,
        reinterpret_cast<void**>(&dataBegin));

    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to map buffer");
    }

    std::memcpy(dataBegin, sourceData, sizeInBytes);
    vkUnmapMemory(device, deviceMemory);

    result = vkBindBufferMemory(device, nativeBuffer, deviceMemory, 0);

    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to call vkBindBufferMemory()");
    }
}

BufferVulkan::~BufferVulkan()
{
    if (nativeBuffer != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkDestroyBuffer(device, nativeBuffer, nullptr);
    }
    if (deviceMemory != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkFreeMemory(device, deviceMemory, nullptr);
    }
}

void BufferVulkan::GetData(
    std::size_t offsetInBytes, void* destination, std::size_t sizeInBytes) const
{
    POMDOG_ASSERT(nativeBuffer != nullptr);

    // FUS RO DAH!
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void BufferVulkan::SetData(
    std::size_t offsetInBytes, const void* source, std::size_t sizeInBytes)
{
    POMDOG_ASSERT(nativeBuffer != nullptr);

    // FUS RO DAH!
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

VkBuffer BufferVulkan::GetBuffer() const
{
    return nativeBuffer;
}

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
