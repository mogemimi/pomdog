// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/memory/unsafe_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class Buffer;
} // namespace pomdog::gpu

namespace pomdog::gpu {

class POMDOG_EXPORT VertexBuffer final {
public:
    VertexBuffer() = delete;
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&&) = default;

    VertexBuffer(
        std::unique_ptr<Buffer>&& nativeBuffer,
        u32 vertexCount,
        u32 strideInBytes,
        BufferUsage bufferUsage);

    ~VertexBuffer();

    VertexBuffer& operator=(const VertexBuffer&) = delete;
    VertexBuffer& operator=(VertexBuffer&&) = default;

    /// Gets the number of vertices.
    [[nodiscard]] u32
    getVertexCount() const noexcept;

    /// Gets the size in bytes of per-vertex data.
    [[nodiscard]] u32
    getStrideBytes() const noexcept;

    /// Gets the size in bytes of this vertex buffer.
    [[nodiscard]] u32
    getSizeInBytes() const noexcept;

    /// Gets the expected usage hint of this vertex buffer.
    [[nodiscard]] BufferUsage
    getBufferUsage() const noexcept;

    /// Sets vertex buffer data.
    void setData(const void* source, u32 elementCount);

    /// Sets vertex buffer data.
    void setData(
        u32 offsetInBytes,
        const void* source,
        u32 elementCount,
        u32 strideInBytes);

    /// Gets the pointer of the native vertex buffer resource.
    [[nodiscard]] unsafe_ptr<Buffer>
    getBuffer();

private:
    std::unique_ptr<Buffer> nativeBuffer_;
    u32 vertexCount_;
    u32 strideInBytes_;
    BufferUsage bufferUsage_;
};

} // namespace pomdog::gpu
