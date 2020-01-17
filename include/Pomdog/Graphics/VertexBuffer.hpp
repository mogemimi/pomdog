// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT VertexBuffer final {
public:
    VertexBuffer() = delete;
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer(VertexBuffer&&) = default;

    VertexBuffer(
        GraphicsDevice& graphicsDevice,
        const void* vertices,
        std::size_t vertexCount,
        std::size_t strideInBytes,
        BufferUsage bufferUsage);

    VertexBuffer(
        GraphicsDevice& graphicsDevice,
        std::size_t vertexCount,
        std::size_t strideInBytes,
        BufferUsage bufferUsage);

    VertexBuffer(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const void* vertices,
        std::size_t vertexCount,
        std::size_t strideBytes,
        BufferUsage bufferUsage);

    VertexBuffer(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        std::size_t vertexCount,
        std::size_t strideBytes,
        BufferUsage bufferUsage);

    ~VertexBuffer();

    VertexBuffer& operator=(const VertexBuffer&) = delete;
    VertexBuffer& operator=(VertexBuffer&&) = default;

    /// Gets the number of vertices.
    std::size_t GetVertexCount() const noexcept;

    /// Gets the size in bytes of per-vertex data.
    std::size_t GetStrideBytes() const noexcept;

    /// Gets the size in bytes of this vertex buffer.
    std::size_t GetSizeInBytes() const noexcept;

    /// Gets the expected usage hint of this vertex buffer.
    BufferUsage GetBufferUsage() const noexcept;

    /// Sets vertex buffer data.
    void SetData(const void* source, std::size_t elementCount);

    /// Sets vertex buffer data.
    void SetData(
        std::size_t offsetInBytes,
        const void* source,
        std::size_t elementCount,
        std::size_t strideInBytes);

    /// Gets the pointer of the native vertex buffer resource.
    Detail::NativeBuffer* GetNativeVertexBuffer();

private:
    std::unique_ptr<Detail::NativeBuffer> nativeVertexBuffer;
    std::uint32_t vertexCount;
    std::uint32_t strideInBytes;
    BufferUsage bufferUsage;
};

} // namespace Pomdog
