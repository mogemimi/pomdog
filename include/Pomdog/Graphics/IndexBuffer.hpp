// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/IndexElementSize.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class POMDOG_EXPORT IndexBuffer final {
public:
    IndexBuffer() = delete;
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&&) = default;

    IndexBuffer(
        std::unique_ptr<Detail::NativeBuffer>&& nativeBuffer,
        IndexElementSize elementSize,
        std::size_t indexCount,
        BufferUsage bufferUsage);

    ~IndexBuffer();

    IndexBuffer& operator=(const IndexBuffer&) = delete;
    IndexBuffer& operator=(IndexBuffer&&) = default;

    /// Gets the number of indices.
    std::size_t GetIndexCount() const noexcept;

    /// Gets the size in bytes of per-index element.
    IndexElementSize GetElementSize() const noexcept;

    /// Gets the size in bytes of this index buffer.
    std::size_t GetSizeInBytes() const noexcept;

    /// Gets the expected usage hint of this index buffer.
    BufferUsage GetBufferUsage() const noexcept;

    /// Sets index buffer data.
    void SetData(const void* source, std::size_t elementCount);

    /// Sets index buffer data.
    void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t elementCount);

    /// Gets the pointer of the native index buffer resource.
    Detail::NativeBuffer* GetNativeBuffer();

private:
    std::unique_ptr<Detail::NativeBuffer> nativeBuffer;
    std::uint32_t indexCount;
    IndexElementSize elementSize;
    BufferUsage bufferUsage;
};

} // namespace Pomdog
