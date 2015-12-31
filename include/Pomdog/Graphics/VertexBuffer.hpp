// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT VertexBuffer {
public:
    VertexBuffer() = delete;
    VertexBuffer(VertexBuffer const&) = delete;
    VertexBuffer(VertexBuffer &&) = default;

    VertexBuffer(GraphicsDevice & graphicsDevice,
        void const* vertices, std::size_t vertexCount,
        std::size_t strideInBytes, BufferUsage bufferUsage);

    VertexBuffer(GraphicsDevice & graphicsDevice,
        std::size_t vertexCount, std::size_t strideInBytes,
        BufferUsage bufferUsage);

    VertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        void const* vertices, std::size_t vertexCount,
        std::size_t strideBytes, BufferUsage bufferUsage);

    VertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        std::size_t vertexCount, std::size_t strideBytes,
        BufferUsage bufferUsage);

    ~VertexBuffer();

    VertexBuffer & operator=(VertexBuffer const&) = delete;
    VertexBuffer & operator=(VertexBuffer &&) = default;

    std::size_t GetVertexCount() const noexcept;

    std::size_t GetStrideBytes() const noexcept;

    std::size_t GetSizeInBytes() const noexcept;

    BufferUsage GetBufferUsage() const noexcept;

    void SetData(void const* source, std::size_t elementCount);

    void SetData(std::size_t offsetInBytes, void const* source,
        std::size_t elementCount, std::size_t strideInBytes);

    Detail::NativeBuffer* GetNativeVertexBuffer();

private:
    std::unique_ptr<Detail::NativeBuffer> nativeVertexBuffer;
    std::uint32_t vertexCount;
    std::uint32_t strideInBytes;
    BufferUsage bufferUsage;
};

} // namespace Pomdog
