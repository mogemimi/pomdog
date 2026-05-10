// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/MTLBuffer.h>
#include <memory>
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
enum class MemoryUsage : u8;
struct BufferDesc;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::metal {
class FrameCounter;
} // namespace pomdog::gpu::detail::metal

namespace pomdog::gpu::detail::metal {

/// BufferMetal is the Metal implementation of Buffer using triple-buffered MTLBuffer resources.
class BufferMetal final : public Buffer {
private:
    std::vector<id<MTLBuffer>> buffers_;
    std::shared_ptr<const FrameCounter> frameCounter_;

public:
    /// Creates the Metal buffer(s) from the descriptor and optional initial data.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::shared_ptr<const FrameCounter> frameCounter,
        id<MTLDevice> device,
        const BufferDesc& desc,
        std::span<const u8> initialData) noexcept;

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

    /// Gets the pointer of the native buffer.
    [[nodiscard]] id<MTLBuffer>
    getBuffer() const noexcept;
};

} // namespace pomdog::gpu::detail::metal
