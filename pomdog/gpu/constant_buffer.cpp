// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/buffer_bind_mode.h"
#include "pomdog/gpu/backends/native_buffer.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

ConstantBuffer::ConstantBuffer(
    std::unique_ptr<detail::NativeBuffer>&& nativeBufferIn,
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

void ConstantBuffer::SetData(const void* source, std::size_t sizeInBytesIn)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(sizeInBytesIn > 0);
    POMDOG_ASSERT(sizeInBytesIn <= sizeInBytes);
    POMDOG_ASSERT(nativeBuffer != nullptr);
    return nativeBuffer->SetData(0, source, sizeInBytesIn);
}

void ConstantBuffer::SetData(
    std::size_t offsetInBytes,
    const void* source,
    std::size_t sizeInBytesIn)
{
    POMDOG_ASSERT(source != nullptr);
    POMDOG_ASSERT(sizeInBytesIn > 0);
    POMDOG_ASSERT(offsetInBytes + sizeInBytesIn <= sizeInBytes);
    POMDOG_ASSERT(nativeBuffer != nullptr);
    return nativeBuffer->SetData(offsetInBytes, source, sizeInBytesIn);
}

std::size_t ConstantBuffer::GetSizeInBytes() const noexcept
{
    return sizeInBytes;
}

BufferUsage ConstantBuffer::GetBufferUsage() const noexcept
{
    return bufferUsage;
}

detail::NativeBuffer* ConstantBuffer::GetNativeBuffer()
{
    POMDOG_ASSERT(nativeBuffer != nullptr);
    return nativeBuffer.get();
}

} // namespace pomdog::gpu
