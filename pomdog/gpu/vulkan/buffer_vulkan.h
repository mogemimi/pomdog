// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/backends/buffer_bind_mode.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/gpu/forward_declarations.h"
#include <vulkan/vulkan.h>
#include <cstddef>
#include <cstdint>
#include <tuple>

namespace pomdog::gpu::detail::vulkan {

class BufferVulkan final : public Buffer {
public:
    BufferVulkan(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode);

    BufferVulkan(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode);

    ~BufferVulkan();

    void GetData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const override;

    void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t sizeInBytes) override;

    VkBuffer GetBuffer() const;

private:
    ::VkDevice device;
    ::VkBuffer nativeBuffer;
    ::VkDeviceMemory nativeBufferMemory;
};

std::tuple<VkBuffer, VkDeviceMemory>
CreateBuffer(
    ::VkDevice device,
    ::VkPhysicalDevice physicalDevice,
    std::size_t sizeInBytes,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags propertyFlags);

} // namespace pomdog::gpu::detail::vulkan
