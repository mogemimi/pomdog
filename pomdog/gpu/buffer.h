// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

class Buffer {
public:
    Buffer() noexcept;
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    virtual ~Buffer();

    virtual void getData(
        u32 offsetInBytes,
        std::span<u8> destination) const = 0;

    virtual void setData(
        u32 offsetInBytes,
        std::span<const u8> source) = 0;

    /// Maps a region of the buffer for CPU access.
    ///
    /// Returns a span covering the mapped memory region. Returns an empty
    /// span if the buffer cannot be mapped (e.g., GpuOnly memory).
    ///
    /// For CpuToGpu buffers that are internally multi-buffered (e.g., Metal),
    /// the returned memory corresponds to the current frame's internal buffer
    /// copy. Data written here will be visible to GPU draw calls submitted in
    /// the same frame. Previous frames' GPU reads are not affected.
    ///
    /// The caller must call unmap() when finished accessing the memory.
    /// The returned span is invalidated after unmap() is called.
    [[nodiscard]] virtual std::span<u8>
    map(u32 offsetInBytes, u32 sizeInBytes) = 0;

    /// Unmaps a previously mapped buffer region.
    ///
    /// After this call, any span returned by the prior map() call is
    /// invalidated and must not be accessed.
    virtual void
    unmap() = 0;
};

} // namespace pomdog::gpu
