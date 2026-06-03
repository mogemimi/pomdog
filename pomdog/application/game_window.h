// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/display_metrics.h"
#include "pomdog/application/window_mode.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class Rect2D;
enum class MouseCursor : u8;
} // namespace pomdog

namespace pomdog {

/// GameWindow represents an application window used by the game.
///
/// GameWindow provides a platform-independent interface for querying and
/// requesting changes to window state, such as the title, client-area bounds,
/// resize permission, cursor state, and window mode.
///
/// Requests that affect platform window state are generally deferred to a frame
/// boundary. This keeps input coordinates, client bounds, the back buffer,
/// the effective pixel ratio, the window mode, and rendering state consistent
/// during a frame.
class POMDOG_EXPORT GameWindow {
public:
    /// Creates a game window.
    GameWindow();

    GameWindow(const GameWindow&) = delete;
    GameWindow& operator=(const GameWindow&) = delete;

    virtual ~GameWindow();

    /// Returns true if the user can resize the window.
    [[nodiscard]] virtual bool
    getAllowUserResizing() const = 0;

    /// Requests a change to the window's user-resizing permission.
    ///
    /// The request is not applied immediately; it is deferred to the next
    /// frame boundary so that the window state stays consistent during a
    /// frame.
    ///
    /// `allowResizing` is the new value of the resizing flag.
    virtual void
    requestAllowUserResizing(bool allowResizing) = 0;

    /// Returns the title of the window.
    [[nodiscard]] virtual std::string
    getTitle() const = 0;

    /// Requests a new title for the window.
    ///
    /// The request is not applied immediately; it is deferred to the next
    /// frame boundary.
    ///
    /// `title` is the new title of the window.
    virtual void
    requestTitle(const std::string& title) = 0;

    /// Returns the committed client-area bounds of the window in logical
    /// pixels.
    ///
    /// The returned Rect2D is the snapshot committed at the start of the
    /// current frame and stays constant until the next frame boundary. Use
    /// this for UI layout, mouse / touch hit testing, and any coordinate that
    /// must remain visually consistent across DPI levels. For GPU resources
    /// sized to match the back buffer, use
    /// `gpu::PresentationParameters::backBufferWidth/backBufferHeight` or
    /// `getDisplayMetrics().backBufferWidth/backBufferHeight` instead, which
    /// are in physical pixels.
    ///
    /// To check whether a pending resize request is outstanding, use
    /// `getPendingClientBounds()`.
    [[nodiscard]] virtual Rect2D
    getClientBounds() const = 0;

    /// Requests a new client-area size and position for the window in
    /// logical pixels.
    ///
    /// The request is not applied immediately. Pomdog applies accepted window
    /// requests at a frame boundary so that input coordinates, client bounds,
    /// and render-target size stay consistent during a frame.
    ///
    /// The platform may clamp, ignore, or reject the requested bounds. The
    /// accepted bounds can be observed via `getClientBounds()` after
    /// `displayMetricsChanged` fires.
    ///
    /// The request is silently discarded when the current window mode is
    /// Fullscreen or Maximized.
    ///
    /// Returns an error if the arguments are immediately invalid, such as when
    /// the dimensions are zero or negative. Asynchronous rejection is reported
    /// through the `displayMetricsChanged` signal.
    [[nodiscard]] virtual std::unique_ptr<Error>
    requestClientBounds(const Rect2D& clientBounds) noexcept = 0;

    /// Returns the pending client-bounds request, if any, in logical pixels.
    ///
    /// The returned value is the request that has not yet been applied at a
    /// frame boundary.
    [[nodiscard]] virtual std::optional<Rect2D>
    getPendingClientBounds() const noexcept = 0;

    /// Returns the effective pixel ratio (physical / logical) for the
    /// current frame.
    ///
    /// On a standard 96 DPI desktop this returns 1.0. On Retina or 4K scaled
    /// displays it is typically 2.0. On desktop (Win32, macOS) this is the
    /// platform-reported ratio; on the Emscripten canvas it is clamped by
    /// `HighDPISettings::maxPixelRatio`. Returns 1.0 when `HighDPIMode::Disabled`
    /// is selected.
    ///
    /// The returned value is the snapshot committed at the start of the
    /// current frame and stays constant until the next frame boundary.
    [[nodiscard]] virtual f32
    getPixelRatio() const noexcept = 0;

    /// Returns the full display-metrics snapshot for the current frame.
    [[nodiscard]] virtual DisplayMetrics
    getDisplayMetrics() const noexcept = 0;

    /// Returns true if the mouse cursor is visible.
    [[nodiscard]] virtual bool
    isMouseCursorVisible() const = 0;

    /// Sets the visibility of the mouse cursor in the window.
    ///
    /// `visible` is the new value of the cursor visibility flag.
    virtual void
    setMouseCursorVisible(bool visible) = 0;

    /// Sets the system-provided mouse cursor asset.
    ///
    /// `cursor` is the new mouse cursor asset.
    virtual void
    setMouseCursor(MouseCursor cursor) = 0;

    /// Returns the current committed window mode.
    ///
    /// The returned mode is the last mode confirmed at a frame boundary. To
    /// check for an outstanding pending request, use `getPendingWindowMode()`.
    [[nodiscard]] virtual WindowMode
    getWindowMode() const noexcept = 0;

    /// Requests a new window mode.
    ///
    /// The request is not guaranteed to complete immediately and may be
    /// rejected by the platform. The committed mode can be observed via
    /// `getWindowMode()` after `windowModeChanged` fires.
    ///
    /// Some modes are not supported on all platforms. For example,
    /// BrowserSoftFullscreen is only supported on Emscripten. Requesting an
    /// unsupported mode returns an error immediately.
    ///
    /// Returns an error if the requested mode is not supported or is otherwise
    /// immediately invalid.
    [[nodiscard]] virtual std::unique_ptr<Error>
    requestWindowMode(WindowMode windowMode) noexcept = 0;

    /// Returns the pending window-mode request, if any.
    ///
    /// The returned value is the request that has not yet been applied at a
    /// frame boundary.
    [[nodiscard]] virtual std::optional<WindowMode>
    getPendingWindowMode() const noexcept = 0;

    /// Fires when any field of `DisplayMetrics` changes.
    ///
    /// Triggers include user resizing, monitor moves between different DPIs,
    /// OS scale changes, browser `devicePixelRatio` changes, and window-mode
    /// transitions that affect the back buffer. The signal fires at most once
    /// per frame boundary and only when the snapshot actually changed.
    ///
    /// Subscribers should:
    ///   - recreate offscreen `RenderTarget2D` / `DepthStencilBuffer` whose
    ///     size depends on `backBufferWidth/backBufferHeight`,
    ///   - re-layout the UI based on `clientBounds`,
    ///   - update any cached projection matrix.
    Signal<void(const DisplayMetrics& metrics)> displayMetricsChanged;

    /// Fires when the committed window mode changes.
    ///
    /// This fires for both programmatic changes, such as `requestWindowMode()`,
    /// and user-triggered changes, such as maximizing via the title bar or the
    /// browser exiting fullscreen.
    Signal<void(WindowMode windowMode)> windowModeChanged;
};

} // namespace pomdog
