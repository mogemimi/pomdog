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

class BufferVulkan final : public Buffer {
private:
    ::VkDevice device_ = nullptr;
    ::VkBuffer nativeBuffer_ = nullptr;
    ::VkDeviceMemory nativeBufferMemory_ = nullptr;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        ::VkDevice device,
        ::VkPhysicalDevice physicalDevice,
        const BufferDesc& desc,
        std::span<const u8> initialData) noexcept;

    ~BufferVulkan();

    void getData(
        u32 offsetInBytes,
        std::span<u8> destination) const override;

    void setData(
        u32 offsetInBytes,
        std::span<const u8> source) override;

    [[nodiscard]] std::span<u8>
    map(u32 offsetInBytes, u32 sizeInBytes) override;

    void unmap() override;

    [[nodiscard]] VkBuffer
    getBuffer() const noexcept;
};

} // namespace pomdog::gpu::detail::vulkan
