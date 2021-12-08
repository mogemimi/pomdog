// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/backends/buffer_bind_mode.hpp"
#include "pomdog/graphics/backends/native_buffer.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/utility/errors.hpp"
#import <Metal/MTLBuffer.h>

namespace pomdog::detail::metal {

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

} // namespace pomdog::detail::metal
