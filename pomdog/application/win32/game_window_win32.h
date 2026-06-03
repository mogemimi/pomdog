// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/win32/windows_headers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

namespace pomdog {
struct DisplayMetrics;
struct HighDPISettings;
} // namespace pomdog

namespace pomdog::detail {
class SystemEventQueue;
} // namespace pomdog::detail

namespace pomdog::detail::win32 {

/// GameWindowWin32 is the Win32 implementation of GameWindow.
/// Wraps an HWND and implements the pending-request pattern so that window
/// mode, client bounds, title, and resizing-allowed changes are applied
/// atomically at the start of each frame by GameHostWin32.
class GameWindowWin32 : public GameWindow {
public:
    GameWindowWin32();

    virtual ~GameWindowWin32();

    [[nodiscard]] virtual bool
    isMinimized() const = 0;

    virtual void
    close() = 0;

    [[nodiscard]] virtual HWND
    getNativeWindowHandle() const = 0;

    /// Applies any pending window requests (mode, bounds, title, resizing).
    /// Called by `GameHostWin32` at the start of each frame, before events are
    /// dispatched and before `game.update()`.
    virtual void
    applyPendingWindowRequests() noexcept = 0;

    /// Commits the latest platform display metrics if they differ from the
    /// currently committed snapshot. Called by `GameHostWin32` during
    /// `doEvents()` after a resize, monitor move, or DPI change has been
    /// detected. Returns the new committed snapshot, or nullopt when nothing
    /// changed since the last commit.
    ///
    /// `pixelRatioHint` carries the pixel ratio reported by WM_DPICHANGED,
    /// if any; when present it is trusted instead of re-querying the window DPI,
    /// which can briefly lag behind the message.
    [[nodiscard]] virtual std::optional<DisplayMetrics>
    commitDisplayMetricsIfChanged(std::optional<f32> pixelRatioHint) noexcept = 0;

    [[nodiscard]] static std::tuple<std::shared_ptr<GameWindowWin32>, std::unique_ptr<Error>>
    create(
        HINSTANCE hInstance,
        int nCmdShow,
        HICON icon,
        HICON iconSmall,
        bool useOpenGL,
        const std::shared_ptr<SystemEventQueue>& eventQueue,
        const gpu::PresentationParameters& presentationParameters,
        const HighDPISettings& highDPI) noexcept;
};

} // namespace pomdog::detail::win32
