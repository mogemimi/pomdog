// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "../RenderSystem/BufferBindMode.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {

ConstantBuffer::ConstantBuffer(
    GraphicsDevice & graphicsDevice,
    std::size_t sizeInBytesIn,
    BufferUsage bufferUsageIn)
    : sizeInBytes(static_cast<decltype(sizeInBytes)>(sizeInBytesIn))
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(sizeInBytes > 0);

    auto nativeDevice = graphicsDevice.GetNativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice != nullptr);
    using Detail::BufferBindMode;

    nativeConstantBuffer = nativeDevice->CreateBuffer(
        sizeInBytes, bufferUsage, BufferBindMode::ConstantBuffer);

    POMDOG_ASSERT(nativeConstantBuffer);
}

ConstantBuffer::ConstantBuffer(
    GraphicsDevice & graphicsDevice,
    const void* sourceData,
    std::size_t sizeInBytesIn,
    BufferUsage bufferUsageIn)
    : sizeInBytes(static_cast<decltype(sizeInBytes)>(sizeInBytesIn))
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(sizeInBytes > 0);

    auto nativeDevice = graphicsDevice.GetNativeGraphicsDevice();

    POMDOG_ASSERT(nativeDevice != nullptr);
    using Detail::BufferBindMode;

    nativeConstantBuffer = nativeDevice->CreateBuffer(
        sourceData, sizeInBytes, bufferUsage, BufferBindMode::ConstantBuffer);

    POMDOG_ASSERT(nativeConstantBuffer);
}

ConstantBuffer::ConstantBuffer(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    std::size_t sizeInBytesIn,
    BufferUsage bufferUsageIn)
    : ConstantBuffer(*graphicsDevice, sizeInBytesIn, bufferUsageIn)
{
}

ConstantBuffer::ConstantBuffer(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const void* sourceData,
    std::size_t sizeInBytesIn,
    BufferUsage bufferUsageIn)
    : ConstantBuffer(*graphicsDevice, sourceData, sizeInBytesIn, bufferUsageIn)
{
}

ConstantBuffer::~ConstantBuffer() = default;

void ConstantBuffer::GetValue(std::size_t sizeInBytesIn, void* result) const
{
    POMDOG_ASSERT(sizeInBytesIn > 0);
    POMDOG_ASSERT(sizeInBytesIn <= sizeInBytes);
    POMDOG_ASSERT(result != nullptr);
    POMDOG_ASSERT(nativeConstantBuffer);
    nativeConstantBuffer->GetData(0, result, sizeInBytesIn);
}

void ConstantBuffer::SetValue(const void* data, std::size_t sizeInBytesIn)
{
    POMDOG_ASSERT(data != nullptr);
    POMDOG_ASSERT(sizeInBytesIn > 0);
    POMDOG_ASSERT(sizeInBytesIn <= sizeInBytes);
    POMDOG_ASSERT(nativeConstantBuffer);
    return nativeConstantBuffer->SetData(0, data, sizeInBytesIn);
}

std::size_t ConstantBuffer::GetSizeInBytes() const noexcept
{
    return sizeInBytes;
}

BufferUsage ConstantBuffer::GetBufferUsage() const noexcept
{
    return bufferUsage;
}

Detail::NativeBuffer* ConstantBuffer::GetNativeConstantBuffer()
{
    POMDOG_ASSERT(nativeConstantBuffer);
    return nativeConstantBuffer.get();
}

} // namespace Pomdog
