// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace {

static std::uint32_t ToIndexElementOffsetBytes(
    IndexElementSize elementSize) noexcept
{
    switch (elementSize) {
    case IndexElementSize::ThirtyTwoBits: return 4;
    case IndexElementSize::SixteenBits: return 2;
    };
#ifdef _MSC_VER
    // FUS RO DAH!
    return 4;
#endif
}

} // unnamed namespace
//-----------------------------------------------------------------------
IndexBuffer::IndexBuffer(GraphicsDevice & graphicsDevice,
    IndexElementSize elementSizeIn,
    void const* indices,
    std::size_t indexCountIn,
    Pomdog::BufferUsage bufferUsageIn)
    : indexCount(static_cast<decltype(indexCount)>(indexCountIn))
    , elementSize(elementSizeIn)
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(indexCount > 0);

    auto sizeInBytes = indexCount * ToIndexElementOffsetBytes(elementSize);
    auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice != nullptr);
    using Detail::RenderSystem::BufferBindMode;

    nativeIndexBuffer = nativeDevice->CreateBuffer(
        indices, sizeInBytes, bufferUsage, BufferBindMode::IndexBuffer);

    POMDOG_ASSERT(nativeIndexBuffer);
}
//-----------------------------------------------------------------------
IndexBuffer::IndexBuffer(GraphicsDevice & graphicsDevice,
    IndexElementSize elementSizeIn,
    std::size_t indexCountIn,
    Pomdog::BufferUsage bufferUsageIn)
    : indexCount(static_cast<decltype(indexCount)>(indexCountIn))
    , elementSize(elementSizeIn)
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(indexCount > 0);

    auto sizeInBytes = indexCount * ToIndexElementOffsetBytes(elementSize);
    auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice != nullptr);
    using Detail::RenderSystem::BufferBindMode;

    nativeIndexBuffer = nativeDevice->CreateBuffer(
        sizeInBytes, bufferUsage, BufferBindMode::IndexBuffer);

    POMDOG_ASSERT(nativeIndexBuffer);
}
//-----------------------------------------------------------------------
IndexBuffer::IndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    IndexElementSize elementSizeIn, void const* indices, std::size_t indexCountIn,
    Pomdog::BufferUsage bufferUsageIn)
    : IndexBuffer(*graphicsDevice, elementSizeIn, indices, indexCountIn, bufferUsageIn)
{
    POMDOG_ASSERT(nativeIndexBuffer);
}
//-----------------------------------------------------------------------
IndexBuffer::IndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    IndexElementSize elementSizeIn, std::size_t indexCountIn,
    Pomdog::BufferUsage bufferUsageIn)
    : IndexBuffer(*graphicsDevice, elementSizeIn, indexCountIn, bufferUsageIn)
{
    POMDOG_ASSERT(nativeIndexBuffer);
}
//-----------------------------------------------------------------------
IndexBuffer::~IndexBuffer() = default;
//-----------------------------------------------------------------------
std::size_t IndexBuffer::IndexCount() const noexcept
{
    return indexCount;
}
//-----------------------------------------------------------------------
IndexElementSize IndexBuffer::ElementSize() const noexcept
{
    return elementSize;
}
//-----------------------------------------------------------------------
std::size_t IndexBuffer::SizeInBytes() const noexcept
{
    POMDOG_ASSERT(indexCount > 0);
    return indexCount * ToIndexElementOffsetBytes(elementSize);
}
//-----------------------------------------------------------------------
BufferUsage IndexBuffer::BufferUsage() const noexcept
{
    return bufferUsage;
}
//-----------------------------------------------------------------------
void IndexBuffer::SetData(void const* source, std::size_t elementCountIn)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCountIn > 0);
    POMDOG_ASSERT(elementCountIn <= indexCount);
    POMDOG_ASSERT(nativeIndexBuffer);
    POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
    nativeIndexBuffer->SetData(0, source,
        ToIndexElementOffsetBytes(elementSize) * elementCountIn);
}
//-----------------------------------------------------------------------
void IndexBuffer::SetData(std::size_t offsetInBytes,
    void const* source, std::size_t elementCountIn)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCountIn > 0);
    POMDOG_ASSERT(elementCountIn <= indexCount);
    POMDOG_ASSERT(nativeIndexBuffer);
    POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
    nativeIndexBuffer->SetData(offsetInBytes, source,
        ToIndexElementOffsetBytes(elementSize) * elementCountIn);
}
//-----------------------------------------------------------------------
Detail::RenderSystem::NativeBuffer* IndexBuffer::NativeIndexBuffer()
{
    POMDOG_ASSERT(nativeIndexBuffer);
    return nativeIndexBuffer.get();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
