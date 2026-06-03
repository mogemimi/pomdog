// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {

/// HighDPIMode selects how Pomdog maps logical window coordinates to physical
/// drawable pixels on high-DPI / Retina / browser-scaled displays.
enum class HighDPIMode : u8 {
    /// Disabled forces the pixel ratio to 1.0 on every platform and skips
    /// platform DPI-awareness setup (for example,
    /// `SetProcessDpiAwarenessContext` on Win32). The OS may bitmap-stretch the
    /// window.
    Disabled,

    /// Enabled uses the platform-reported pixel ratio.
    ///
    /// On Win32, this enables Per-Monitor V2 DPI awareness. On platforms where
    /// Pomdog controls the backing-store size directly, such as the Emscripten
    /// canvas, the effective pixel ratio may be limited by
    /// `HighDPISettings::maxPixelRatio`.
    Enabled,

    /// PlatformDefault uses Pomdog's recommended behavior for the current
    /// platform. Currently equivalent to `Enabled` on all supported platforms.
    PlatformDefault,
};

/// HighDPISettings configures how the engine handles high-DPI displays.
///
/// Set on `GameHostOptions::highDPI` from `GameSetup::configure()`. The pixel
/// ratio reported by `GameWindow::getPixelRatio()` is the effective ratio used
/// by the current backend. It is the platform-reported ratio on desktop
/// backends, or `min(platformPixelRatio, maxPixelRatio)` on the Emscripten
/// canvas.
struct POMDOG_EXPORT HighDPISettings final {
    /// The high-DPI mode.
    ///
    /// Defaults to `PlatformDefault`, which currently enables high-DPI handling
    /// on all supported platforms.
    HighDPIMode mode = HighDPIMode::PlatformDefault;

    /// The upper bound for the effective pixel ratio.
    ///
    /// Applied only where Pomdog controls the backing-store resolution
    /// directly, such as the Emscripten / WebGL canvas. Desktop backends
    /// currently use the platform-reported window DPI or backing scale as-is,
    /// so this bound is ignored there. Use a separate render-scale or
    /// resolution policy for desktop internal-resolution scaling.
    ///
    /// Defaults to 2.0. Browsers and phones with DPR 3.0+ are clamped to 2.0 to
    /// keep GPU load reasonable. Set to a larger value, such as 3.0, for higher
    /// quality on such devices, or to 1.5 for performance.
    f32 maxPixelRatio = 2.0f;
};

/// Returns the clamped effective pixel ratio derived from `settings` and a raw
/// pixel ratio reported by the platform.
///
/// Used where Pomdog controls the backing-store resolution directly, such as
/// the Emscripten canvas. Desktop backends use
/// `computeUnclampedPixelRatio` instead.
///
/// Returns 1.0 when `settings.mode` is `Disabled`. Otherwise returns
/// `min(rawPixelRatio, settings.maxPixelRatio)`, falling back to 1.0 for a
/// non-positive raw value, so the result is never larger than `maxPixelRatio`.
[[nodiscard]] POMDOG_EXPORT f32
computeEffectivePixelRatio(const HighDPISettings& settings, f32 rawPixelRatio) noexcept;

/// Returns the pixel ratio derived from `settings` and a raw pixel ratio
/// reported by the platform, without applying `maxPixelRatio`.
///
/// Used by desktop backends such as Win32 and macOS, where the OS determines
/// the drawable size from its DPI or backing-scale setting. Applying
/// `maxPixelRatio` there would make `pixelRatio` inconsistent with the actual
/// relationship between the logical client size and the back-buffer size.
///
/// Returns 1.0 when `settings.mode` is `Disabled`, or for a non-positive raw
/// value; otherwise returns `rawPixelRatio` unchanged.
[[nodiscard]] POMDOG_EXPORT f32
computeUnclampedPixelRatio(const HighDPISettings& settings, f32 rawPixelRatio) noexcept;

} // namespace pomdog
