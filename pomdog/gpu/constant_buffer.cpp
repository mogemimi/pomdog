// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/backends/buffer_bind_mode.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

ConstantBuffer::ConstantBuffer(
    std::shared_ptr<Buffer> nativeBufferIn,
    u32 sizeInBytesIn,
    BufferUsage bufferUsageIn)
    : nativeBuffer_(std::move(nativeBufferIn))
    , sizeInBytes_(sizeInBytesIn)
    , bufferUsage_(bufferUsageIn)
{
    POMDOG_ASSERT(sizeInBytes_ > 0);
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
}

ConstantBuffer::~ConstantBuffer() = default;

void ConstantBuffer::setData(u32 offsetInBytes, std::span<const std::byte> source)
{
    POMDOG_ASSERT(!source.empty());
    POMDOG_ASSERT(source.data() != nullptr);
    POMDOG_ASSERT(source.size() > 0);
    POMDOG_ASSERT(offsetInBytes + source.size() <= sizeInBytes_);
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    return nativeBuffer_->setData(offsetInBytes, source.data(), static_cast<u32>(source.size()));
}

u32 ConstantBuffer::getSizeInBytes() const noexcept
{
    return sizeInBytes_;
}

BufferUsage
ConstantBuffer::getBufferUsage() const noexcept
{
    return bufferUsage_;
}

unsafe_ptr<Buffer>
ConstantBuffer::getBuffer()
{
    POMDOG_ASSERT(nativeBuffer_ != nullptr);
    return nativeBuffer_.get();
}

} // namespace pomdog::gpu
