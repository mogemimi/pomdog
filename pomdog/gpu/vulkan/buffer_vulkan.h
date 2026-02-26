// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/backends/buffer_bind_mode.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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

    void getData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const override;

    void setData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t sizeInBytes) override;

    [[nodiscard]] VkBuffer
    getBuffer() const;

private:
    ::VkDevice device;
    ::VkBuffer nativeBuffer;
    ::VkDeviceMemory nativeBufferMemory;
};

std::tuple<VkBuffer, VkDeviceMemory>
createBuffer(
    ::VkDevice device,
    ::VkPhysicalDevice physicalDevice,
    std::size_t sizeInBytes,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags propertyFlags);

} // namespace pomdog::gpu::detail::vulkan
