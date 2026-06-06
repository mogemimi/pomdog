// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// PresentMode specifies how rendered frames are presented to the display:
/// whether presentation is synchronized with the display refresh, and how
/// pending frames are handled before they become visible.
///
/// Requested via `GameHost::requestPresentMode()`. Not every mode is supported
/// on every platform / graphics backend; an unsupported mode falls back to the
/// closest supported one, and `GameHost::getPresentMode()` reports the
/// effective mode that was actually applied.
enum class PresentMode : u8 {
    /// Present as soon as a frame is ready, without waiting for the vertical
    /// blank. This may allow tearing and an uncapped frame rate, subject to any
    /// `GameHostOptions::maxFramesPerSecond` cap. Maps to `IMMEDIATE` (Vulkan)
    /// or sync interval 0 (Direct3D 11 / OpenGL).
    Immediate,

    /// Synchronize presentation with the display refresh and present frames in
    /// order. This prevents tearing, with presentation paced by the display
    /// refresh. Supported on every backend. Maps to `FIFO` (Vulkan).
    VSync,

    /// Synchronize presentation with the display refresh while frames are
    /// delivered in time, but may present immediately, allowing tearing, if a
    /// frame misses the refresh interval. This trades occasional tearing for
    /// reduced stutter. Maps to `FIFO_RELAXED` (Vulkan) or the
    /// `*_swap_control_tear` extension (OpenGL). Falls back to `VSync` where
    /// unsupported.
    Adaptive,

    /// Synchronize presentation with the display refresh, preventing tearing,
    /// but keep only the most recent completed frame for the next refresh.
    /// Older pending frames may be discarded.
    ///
    /// This can reduce presentation latency compared to standard V-Sync, but
    /// without a frame-rate cap the application may render faster than the
    /// display refresh rate. This is a present policy rather than a V-Sync
    /// flavor.
    /// Maps to `MAILBOX` (Vulkan). Falls back to `VSync` where unsupported.
    Mailbox,
};

} // namespace pomdog::gpu
