// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/metal/buffer_metal.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/graphics/buffer_usage.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"
#import <Metal/Metal.h>
#include <cstring>

namespace pomdog::detail::metal {
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
        Log::Warning("pomdog",
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

} // namespace

std::unique_ptr<Error>
BufferMetal::Initialize(
    id<MTLDevice> device,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    BufferBindMode bindMode) noexcept
{
    const auto alignedSize = ComputeAlignedSize(sizeInBytes, bindMode);
    nativeBuffer = [device newBufferWithLength:alignedSize options:ToResourceOptions(bufferUsage)];
    if (nativeBuffer == nullptr) {
        return errors::New("failed to create MTLBuffer");
    }

    nativeBuffer.label = @"Pomdog.BufferMetal";
    return nullptr;
}

std::unique_ptr<Error>
BufferMetal::Initialize(
    id<MTLDevice> device,
    const void* vertices,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    BufferBindMode bindMode) noexcept
{
    const auto alignedSize = ComputeAlignedSize(sizeInBytes, bindMode);
    nativeBuffer = [device newBufferWithLength:alignedSize options:ToResourceOptions(bufferUsage)];
    if (nativeBuffer == nullptr) {
        return errors::New("failed to create MTLBuffer");
    }

    nativeBuffer.label = @"Pomdog.BufferMetal";

    SetData(0, vertices, sizeInBytes);
    return nullptr;
}

void BufferMetal::GetData(
    std::size_t offsetInBytes,
    void* destination,
    std::size_t sizeInBytes) const
{
    POMDOG_ASSERT(nativeBuffer != nullptr);
    auto source = reinterpret_cast<const uint8_t*>([nativeBuffer contents]);
    std::memcpy(destination, source + offsetInBytes, sizeInBytes);
}

void BufferMetal::SetData(
    std::size_t offsetInBytes,
    const void* source,
    std::size_t sizeInBytes)
{
    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT([nativeBuffer length] >= (sizeInBytes + offsetInBytes));
    auto destination = reinterpret_cast<std::uint8_t*>([nativeBuffer contents]);
    std::memcpy(destination + offsetInBytes, source, sizeInBytes);
}

id<MTLBuffer> BufferMetal::GetBuffer() const noexcept
{
    return nativeBuffer;
}

} // namespace pomdog::detail::metal
