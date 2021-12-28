// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/vertex_buffer.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/graphics/backends/buffer_bind_mode.h"
#include "pomdog/graphics/backends/native_buffer.h"
#include "pomdog/graphics/buffer_usage.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

VertexBuffer::VertexBuffer(
    std::unique_ptr<detail::NativeBuffer>&& nativeBufferIn,
    std::size_t vertexCountIn,
    std::size_t strideInBytesIn,
    BufferUsage bufferUsageIn)
    : nativeBuffer(std::move(nativeBufferIn))
    , vertexCount(static_cast<decltype(vertexCount)>(vertexCountIn))
    , strideInBytes(static_cast<decltype(strideInBytes)>(strideInBytesIn))
    , bufferUsage(bufferUsageIn)
{
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideInBytes > 0);
    POMDOG_ASSERT(nativeBuffer != nullptr);
}

VertexBuffer::~VertexBuffer() = default;

std::size_t VertexBuffer::GetStrideBytes() const noexcept
{
    return strideInBytes;
}

std::size_t VertexBuffer::GetVertexCount() const noexcept
{
    return vertexCount;
}

std::size_t VertexBuffer::GetSizeInBytes() const noexcept
{
    POMDOG_ASSERT(strideInBytes > 0);
    POMDOG_ASSERT(vertexCount > 0);
    return vertexCount * strideInBytes;
}

BufferUsage VertexBuffer::GetBufferUsage() const noexcept
{
    return bufferUsage;
}

void VertexBuffer::SetData(const void* source, std::size_t elementCount)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCount > 0);
    POMDOG_ASSERT(elementCount <= vertexCount);
    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT(bufferUsage != pomdog::BufferUsage::Immutable);
    nativeBuffer->SetData(0, source, elementCount * strideInBytes);
}

void VertexBuffer::SetData(
    std::size_t offsetInBytes,
    const void* source,
    std::size_t elementCount,
    std::size_t strideInBytesIn)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCount > 0);
    POMDOG_ASSERT(elementCount <= vertexCount);
    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT(bufferUsage != pomdog::BufferUsage::Immutable);
    nativeBuffer->SetData(offsetInBytes, source,
        elementCount * strideInBytesIn);
}

detail::NativeBuffer* VertexBuffer::GetNativeBuffer()
{
    POMDOG_ASSERT(nativeBuffer != nullptr);
    return nativeBuffer.get();
}

} // namespace pomdog
