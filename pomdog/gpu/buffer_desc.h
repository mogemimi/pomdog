// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/buffer_bind_flags.h"
#include "pomdog/gpu/memory_usage.h"

namespace pomdog::gpu {

/// Describes a buffer resource to be created.
struct BufferDesc final {
    /// The total size of the buffer in bytes.
    u32 sizeInBytes = 0;

    /// The CPU/GPU access pattern.
    MemoryUsage memoryUsage = MemoryUsage::GpuOnly;

    /// How the buffer can be bound to the pipeline.
    BufferBindFlags bindFlags = BufferBindFlags::None;
};

} // namespace pomdog::gpu
