// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/buffer_bind_mode.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

VertexBuffer::VertexBuffer(
    std::unique_ptr<Buffer>&& nativeBufferIn,
    std::size_t vertexCountIn,
    std::size_t strideInBytesIn,
    BufferUsage bufferUsageIn)
    : nativeBuffer_(std::move(nativeBufferIn))
    , vertexCount_(static_cast<decltype(vertexCount_)>(vertexCountIn))
    , strideInBytes_(static_cast<decltype(strideInBytes_)>(strideInBytesIn))
    , bufferUsage_(bufferUsageIn)
{
    POMDOG_ASSERT(vertexCount_ > 0);
    POMDOG_ASSERT(strideInBytes_ > 0);
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
}

VertexBuffer::~VertexBuffer() = default;

std::size_t VertexBuffer::getStrideBytes() const noexcept
{
    return strideInBytes_;
}

std::size_t VertexBuffer::getVertexCount() const noexcept
{
    return vertexCount_;
}

std::size_t VertexBuffer::getSizeInBytes() const noexcept
{
    POMDOG_ASSERT(strideInBytes_ > 0);
    POMDOG_ASSERT(vertexCount_ > 0);
    return vertexCount_ * strideInBytes_;
}

BufferUsage VertexBuffer::getBufferUsage() const noexcept
{
    return bufferUsage_;
}

void VertexBuffer::setData(const void* source, std::size_t elementCount)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCount > 0);
    POMDOG_ASSERT(elementCount <= vertexCount_);
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    POMDOG_ASSERT(bufferUsage_ != BufferUsage::Immutable);
    nativeBuffer_->setData(0, source, elementCount * strideInBytes_);
}

void VertexBuffer::setData(
    std::size_t offsetInBytes,
    const void* source,
    std::size_t elementCount,
    std::size_t strideInBytesIn)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCount > 0);
    POMDOG_ASSERT(elementCount <= vertexCount_);
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    POMDOG_ASSERT(bufferUsage_ != BufferUsage::Immutable);
    nativeBuffer_->setData(offsetInBytes, source, elementCount * strideInBytesIn);
}

Buffer* VertexBuffer::getBuffer()
{
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    return nativeBuffer_.get();
}

} // namespace pomdog::gpu
