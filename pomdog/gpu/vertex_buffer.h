// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/graphics/buffer_usage.h"
#include "pomdog/graphics/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT VertexBuffer final {
public:
    VertexBuffer() = delete;
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&&) = default;

    VertexBuffer(
        std::unique_ptr<detail::NativeBuffer>&& nativeBuffer,
        std::size_t vertexCount,
        std::size_t strideInBytes,
        BufferUsage bufferUsage);

    ~VertexBuffer();

    VertexBuffer& operator=(const VertexBuffer&) = delete;
    VertexBuffer& operator=(VertexBuffer&&) = default;

    /// Gets the number of vertices.
    [[nodiscard]] std::size_t GetVertexCount() const noexcept;

    /// Gets the size in bytes of per-vertex data.
    [[nodiscard]] std::size_t GetStrideBytes() const noexcept;

    /// Gets the size in bytes of this vertex buffer.
    [[nodiscard]] std::size_t GetSizeInBytes() const noexcept;

    /// Gets the expected usage hint of this vertex buffer.
    [[nodiscard]] BufferUsage GetBufferUsage() const noexcept;

    /// Sets vertex buffer data.
    void SetData(const void* source, std::size_t elementCount);

    /// Sets vertex buffer data.
    void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t elementCount,
        std::size_t strideInBytes);

    /// Gets the pointer of the native vertex buffer resource.
    [[nodiscard]] detail::NativeBuffer* GetNativeBuffer();

private:
    std::unique_ptr<detail::NativeBuffer> nativeBuffer;
    std::uint32_t vertexCount;
    std::uint32_t strideInBytes;
    BufferUsage bufferUsage;
};

} // namespace pomdog
