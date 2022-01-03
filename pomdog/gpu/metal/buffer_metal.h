// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/buffer_bind_mode.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/MTLBuffer.h>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::metal {
class FrameCounter;
} // namespace pomdog::gpu::detail::metal

namespace pomdog::gpu::detail::metal {

class BufferMetal final : public Buffer {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        std::shared_ptr<const FrameCounter> frameCounter,
        id<MTLDevice> device,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        std::shared_ptr<const FrameCounter> frameCounter,
        id<MTLDevice> device,
        const void* vertices,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) noexcept;

    void GetData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const override;

    void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t sizeInBytes) override;

    /// Gets the pointer of the native buffer.
    [[nodiscard]] id<MTLBuffer> GetBuffer() const noexcept;

private:
    std::vector<id<MTLBuffer>> buffers_;
    std::shared_ptr<const FrameCounter> frameCounter_;
};

} // namespace pomdog::gpu::detail::metal
