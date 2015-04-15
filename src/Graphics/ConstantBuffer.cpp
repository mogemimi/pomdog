// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
ConstantBuffer::ConstantBuffer(
    GraphicsDevice & graphicsDevice,
    std::size_t sizeInBytesIn,
    Pomdog::BufferUsage bufferUsageIn)
    : sizeInBytes(static_cast<decltype(sizeInBytes)>(sizeInBytesIn))
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(sizeInBytes > 0);

    auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice != nullptr);
    using Detail::RenderSystem::BufferBindMode;

    nativeConstantBuffer = nativeDevice->CreateBuffer(
        sizeInBytes, bufferUsage, BufferBindMode::ConstantBuffer);

    POMDOG_ASSERT(nativeConstantBuffer);
}
//-----------------------------------------------------------------------
ConstantBuffer::ConstantBuffer(
    GraphicsDevice & graphicsDevice,
    void const* sourceData,
    std::size_t sizeInBytesIn,
    Pomdog::BufferUsage bufferUsageIn)
    : sizeInBytes(static_cast<decltype(sizeInBytes)>(sizeInBytesIn))
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(sizeInBytes > 0);

    auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice != nullptr);
    using Detail::RenderSystem::BufferBindMode;

    nativeConstantBuffer = nativeDevice->CreateBuffer(
        sourceData, sizeInBytes, bufferUsage, BufferBindMode::ConstantBuffer);

    POMDOG_ASSERT(nativeConstantBuffer);
}
//-----------------------------------------------------------------------
ConstantBuffer::ConstantBuffer(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    std::size_t sizeInBytesIn,
    Pomdog::BufferUsage bufferUsageIn)
    : ConstantBuffer(*graphicsDevice, sizeInBytesIn, bufferUsageIn)
{}
//-----------------------------------------------------------------------
ConstantBuffer::ConstantBuffer(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    void const* sourceData,
    std::size_t sizeInBytesIn,
    Pomdog::BufferUsage bufferUsageIn)
    : ConstantBuffer(*graphicsDevice, sourceData, sizeInBytesIn, bufferUsageIn)
{}
//-----------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer() = default;
//-----------------------------------------------------------------------
void ConstantBuffer::GetValue(std::size_t sizeInBytesIn, void* result) const
{
    POMDOG_ASSERT(sizeInBytesIn > 0);
    POMDOG_ASSERT(sizeInBytesIn <= sizeInBytes);
    POMDOG_ASSERT(result != nullptr);
    POMDOG_ASSERT(nativeConstantBuffer);
    nativeConstantBuffer->GetData(0, result, sizeInBytesIn);
}
//-----------------------------------------------------------------------
void ConstantBuffer::SetValue(void const* data, std::size_t sizeInBytesIn)
{
    POMDOG_ASSERT(data != nullptr);
    POMDOG_ASSERT(sizeInBytesIn > 0);
    POMDOG_ASSERT(sizeInBytesIn <= sizeInBytes);
    POMDOG_ASSERT(nativeConstantBuffer);
    return nativeConstantBuffer->SetData(0, data, sizeInBytesIn);
}
//-----------------------------------------------------------------------
std::size_t ConstantBuffer::SizeInBytes() const noexcept
{
    return sizeInBytes;
}
//-----------------------------------------------------------------------
BufferUsage ConstantBuffer::BufferUsage() const noexcept
{
    return bufferUsage;
}
//-----------------------------------------------------------------------
Detail::RenderSystem::NativeBuffer* ConstantBuffer::NativeConstantBuffer()
{
    POMDOG_ASSERT(nativeConstantBuffer);
    return nativeConstantBuffer.get();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
