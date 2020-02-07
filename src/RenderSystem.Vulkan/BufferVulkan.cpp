// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "BufferVulkan.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <optional>
#include <utility>

namespace Pomdog::Detail::Vulkan {
namespace {

VkBufferUsageFlags ToVkBufferUsageFlags(BufferBindMode bindMode) noexcept
{
    switch (bindMode) {
    case BufferBindMode::VertexBuffer:
        return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    case BufferBindMode::IndexBuffer:
        return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    case BufferBindMode::ConstantBuffer:
        return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }
    return 0;
}

std::optional<std::uint32_t>
FindMemoryType(VkPhysicalDevice device, std::uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) noexcept
{
    VkPhysicalDeviceMemoryProperties props;
    vkGetPhysicalDeviceMemoryProperties(device, &props);

    for (std::uint32_t i = 0; i < props.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) && ((props.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)) {
            return i;
        }
    }
    return std::nullopt;
}

void CopyBuffer(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue graphicsQueue,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize size)
{
    VkCommandBufferAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion;
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

} // namespace

std::tuple<VkBuffer, VkDeviceMemory>
CreateBuffer(
    ::VkDevice device,
    ::VkPhysicalDevice physicalDevice,
    std::size_t sizeInBytes,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags propertyFlags)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(physicalDevice != nullptr);

    VkBufferCreateInfo info;
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.size = sizeInBytes;
    info.usage = usageFlags;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;

    VkBuffer buffer = nullptr;
    VkDeviceMemory bufferMemory = nullptr;

    auto result = vkCreateBuffer(device, &info, nullptr, &buffer);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create VkBuffer");
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

    const auto memoryTypeIndex = FindMemoryType(
        physicalDevice,
        memoryRequirements.memoryTypeBits,
        propertyFlags);
    if (memoryTypeIndex == std::nullopt) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to find suitable memory type");
    }

    VkMemoryAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = *memoryTypeIndex;

    result = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to allocate buffer memory");
    }

    result = vkBindBufferMemory(device, buffer, bufferMemory, 0);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to call vkBindBufferMemory()");
    }

    return std::make_tuple(buffer, bufferMemory);
}

BufferVulkan::BufferVulkan(
    ::VkDevice deviceIn,
    ::VkPhysicalDevice physicalDevice,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    BufferBindMode bindMode)
    : BufferVulkan(
        deviceIn,
        physicalDevice,
        nullptr,
        sizeInBytes,
        bufferUsage,
        bindMode)
{
}

BufferVulkan::BufferVulkan(
    ::VkDevice deviceIn,
    ::VkPhysicalDevice physicalDevice,
    const void* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    BufferBindMode bindMode)
    : device(deviceIn)
    , nativeBuffer(nullptr)
    , nativeBufferMemory(nullptr)
{
    POMDOG_ASSERT(device != nullptr);

    const auto usageFlags = ToVkBufferUsageFlags(bindMode);

    switch (bufferUsage) {
    case BufferUsage::Immutable:
        std::tie(nativeBuffer, nativeBufferMemory) = CreateBuffer(
            device,
            physicalDevice,
            sizeInBytes,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | usageFlags,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        break;
    case BufferUsage::Dynamic:
        std::tie(nativeBuffer, nativeBufferMemory) = CreateBuffer(
            device,
            physicalDevice,
            sizeInBytes,
            usageFlags,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        break;
    }

    ::VkBuffer stagingBuffer = nullptr;
    ::VkDeviceMemory stagingBufferMemory = nullptr;
    ::VkDeviceMemory mappedBufferMemory = nullptr;

    switch (bufferUsage) {
    case BufferUsage::Immutable:
        std::tie(stagingBuffer, stagingBufferMemory) = CreateBuffer(
            device,
            physicalDevice,
            sizeInBytes,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        mappedBufferMemory = stagingBufferMemory;
        break;
    case BufferUsage::Dynamic:
        mappedBufferMemory = nativeBufferMemory;
        break;
    }

    if (sourceData != nullptr) {
        void* data = nullptr;
        auto result = vkMapMemory(device, mappedBufferMemory, 0, sizeInBytes, 0, &data);
        if (result != VK_SUCCESS) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to map buffer");
        }

        std::memcpy(data, sourceData, sizeInBytes);
        vkUnmapMemory(device, mappedBufferMemory);
    }

    if (bufferUsage == BufferUsage::Immutable) {
        // FIXME: Not implemented yet.
        VkCommandPool commandPool = nullptr;
        VkQueue graphicsQueue = nullptr;
        CopyBuffer(device, commandPool, graphicsQueue, stagingBuffer, nativeBuffer, sizeInBytes);

        POMDOG_ASSERT(stagingBuffer != nullptr);
        POMDOG_ASSERT(stagingBufferMemory != nullptr);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }
}

BufferVulkan::~BufferVulkan()
{
    if (nativeBuffer != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkDestroyBuffer(device, nativeBuffer, nullptr);
    }
    if (nativeBufferMemory != nullptr) {
        POMDOG_ASSERT(device != nullptr);
        vkFreeMemory(device, nativeBufferMemory, nullptr);
    }
}

void BufferVulkan::GetData(
    std::size_t offsetInBytes, void* destination, std::size_t sizeInBytes) const
{
    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT(device != nullptr);

    void* data = nullptr;
    auto result = vkMapMemory(device, nativeBufferMemory, offsetInBytes, sizeInBytes, 0, &data);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to map buffer");
    }

    std::memcpy(destination, data, sizeInBytes);
    vkUnmapMemory(device, nativeBufferMemory);
}

void BufferVulkan::SetData(
    std::size_t offsetInBytes, const void* source, std::size_t sizeInBytes)
{
    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT(device != nullptr);

    void* data = nullptr;
    auto result = vkMapMemory(device, nativeBufferMemory, offsetInBytes, sizeInBytes, 0, &data);
    if (result != VK_SUCCESS) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to map buffer");
    }

    std::memcpy(data, source, sizeInBytes);
    vkUnmapMemory(device, nativeBufferMemory);
}

VkBuffer BufferVulkan::GetBuffer() const
{
    return nativeBuffer;
}

} // namespace Pomdog::Detail::Vulkan
