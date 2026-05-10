// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/gpu/vulkan/prerequisites_vulkan.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
enum class MemoryUsage : u8;
struct BufferDesc;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::vulkan {

/// BufferVulkan is the Vulkan implementation of Buffer.
class BufferVulkan final : public Buffer {
private:
    ::VkDevice device_ = nullptr;
    ::VkBuffer nativeBuffer_ = nullptr;
    ::VkDeviceMemory nativeBufferMemory_ = nullptr;

public:
    /// Creates a VkBuffer and allocates VkDeviceMemory for the given descriptor.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        const BufferDesc& desc,
        std::span<const u8> initialData) noexcept;

    ~BufferVulkan();

    /// Reads data from the buffer into the destination span.
    void getData(
        u32 offsetInBytes,
        std::span<u8> destination) const override;

    /// Writes data from the source span into the buffer.
    void setData(
        u32 offsetInBytes,
        std::span<const u8> source) override;

    /// Maps a region of the buffer into CPU-accessible memory and returns a writable span.
    [[nodiscard]] std::span<u8>
    map(u32 offsetInBytes, u32 sizeInBytes) override;

    /// Unmaps the CPU-accessible memory region previously returned by map().
    void unmap() override;

    /// Returns the VkBuffer handle.
    [[nodiscard]] VkBuffer
    getBuffer() const noexcept;
};

} // namespace pomdog::gpu::detail::vulkan
