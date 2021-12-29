// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/backends/buffer_bind_mode.h"
#include "pomdog/gpu/backends/native_buffer.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/utility/errors.h"
#import <Metal/MTLBuffer.h>

namespace pomdog::gpu::detail::metal {

class BufferMetal final : public NativeBuffer {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        id<MTLDevice> device,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
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

    id<MTLBuffer> GetBuffer() const noexcept;

private:
    id<MTLBuffer> nativeBuffer = nullptr;
};

} // namespace pomdog::gpu::detail::metal
