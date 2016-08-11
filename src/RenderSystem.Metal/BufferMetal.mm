// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "BufferMetal.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <cstring>
#import <Metal/Metal.h>

namespace Pomdog {
namespace Detail {
namespace Metal {
namespace {

MTLResourceOptions ToResourceOptions(BufferUsage bufferUsage)
{
#if (defined(MAC_OS_X_VERSION_10_11) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_11)) \
    || (defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && (__IPHONE_OS_VERSION_MIN_REQUIRED >= 90000))
    switch (bufferUsage) {
    case BufferUsage::Immutable:
        return MTLResourceStorageModeShared;
        break;
    case BufferUsage::Dynamic:
        return MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeShared;
        break;
    }
#else
    return MTLResourceCPUCacheModeDefaultCache;
#endif
}

} // unnamed namespace

BufferMetal::BufferMetal(
    id<MTLDevice> device,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage)
    : nativeBuffer(nil)
{
    nativeBuffer = [device newBufferWithLength:sizeInBytes
        options:ToResourceOptions(bufferUsage)];

   if (nativeBuffer == nil) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create MTLBuffer");
    }

    nativeBuffer.label = @"Pomdog.BufferMetal";
}

BufferMetal::BufferMetal(
    id<MTLDevice> device,
    const void* vertices,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage)
    : nativeBuffer(nil)
{
    nativeBuffer = [device newBufferWithLength:sizeInBytes
        options:ToResourceOptions(bufferUsage)];

   if (nativeBuffer == nil) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create MTLBuffer");
    }

    nativeBuffer.label = @"Pomdog.BufferMetal";

    SetData(0, vertices, sizeInBytes);
}

void BufferMetal::GetData(
    std::size_t offsetInBytes,
    void* destination,
    std::size_t sizeInBytes) const
{
    POMDOG_ASSERT(nativeBuffer != nil);
    auto source = [nativeBuffer contents];
    std::memcpy(destination, source, sizeInBytes);
}

void BufferMetal::SetData(
    std::size_t offsetInBytes,
    const void* source,
    std::size_t sizeInBytes)
{
    POMDOG_ASSERT(nativeBuffer != nil);
    POMDOG_ASSERT([nativeBuffer length] <= sizeInBytes);
    auto destination = reinterpret_cast<std::uint8_t*>([nativeBuffer contents]);
    std::memcpy(destination + offsetInBytes, source, sizeInBytes);
}

id<MTLBuffer> BufferMetal::GetBuffer() const
{
    return nativeBuffer;
}

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
