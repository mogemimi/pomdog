// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/index_element_size.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

class POMDOG_EXPORT IndexBuffer final {
public:
    IndexBuffer() = delete;
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&&) = default;

    IndexBuffer(
        std::unique_ptr<Buffer>&& nativeBuffer,
        IndexElementSize elementSize,
        std::size_t indexCount,
        BufferUsage bufferUsage);

    ~IndexBuffer();

    IndexBuffer& operator=(const IndexBuffer&) = delete;
    IndexBuffer& operator=(IndexBuffer&&) = default;

    /// Gets the number of indices.
    [[nodiscard]] std::size_t GetIndexCount() const noexcept;

    /// Gets the size in bytes of per-index element.
    [[nodiscard]] IndexElementSize GetElementSize() const noexcept;

    /// Gets the size in bytes of this index buffer.
    [[nodiscard]] std::size_t GetSizeInBytes() const noexcept;

    /// Gets the expected usage hint of this index buffer.
    [[nodiscard]] BufferUsage GetBufferUsage() const noexcept;

    /// Sets index buffer data.
    void SetData(const void* source, std::size_t elementCount);

    /// Sets index buffer data.
    void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t elementCount);

    /// Gets the pointer of the native index buffer resource.
    [[nodiscard]] Buffer* GetBuffer();

private:
    std::unique_ptr<Buffer> nativeBuffer;
    std::uint32_t indexCount;
    IndexElementSize elementSize;
    BufferUsage bufferUsage;
};

} // namespace pomdog::gpu
