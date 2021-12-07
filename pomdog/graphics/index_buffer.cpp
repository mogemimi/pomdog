// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/index_buffer.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/backends/buffer_bind_mode.hpp"
#include "pomdog/graphics/backends/buffer_helper.hpp"
#include "pomdog/graphics/backends/native_buffer.hpp"
#include "pomdog/utility/assert.hpp"

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
