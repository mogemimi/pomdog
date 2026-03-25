// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// Specifies the CPU/GPU access pattern for a buffer resource.
///
/// When a buffer uses CpuToGpu memory, backends may internally maintain
/// multiple copies of the buffer (double or triple buffering) to allow safe
/// concurrent CPU/GPU access across frames. This is transparent to the user:
/// Buffer::map() always operates on the current frame's buffer copy, and
/// writes do not interfere with GPU reads of previous frames' data.
///
/// On backends that do not use explicit multi-buffering (e.g., D3D11), the
/// driver performs equivalent buffer renaming internally, providing the same
/// safety guarantee.
enum class MemoryUsage : u8 {
    /// GPU-only memory. Initialized once, not updated by CPU afterward.
    /// The buffer is stored as a single copy.
    GpuOnly,

    /// CPU writable, GPU readable. Suitable for per-frame updates.
    /// Backends may internally multi-buffer (e.g., Metal allocates N copies
    /// indexed by a frame counter). Buffer::map() always returns a pointer
    /// to the current frame's copy.
    CpuToGpu,

    /// GPU writable, CPU readable. Used for readback operations.
    /// The buffer is stored as a single copy.
    GpuToCpu,
};

} // namespace pomdog::gpu
