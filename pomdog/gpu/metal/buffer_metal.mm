// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/buffer_metal.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/metal/frame_counter.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"
#import <Metal/Metal.h>
#include <cstring>

namespace pomdog::gpu::detail::metal {
namespace {

[[nodiscard]] MTLResourceOptions
toResourceOptions(BufferUsage bufferUsage) noexcept
{
    switch (bufferUsage) {
    case BufferUsage::Immutable:
        return MTLResourceStorageModeShared;
    case BufferUsage::Dynamic:
        return MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeShared;
    }
    POMDOG_UNREACHABLE("Unsupported buffer usage");
}

[[nodiscard]] std::size_t
computeAlignedSize(std::size_t sizeInBytes, BufferBindMode bindMode)
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
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
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
BufferMetal::initialize(
    std::shared_ptr<const FrameCounter> frameCounter,
    id<MTLDevice> device,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    BufferBindMode bindMode) noexcept
{
    frameCounter_ = std::move(frameCounter);
    if (frameCounter_ == nullptr) {
        return errors::make("frameCounter_ must be != nullptr");
    }

    if (bufferUsage == BufferUsage::Dynamic) {
        buffers_.resize(frameCounter_->getMaxCount());
    }
    else {
        buffers_.resize(1);
    }
    POMDOG_ASSERT(buffers_.size() == buffers_.capacity());

    for (auto& nativeBuffer : buffers_) {
        const auto alignedSize = computeAlignedSize(sizeInBytes, bindMode);
        nativeBuffer = [device newBufferWithLength:alignedSize options:toResourceOptions(bufferUsage)];
        if (nativeBuffer == nullptr) {
            return errors::make("failed to create MTLBuffer");
        }

        nativeBuffer.label = @"Pomdog.BufferMetal";
    }

    return nullptr;
}

std::unique_ptr<Error>
BufferMetal::initialize(
    std::shared_ptr<const FrameCounter> frameCounter,
    id<MTLDevice> device,
    const void* vertices,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    BufferBindMode bindMode) noexcept
{
    frameCounter_ = std::move(frameCounter);
    if (frameCounter_ == nullptr) {
        return errors::make("frameCounter_ must be != nullptr");
    }

    if (bufferUsage == BufferUsage::Dynamic) {
        buffers_.resize(frameCounter_->getMaxCount());
    }
    else {
        buffers_.resize(1);
    }
    POMDOG_ASSERT(buffers_.size() == buffers_.capacity());

    for (auto& nativeBuffer : buffers_) {
        const auto alignedSize = computeAlignedSize(sizeInBytes, bindMode);
        nativeBuffer = [device newBufferWithLength:alignedSize options:toResourceOptions(bufferUsage)];
        if (nativeBuffer == nullptr) {
            return errors::make("failed to create MTLBuffer");
        }

        nativeBuffer.label = @"Pomdog.BufferMetal";

        // NOTE: Copy data to buffer.
        auto destination = reinterpret_cast<std::uint8_t*>([nativeBuffer contents]);
        std::memcpy(destination, vertices, sizeInBytes);
    }

    return nullptr;
}

void BufferMetal::getData(
    std::size_t offsetInBytes,
    void* destination,
    std::size_t sizeInBytes) const
{
    POMDOG_ASSERT(frameCounter_ != nullptr);
    const auto bufferIndex = frameCounter_->getCurrentIndex() % static_cast<std::uint32_t>(buffers_.size());
    auto& nativeBuffer = buffers_[bufferIndex];

    POMDOG_ASSERT(nativeBuffer != nullptr);
    auto source = reinterpret_cast<const uint8_t*>([nativeBuffer contents]);
    std::memcpy(destination, source + offsetInBytes, sizeInBytes);
}

void BufferMetal::setData(
    std::size_t offsetInBytes,
    const void* source,
    std::size_t sizeInBytes)
{
    POMDOG_ASSERT(frameCounter_ != nullptr);
    const auto bufferIndex = frameCounter_->getCurrentIndex() % static_cast<std::uint32_t>(buffers_.size());
    auto& nativeBuffer = buffers_[bufferIndex];

    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT([nativeBuffer length] >= (sizeInBytes + offsetInBytes));
    auto destination = reinterpret_cast<std::uint8_t*>([nativeBuffer contents]);
    std::memcpy(destination + offsetInBytes, source, sizeInBytes);
}

id<MTLBuffer> BufferMetal::getBuffer() const noexcept
{
    POMDOG_ASSERT(frameCounter_ != nullptr);
    const auto bufferIndex = frameCounter_->getCurrentIndex() % static_cast<std::uint32_t>(buffers_.size());
    auto& nativeBuffer = buffers_[bufferIndex];

    return nativeBuffer;
}

} // namespace pomdog::gpu::detail::metal
