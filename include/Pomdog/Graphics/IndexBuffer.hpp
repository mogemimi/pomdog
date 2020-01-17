// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/IndexElementSize.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT IndexBuffer final {
public:
    IndexBuffer() = delete;
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&&) = default;

    IndexBuffer(
        GraphicsDevice& graphicsDevice,
        IndexElementSize elementSize,
        const void* indices,
        std::size_t indexCount,
        BufferUsage bufferUsage);

    IndexBuffer(
        GraphicsDevice& graphicsDevice,
        IndexElementSize elementSize,
        std::size_t indexCount,
        BufferUsage bufferUsage);

    IndexBuffer(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        IndexElementSize elementSize,
        const void* indices,
        std::size_t indexCount,
        BufferUsage bufferUsage);

    IndexBuffer(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
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
    Detail::NativeBuffer* GetNativeIndexBuffer();

private:
    std::unique_ptr<Detail::NativeBuffer> nativeIndexBuffer;
    std::uint32_t indexCount;
    IndexElementSize elementSize;
    BufferUsage bufferUsage;
};

} // namespace Pomdog
