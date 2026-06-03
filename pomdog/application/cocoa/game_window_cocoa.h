// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Cocoa/Cocoa.h>
#include <memory>
#include <optional>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

@class NSWindow;
@class NSView;

namespace pomdog {
class Error;
struct DisplayMetrics;
struct HighDPISettings;
} // namespace pomdog

namespace pomdog::detail {
class SystemEventQueue;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {

/// GameWindowCocoa is the macOS (Cocoa) implementation of GameWindow.
/// Wraps an NSWindow and provides the pending-request pattern used by both
/// GameHostCocoa (OpenGL) and GameHostMetal to apply window-mode, bounds,
/// title, and resizing changes at a safe frame boundary.
class GameWindowCocoa : public GameWindow {
public:
    GameWindowCocoa();

    ~GameWindowCocoa() override;

    [[nodiscard]] static std::tuple<std::shared_ptr<GameWindowCocoa>, std::unique_ptr<Error>>
    create(
        NSWindow* nativeWindow,
        const HighDPISettings& highDPI,
        const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept;

    /// Returns true if the window is minimized, false otherwise.
    [[nodiscard]] virtual bool
    isMinimized() const noexcept = 0;

    virtual void
    setView(NSView* gameView) noexcept = 0;

    /// Applies any pending window requests (mode, bounds, title, resizing).
    /// Called by GameHostCocoa at the start of each frame, before `game->update()`.
    virtual void
    applyPendingWindowRequests() noexcept = 0;

    /// Commits the latest platform display metrics if they differ from the
    /// currently committed snapshot. Called by `GameHostCocoa` /
    /// `GameHostMetal` during `doEvents()` after a view-resize or backing
    /// scale change. Returns the new committed snapshot, or nullopt when
    /// nothing changed since the last commit.
    [[nodiscard]] virtual std::optional<DisplayMetrics>
    commitDisplayMetricsIfChanged() noexcept = 0;
};

} // namespace pomdog::detail::cocoa
