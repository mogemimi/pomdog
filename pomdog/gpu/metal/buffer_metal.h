// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/backends/buffer_bind_mode.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/MTLBuffer.h>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
enum class BufferUsage : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::metal {
class FrameCounter;
} // namespace pomdog::gpu::detail::metal

namespace pomdog::gpu::detail::metal {

class BufferMetal final : public Buffer {
public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::shared_ptr<const FrameCounter> frameCounter,
        id<MTLDevice> device,
        u32 sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::shared_ptr<const FrameCounter> frameCounter,
        id<MTLDevice> device,
        const void* vertices,
        u32 sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) noexcept;

    void getData(
        u32 offsetInBytes,
        void* destination,
        u32 sizeInBytes) const override;

    void setData(
        u32 offsetInBytes,
        const void* source,
        u32 sizeInBytes) override;

    /// Gets the pointer of the native buffer.
    [[nodiscard]] id<MTLBuffer>
    getBuffer() const noexcept;

private:
    std::vector<id<MTLBuffer>> buffers_;
    std::shared_ptr<const FrameCounter> frameCounter_;
};

} // namespace pomdog::gpu::detail::metal
