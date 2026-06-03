// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/rect2d.h"

namespace pomdog {

/// DisplayMetrics is a per-frame snapshot of the window's client area,
/// back-buffer size, and effective pixel ratio.
///
/// `GameHost` commits all fields once per frame, immediately before
/// `Game::update()`. The committed values remain constant through
/// `Game::update()` and `Game::draw()`. Updates caused by user resizing,
/// monitor moves, DPI changes, or window-mode transitions are accumulated by
/// the platform event queue during a frame and applied atomically before the
/// next `Game::update()`. Game code never observes a partially-updated state.
///
/// Use `GameWindow::displayMetricsChanged` to observe transitions; the argument
/// is the newly committed snapshot.
struct POMDOG_EXPORT DisplayMetrics final {
    /// The client area in logical pixels.
    ///
    /// Used for UI layout, mouse/touch hit testing, and game-level coordinates
    /// that should not change when the device pixel ratio changes.
    Rect2D clientBounds = {};

    /// The back-buffer width in physical pixels.
    ///
    /// Equals `gpu::PresentationParameters::backBufferWidth`. Use this value,
    /// rather than recomputing from `clientBounds` and `pixelRatio`, when
    /// sizing GPU resources.
    i32 backBufferWidth = 0;

    /// The back-buffer height in physical pixels.
    ///
    /// Equals `gpu::PresentationParameters::backBufferHeight`. Use this value,
    /// rather than recomputing from `clientBounds` and `pixelRatio`, when
    /// sizing GPU resources.
    i32 backBufferHeight = 0;

    /// The effective pixel ratio used to convert logical pixels to physical
    /// pixels.
    ///
    /// On Win32, this is derived from the window DPI. On macOS, this is derived
    /// from the backing scale factor. On Emscripten, this is derived from the
    /// browser device pixel ratio and clamped by
    /// `HighDPISettings::maxPixelRatio`. Equals 1.0 when
    /// `HighDPIMode::Disabled` is selected.
    f32 pixelRatio = 1.0f;

    [[nodiscard]] bool operator==(const DisplayMetrics&) const noexcept = default;
    [[nodiscard]] bool operator!=(const DisplayMetrics&) const noexcept = default;
};

} // namespace pomdog
