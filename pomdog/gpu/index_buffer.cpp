// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/index_buffer.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/buffer_bind_mode.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

IndexBuffer::IndexBuffer(
    std::unique_ptr<Buffer>&& nativeBufferIn,
    IndexFormat elementSizeIn,
    std::size_t indexCountIn,
    BufferUsage bufferUsageIn)
    : nativeBuffer_(std::move(nativeBufferIn))
    , indexCount_(static_cast<decltype(indexCount_)>(indexCountIn))
    , elementSize_(elementSizeIn)
    , bufferUsage_(bufferUsageIn)
{
    POMDOG_ASSERT(indexCount_ > 0);
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
}

IndexBuffer::~IndexBuffer() = default;

std::size_t IndexBuffer::getIndexCount() const noexcept
{
    return indexCount_;
}

IndexFormat IndexBuffer::getElementSize() const noexcept
{
    return elementSize_;
}

std::size_t IndexBuffer::getSizeInBytes() const noexcept
{
    POMDOG_ASSERT(indexCount_ > 0);
    return indexCount_ * detail::BufferHelper::ToIndexElementOffsetBytes(elementSize_);
}

BufferUsage IndexBuffer::getBufferUsage() const noexcept
{
    return bufferUsage_;
}

void IndexBuffer::setData(const void* source, std::size_t elementCountIn)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCountIn > 0);
    POMDOG_ASSERT(elementCountIn <= indexCount_);
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    POMDOG_ASSERT(bufferUsage_ != BufferUsage::Immutable);
    nativeBuffer_->setData(
        0,
        source,
        detail::BufferHelper::ToIndexElementOffsetBytes(elementSize_) * elementCountIn);
}

void IndexBuffer::setData(
    std::size_t offsetInBytes,
    const void* source,
    std::size_t elementCountIn)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(elementCountIn > 0);
    POMDOG_ASSERT(elementCountIn <= indexCount_);
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    POMDOG_ASSERT(bufferUsage_ != BufferUsage::Immutable);
    nativeBuffer_->setData(
        offsetInBytes,
        source,
        detail::BufferHelper::ToIndexElementOffsetBytes(elementSize_) * elementCountIn);
}

Buffer* IndexBuffer::getBuffer()
{
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    return nativeBuffer_.get();
}

} // namespace pomdog::gpu
