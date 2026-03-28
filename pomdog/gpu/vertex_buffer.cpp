// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

VertexBuffer::VertexBuffer(
    std::shared_ptr<Buffer> nativeBufferIn,
    u32 vertexCountIn,
    u32 strideInBytesIn,
    BufferUsage bufferUsageIn)
    : nativeBuffer_(std::move(nativeBufferIn))
    , vertexCount_(vertexCountIn)
    , strideInBytes_(strideInBytesIn)
    , bufferUsage_(bufferUsageIn)
{
    POMDOG_ASSERT(vertexCount_ > 0);
    POMDOG_ASSERT(strideInBytes_ > 0);
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
}

VertexBuffer::~VertexBuffer() = default;

u32 VertexBuffer::getStrideBytes() const noexcept
{
    return strideInBytes_;
}

u32 VertexBuffer::getVertexCount() const noexcept
{
    return vertexCount_;
}

u32 VertexBuffer::getSizeInBytes() const noexcept
{
    POMDOG_ASSERT(strideInBytes_ > 0);
    POMDOG_ASSERT(vertexCount_ > 0);
    return vertexCount_ * strideInBytes_;
}

BufferUsage
VertexBuffer::getBufferUsage() const noexcept
{
    return bufferUsage_;
}

void VertexBuffer::setData(const void* source, u32 elementCount)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCount > 0);
    POMDOG_ASSERT(elementCount <= vertexCount_);
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    POMDOG_ASSERT(bufferUsage_ != BufferUsage::Immutable);
    nativeBuffer_->setData(0, std::span<const u8>{reinterpret_cast<const u8*>(source), elementCount * strideInBytes_});
}

void VertexBuffer::setData(
    u32 offsetInBytes,
    const void* source,
    u32 elementCount,
    u32 strideInBytesIn)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCount > 0);
    POMDOG_ASSERT(elementCount <= vertexCount_);
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    POMDOG_ASSERT(bufferUsage_ != BufferUsage::Immutable);
    nativeBuffer_->setData(offsetInBytes, std::span<const u8>{reinterpret_cast<const u8*>(source), elementCount * strideInBytesIn});
}

unsafe_ptr<Buffer>
VertexBuffer::getBuffer()
{
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    return nativeBuffer_.get();
}

} // namespace pomdog::gpu
