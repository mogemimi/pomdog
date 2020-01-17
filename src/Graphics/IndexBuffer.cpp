// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "../RenderSystem/BufferBindMode.hpp"
#include "../RenderSystem/BufferHelper.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

IndexBuffer::IndexBuffer(
    GraphicsDevice& graphicsDevice,
    IndexElementSize elementSizeIn,
    const void* indices,
    std::size_t indexCountIn,
    BufferUsage bufferUsageIn)
    : indexCount(static_cast<decltype(indexCount)>(indexCountIn))
    , elementSize(elementSizeIn)
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(indexCount > 0);

    auto sizeInBytes = indexCount * Detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);
    auto nativeDevice = graphicsDevice.GetNativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice != nullptr);
    using Detail::BufferBindMode;

    nativeIndexBuffer = nativeDevice->CreateBuffer(
        indices, sizeInBytes, bufferUsage, BufferBindMode::IndexBuffer);

    POMDOG_ASSERT(nativeIndexBuffer);
}

IndexBuffer::IndexBuffer(
    GraphicsDevice& graphicsDevice,
    IndexElementSize elementSizeIn,
    std::size_t indexCountIn,
    BufferUsage bufferUsageIn)
    : indexCount(static_cast<decltype(indexCount)>(indexCountIn))
    , elementSize(elementSizeIn)
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(indexCount > 0);

    auto sizeInBytes = indexCount * Detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);
    auto nativeDevice = graphicsDevice.GetNativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice != nullptr);
    using Detail::BufferBindMode;

    nativeIndexBuffer = nativeDevice->CreateBuffer(
        sizeInBytes, bufferUsage, BufferBindMode::IndexBuffer);

    POMDOG_ASSERT(nativeIndexBuffer);
}

IndexBuffer::IndexBuffer(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    IndexElementSize elementSizeIn,
    const void* indices,
    std::size_t indexCountIn,
    BufferUsage bufferUsageIn)
    : IndexBuffer(*graphicsDevice, elementSizeIn, indices, indexCountIn, bufferUsageIn)
{
    POMDOG_ASSERT(nativeIndexBuffer);
}

IndexBuffer::IndexBuffer(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    IndexElementSize elementSizeIn,
    std::size_t indexCountIn,
    BufferUsage bufferUsageIn)
    : IndexBuffer(*graphicsDevice, elementSizeIn, indexCountIn, bufferUsageIn)
{
    POMDOG_ASSERT(nativeIndexBuffer);
}

IndexBuffer::~IndexBuffer() = default;

std::size_t IndexBuffer::GetIndexCount() const noexcept
{
    return indexCount;
}

IndexElementSize IndexBuffer::GetElementSize() const noexcept
{
    return elementSize;
}

std::size_t IndexBuffer::GetSizeInBytes() const noexcept
{
    POMDOG_ASSERT(indexCount > 0);
    return indexCount * Detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);
}

BufferUsage IndexBuffer::GetBufferUsage() const noexcept
{
    return bufferUsage;
}

void IndexBuffer::SetData(const void* source, std::size_t elementCountIn)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCountIn > 0);
    POMDOG_ASSERT(elementCountIn <= indexCount);
    POMDOG_ASSERT(nativeIndexBuffer);
    POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
    nativeIndexBuffer->SetData(0, source,
        Detail::BufferHelper::ToIndexElementOffsetBytes(elementSize) * elementCountIn);
}

void IndexBuffer::SetData(
    std::size_t offsetInBytes,
    const void* source,
    std::size_t elementCountIn)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCountIn > 0);
    POMDOG_ASSERT(elementCountIn <= indexCount);
    POMDOG_ASSERT(nativeIndexBuffer);
    POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
    nativeIndexBuffer->SetData(offsetInBytes, source,
        Detail::BufferHelper::ToIndexElementOffsetBytes(elementSize) * elementCountIn);
}

Detail::NativeBuffer* IndexBuffer::GetNativeIndexBuffer()
{
    POMDOG_ASSERT(nativeIndexBuffer);
    return nativeIndexBuffer.get();
}

} // namespace Pomdog
