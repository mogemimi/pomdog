// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

class POMDOG_EXPORT VertexBuffer final {
public:
    VertexBuffer() = delete;
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&&) = default;

    VertexBuffer(
        std::unique_ptr<Buffer>&& nativeBuffer,
        std::size_t vertexCount,
        std::size_t strideInBytes,
        BufferUsage bufferUsage);

    ~VertexBuffer();

    VertexBuffer& operator=(const VertexBuffer&) = delete;
    VertexBuffer& operator=(VertexBuffer&&) = default;

    /// Gets the number of vertices.
    [[nodiscard]] std::size_t
    getVertexCount() const noexcept;

    /// Gets the size in bytes of per-vertex data.
    [[nodiscard]] std::size_t
    getStrideBytes() const noexcept;

    /// Gets the size in bytes of this vertex buffer.
    [[nodiscard]] std::size_t
    getSizeInBytes() const noexcept;

    /// Gets the expected usage hint of this vertex buffer.
    [[nodiscard]] BufferUsage
    getBufferUsage() const noexcept;

    /// Sets vertex buffer data.
    void setData(const void* source, std::size_t elementCount);

    /// Sets vertex buffer data.
    void setData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t elementCount,
        std::size_t strideInBytes);

    /// Gets the pointer of the native vertex buffer resource.
    [[nodiscard]] Buffer*
    getBuffer();

private:
    std::unique_ptr<Buffer> nativeBuffer_;
    std::uint32_t vertexCount_;
    std::uint32_t strideInBytes_;
    BufferUsage bufferUsage_;
};

} // namespace pomdog::gpu
