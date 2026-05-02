// Copyright mogemimi. Distributed under the MIT license.

#pragma once

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

/// Represents an application window used by the game.
///
/// GameWindow provides a platform-independent interface for querying and
/// requesting changes to window state, such as the title, client-area bounds,
/// resize permission, cursor state, and window mode.
///
/// Requests that affect platform window state are generally deferred to a frame
/// boundary. This keeps input coordinates, client bounds, window mode, and
/// rendering state consistent during a frame.
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

    /// Returns the committed client-area bounds of the window.
    ///
    /// The returned Rect2D describes the last client-area position and size
    /// confirmed by the platform at a frame boundary. To check whether a
    /// pending resize request is outstanding, use `getPendingClientBounds()`.
    [[nodiscard]] virtual Rect2D
    getClientBounds() const = 0;

    /// Requests a new client-area size and position for the window.
    ///
    /// The request is not applied immediately. Pomdog applies accepted window
    /// requests at a frame boundary so that input coordinates, client bounds,
    /// and render-target size stay consistent during a frame.
    ///
    /// The platform may clamp, ignore, or reject the requested bounds. The
    /// accepted bounds can be observed via `getClientBounds()` after
    /// `clientSizeChanged` fires.
    ///
    /// The request is silently discarded when the current window mode is
    /// Fullscreen or Maximized.
    ///
    /// Returns an error if the arguments are immediately invalid, such as when
    /// the dimensions are zero or negative. Asynchronous rejection is reported
    /// through the `clientSizeChanged` signal.
    [[nodiscard]] virtual std::unique_ptr<Error>
    requestClientBounds(const Rect2D& clientBounds) noexcept = 0;

    /// Returns the pending client-bounds request, if any.
    ///
    /// The returned value is the request that has not yet been applied at a
    /// frame boundary.
    [[nodiscard]] virtual std::optional<Rect2D>
    getPendingClientBounds() const noexcept = 0;

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

    /// Signal that fires when the client-area size changes.
    Signal<void(int width, int height)> clientSizeChanged;

    /// Signal that fires when the committed window mode changes.
    ///
    /// This fires for both programmatic changes, such as `requestWindowMode()`,
    /// and user-triggered changes, such as maximizing via the title bar or the
    /// browser exiting fullscreen.
    Signal<void(WindowMode windowMode)> windowModeChanged;
};

} // namespace pomdog
