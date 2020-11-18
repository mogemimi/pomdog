// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "../RenderSystem/BufferBindMode.hpp"
#include "../RenderSystem/NativeBuffer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {

ConstantBuffer::ConstantBuffer(
    std::unique_ptr<Detail::NativeBuffer>&& nativeBufferIn,
    std::size_t sizeInBytesIn,
    BufferUsage bufferUsageIn)
    : nativeBuffer(std::move(nativeBufferIn))
    , sizeInBytes(static_cast<decltype(sizeInBytes)>(sizeInBytesIn))
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(sizeInBytes > 0);
    POMDOG_ASSERT(nativeBuffer != nullptr);
}

ConstantBuffer::~ConstantBuffer() = default;

void ConstantBuffer::GetValue(std::size_t sizeInBytesIn, void* result) const
{
    POMDOG_ASSERT(sizeInBytesIn > 0);
    POMDOG_ASSERT(sizeInBytesIn <= sizeInBytes);
    POMDOG_ASSERT(result != nullptr);
    POMDOG_ASSERT(nativeBuffer != nullptr);
    nativeBuffer->GetData(0, result, sizeInBytesIn);
}

void ConstantBuffer::SetValue(const void* data, std::size_t sizeInBytesIn)
{
    POMDOG_ASSERT(data != nullptr);
    POMDOG_ASSERT(sizeInBytesIn > 0);
    POMDOG_ASSERT(sizeInBytesIn <= sizeInBytes);
    POMDOG_ASSERT(nativeBuffer != nullptr);
    return nativeBuffer->SetData(0, data, sizeInBytesIn);
}

std::size_t ConstantBuffer::GetSizeInBytes() const noexcept
{
    return sizeInBytes;
}

BufferUsage ConstantBuffer::GetBufferUsage() const noexcept
{
    return bufferUsage;
}

Detail::NativeBuffer* ConstantBuffer::GetNativeBuffer()
{
    POMDOG_ASSERT(nativeBuffer != nullptr);
    return nativeBuffer.get();
}

} // namespace Pomdog
