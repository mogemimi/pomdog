// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/buffer_metal.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/buffer_bind_flags.h"
#include "pomdog/gpu/buffer_desc.h"
#include "pomdog/gpu/memory_usage.h"
#include "pomdog/gpu/metal/frame_counter.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/Metal.h>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::metal {
namespace {

[[nodiscard]] MTLResourceOptions
toResourceOptions(MemoryUsage memoryUsage) noexcept
{
    switch (memoryUsage) {
    case MemoryUsage::GpuOnly:
        return MTLResourceStorageModeShared;
    case MemoryUsage::CpuToGpu:
        return MTLResourceCPUCacheModeWriteCombined | MTLResourceStorageModeShared;
    case MemoryUsage::GpuToCpu:
        return MTLResourceStorageModeShared;
    }
    POMDOG_UNREACHABLE("Unsupported memory usage");
}

[[nodiscard]] u32
computeAlignedSize(u32 sizeInBytes, BufferBindFlags bindFlags)
{
    if (!hasFlag(bindFlags, BufferBindFlags::ConstantBuffer)) {
        return sizeInBytes;
    }

#if (defined(MAC_OS_X_VERSION_10_11) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_11))
    constexpr u32 alignmentSize = 256;
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

void BufferMetal::getData(
    u32 offsetInBytes,
    std::span<u8> destination) const
{
    POMDOG_ASSERT(frameCounter_ != nullptr);
    const auto bufferIndex = frameCounter_->getCurrentIndex() % static_cast<u32>(buffers_.size());
    auto& nativeBuffer = buffers_[bufferIndex];

    POMDOG_ASSERT(nativeBuffer != nullptr);
    auto source = reinterpret_cast<const u8*>([nativeBuffer contents]);
    std::memcpy(destination.data(), source + offsetInBytes, destination.size());
}

void BufferMetal::setData(
    u32 offsetInBytes,
    std::span<const u8> source)
{
    POMDOG_ASSERT(frameCounter_ != nullptr);
    const auto bufferIndex = frameCounter_->getCurrentIndex() % static_cast<u32>(buffers_.size());
    auto& nativeBuffer = buffers_[bufferIndex];

    POMDOG_ASSERT(nativeBuffer != nullptr);
    POMDOG_ASSERT([nativeBuffer length] >= (source.size() + offsetInBytes));
    auto destination = reinterpret_cast<u8*>([nativeBuffer contents]);
    std::memcpy(destination + offsetInBytes, source.data(), source.size());
}

id<MTLBuffer>
BufferMetal::getBuffer() const noexcept
{
    POMDOG_ASSERT(frameCounter_ != nullptr);
    const auto bufferIndex = frameCounter_->getCurrentIndex() % static_cast<u32>(buffers_.size());
    auto& nativeBuffer = buffers_[bufferIndex];

    return nativeBuffer;
}

std::unique_ptr<Error>
BufferMetal::initialize(
    std::shared_ptr<const FrameCounter> frameCounter,
    id<MTLDevice> device,
    const BufferDesc& desc,
    std::span<const u8> initialData) noexcept
{
    frameCounter_ = std::move(frameCounter);
    if (frameCounter_ == nullptr) {
        return errors::make("frameCounter_ must be != nullptr");
    }

    if (!initialData.empty()) {
        if (initialData.size() < desc.sizeInBytes) {
            return errors::make("initialData size must be >= desc.sizeInBytes");
        }
    }

    if (desc.memoryUsage == MemoryUsage::CpuToGpu) {
        buffers_.resize(frameCounter_->getMaxCount());
    }
    else {
        buffers_.resize(1);
    }
    POMDOG_ASSERT(buffers_.size() == buffers_.capacity());

    for (auto& nativeBuffer : buffers_) {
        const auto alignedSize = computeAlignedSize(desc.sizeInBytes, desc.bindFlags);
        nativeBuffer = [device newBufferWithLength:alignedSize options:toResourceOptions(desc.memoryUsage)];
        if (nativeBuffer == nullptr) {
            return errors::make("failed to create MTLBuffer");
        }

        nativeBuffer.label = @"Pomdog.BufferMetal";

        if (!initialData.empty()) {
            auto destination = reinterpret_cast<u8*>([nativeBuffer contents]);
            std::memcpy(destination, initialData.data(), desc.sizeInBytes);
        }
    }

    return nullptr;
}

std::span<u8> BufferMetal::map(u32 offsetInBytes, u32 sizeInBytes)
{
    POMDOG_ASSERT(frameCounter_ != nullptr);
    const auto bufferIndex = frameCounter_->getCurrentIndex() % static_cast<u32>(buffers_.size());
    auto& nativeBuffer = buffers_[bufferIndex];

    POMDOG_ASSERT(nativeBuffer != nullptr);
    auto* ptr = reinterpret_cast<u8*>([nativeBuffer contents]) + offsetInBytes;
    return std::span<u8>{ptr, sizeInBytes};
}

void BufferMetal::unmap()
{
    // No-op for shared storage mode on Metal.
}

} // namespace pomdog::gpu::detail::metal
