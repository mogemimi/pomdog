// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "../Graphics.Backends/BufferBindMode.hpp"
#include "../Graphics.Backends/BufferHelper.hpp"
#include "../Graphics.Backends/NativeBuffer.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

IndexBuffer::IndexBuffer(
    std::unique_ptr<Detail::NativeBuffer>&& nativeBufferIn,
    IndexElementSize elementSizeIn,
    std::size_t indexCountIn,
    BufferUsage bufferUsageIn)
    : nativeBuffer(std::move(nativeBufferIn))
    , indexCount(static_cast<decltype(indexCount)>(indexCountIn))
    , elementSize(elementSizeIn)
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(indexCount > 0);
    POMDOG_ASSERT(nativeBuffer != nullptr);
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
    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
    nativeBuffer->SetData(0, source,
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
    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
    nativeBuffer->SetData(offsetInBytes, source,
        Detail::BufferHelper::ToIndexElementOffsetBytes(elementSize) * elementCountIn);
}

Detail::NativeBuffer* IndexBuffer::GetNativeBuffer()
{
    POMDOG_ASSERT(nativeBuffer != nullptr);
    return nativeBuffer.get();
}

} // namespace Pomdog
