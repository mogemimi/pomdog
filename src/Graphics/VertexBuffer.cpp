// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
VertexBuffer::VertexBuffer(GraphicsDevice & graphicsDevice,
    void const* vertices,
    std::size_t vertexCountIn,
    std::size_t strideInBytesIn,
    Pomdog::BufferUsage bufferUsageIn)
    : vertexCount(static_cast<decltype(vertexCount)>(vertexCountIn))
    , strideInBytes(static_cast<decltype(strideInBytes)>(strideInBytesIn))
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(vertices != nullptr);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideInBytes > 0);

    auto sizeInBytes = vertexCount * strideInBytes;
    auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice != nullptr);
    using Detail::RenderSystem::BufferBindMode;

    nativeVertexBuffer = nativeDevice->CreateBuffer(
        vertices, sizeInBytes, bufferUsage, BufferBindMode::VertexBuffer);

    POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
VertexBuffer::VertexBuffer(GraphicsDevice & graphicsDevice,
    std::size_t vertexCountIn,
    std::size_t strideInBytesIn,
    Pomdog::BufferUsage bufferUsageIn)
    : vertexCount(static_cast<decltype(vertexCount)>(vertexCountIn))
    , strideInBytes(static_cast<decltype(strideInBytes)>(strideInBytesIn))
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideInBytes > 0);

    auto sizeInBytes = vertexCount * strideInBytes;
    auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice != nullptr);
    using Detail::RenderSystem::BufferBindMode;

    nativeVertexBuffer = nativeDevice->CreateBuffer(
        sizeInBytes, bufferUsage, BufferBindMode::VertexBuffer);

    POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
VertexBuffer::VertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    void const* vertices, std::size_t vertexCountIn,
    std::size_t strideBytesIn, Pomdog::BufferUsage bufferUsageIn)
    : VertexBuffer(*graphicsDevice, vertices, vertexCountIn, strideBytesIn, bufferUsageIn)
{
    POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
VertexBuffer::VertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    std::size_t vertexCountIn, std::size_t strideBytesIn, Pomdog::BufferUsage bufferUsageIn)
    : VertexBuffer(*graphicsDevice, vertexCountIn, strideBytesIn, bufferUsageIn)
{
    POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
VertexBuffer::~VertexBuffer() = default;
//-----------------------------------------------------------------------
std::size_t VertexBuffer::StrideBytes() const noexcept
{
    return strideInBytes;
}
//-----------------------------------------------------------------------
std::size_t VertexBuffer::VertexCount() const noexcept
{
    return vertexCount;
}
//-----------------------------------------------------------------------
std::size_t VertexBuffer::SizeInBytes() const noexcept
{
    POMDOG_ASSERT(strideInBytes > 0);
    POMDOG_ASSERT(vertexCount > 0);
    return vertexCount * strideInBytes;
}
//-----------------------------------------------------------------------
BufferUsage VertexBuffer::BufferUsage() const noexcept
{
    return bufferUsage;
}
//-----------------------------------------------------------------------
void VertexBuffer::SetData(void const* source, std::size_t elementCount)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCount > 0);
    POMDOG_ASSERT(elementCount <= vertexCount);
    POMDOG_ASSERT(nativeVertexBuffer);
    POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
    nativeVertexBuffer->SetData(0, source, elementCount * strideInBytes);
}
//-----------------------------------------------------------------------
void VertexBuffer::SetData(std::size_t offsetInBytes, void const* source,
    std::size_t elementCount, std::size_t strideInBytesIn)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCount > 0);
    POMDOG_ASSERT(elementCount <= vertexCount);
    POMDOG_ASSERT(nativeVertexBuffer);
    POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
    nativeVertexBuffer->SetData(offsetInBytes, source,
        elementCount * strideInBytesIn);
}
//-----------------------------------------------------------------------
Detail::RenderSystem::NativeBuffer* VertexBuffer::NativeVertexBuffer()
{
    POMDOG_ASSERT(nativeVertexBuffer);
    return nativeVertexBuffer.get();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
