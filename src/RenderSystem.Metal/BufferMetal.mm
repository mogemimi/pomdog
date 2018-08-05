// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "BufferMetal.hpp"
#include "../Basic/Unreachable.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#import <Metal/Metal.h>
#include <cstring>

namespace Pomdog {
namespace Detail {
namespace Metal {
namespace {

MTLResourceOptions ToResourceOptions(BufferUsage bufferUsage) noexcept
{
    switch (bufferUsage) {
    case BufferUsage::Immutable:
        return MTLResourceStorageModeShared;
    case BufferUsage::Dynamic:
        return MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeShared;
    }
    POMDOG_UNREACHABLE("Unsupported buffer usage");
}

std::size_t ComputeAlignedSize(std::size_t sizeInBytes, BufferBindMode bindMode)
{
    if (bindMode != BufferBindMode::ConstantBuffer) {
        return sizeInBytes;
    }

#if (defined(MAC_OS_X_VERSION_10_11) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_11))
    // NOTE:
    // For buffers in the constant address space, the offset must be aligned to 256 bytes in macOS.
    // https://developer.apple.com/reference/metal/mtlrendercommandencoder/1515829-setvertexbuffer?language=objc

    constexpr std::size_t alignmentSize = 256;
    if (sizeInBytes % alignmentSize != 0) {
#if defined(DEBUG) && !defined(NDEBUG)
        Log::Warning("Pomdog",
            "You must set the sizeInBytes value in multiples of 256.\n"
            "ConstantBuffer size also needs to be a multiples of 256.");
#endif
        const auto offset = sizeInBytes % alignmentSize;
        POMDOG_ASSERT(offset < alignmentSize);
        sizeInBytes += (alignmentSize - offset);
    }
#endif
    return sizeInBytes;
}

} // unnamed namespace

BufferMetal::BufferMetal(
    id<MTLDevice> device,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    BufferBindMode bindMode)
    : nativeBuffer(nil)
{
    const auto alignedSize = ComputeAlignedSize(sizeInBytes, bindMode);
    nativeBuffer = [device newBufferWithLength:alignedSize
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
    BufferUsage bufferUsage,
    BufferBindMode bindMode)
    : nativeBuffer(nil)
{
    const auto alignedSize = ComputeAlignedSize(sizeInBytes, bindMode);
    nativeBuffer = [device newBufferWithLength:alignedSize
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
    auto source = reinterpret_cast<const uint8_t*>([nativeBuffer contents]);
    std::memcpy(destination, source + offsetInBytes, sizeInBytes);
}

void BufferMetal::SetData(
    std::size_t offsetInBytes,
    const void* source,
    std::size_t sizeInBytes)
{
    POMDOG_ASSERT(nativeBuffer != nil);
    POMDOG_ASSERT([nativeBuffer length] >= (sizeInBytes + offsetInBytes));
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
