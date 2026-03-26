// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/index_format.h"
#include "pomdog/memory/unsafe_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class Buffer;
} // namespace pomdog::gpu

namespace pomdog::gpu {

class POMDOG_EXPORT IndexBuffer final {
public:
    IndexBuffer() = delete;
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&&) = default;

    IndexBuffer(
        std::shared_ptr<Buffer> nativeBuffer,
        IndexFormat elementSize,
        u32 indexCount,
        BufferUsage bufferUsage);

    ~IndexBuffer();

    IndexBuffer& operator=(const IndexBuffer&) = delete;
    IndexBuffer& operator=(IndexBuffer&&) = default;

    /// Gets the number of indices.
    [[nodiscard]] u32
    getIndexCount() const noexcept;

    /// Gets the size in bytes of per-index element.
    [[nodiscard]] IndexFormat
    getElementSize() const noexcept;

    /// Gets the size in bytes of this index buffer.
    [[nodiscard]] u32
    getSizeInBytes() const noexcept;

    /// Gets the expected usage hint of this index buffer.
    [[nodiscard]] BufferUsage
    getBufferUsage() const noexcept;

    /// Sets index buffer data.
    void setData(const void* source, u32 elementCount);

    /// Sets index buffer data.
    void setData(
        u32 offsetInBytes,
        const void* source,
        u32 elementCount);

    /// Gets the pointer of the native index buffer resource.
    [[nodiscard]] unsafe_ptr<Buffer>
    getBuffer();

private:
    std::shared_ptr<Buffer> nativeBuffer_;
    u32 indexCount_;
    IndexFormat elementSize_;
    BufferUsage bufferUsage_;
};

} // namespace pomdog::gpu
