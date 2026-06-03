// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/application/window_mode.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
struct DisplayMetrics;
struct HighDPISettings;
} // namespace pomdog

namespace pomdog::detail {
class SystemEventQueue;
} // namespace pomdog::detail

namespace pomdog::detail::emscripten {

/// GameWindowEmscripten is the Emscripten/WebAssembly implementation of GameWindow.
/// Controls the size and fullscreen state of an HTML canvas element, and
/// implements the pending-request pattern so that changes are applied at the
/// start of each frame by GameHostEmscripten.
class GameWindowEmscripten : public GameWindow {
public:
    GameWindowEmscripten() noexcept;

    ~GameWindowEmscripten() noexcept override;

    [[nodiscard]] virtual const std::string&
    getTargetCanvas() const noexcept = 0;

    /// Applies any pending window requests (mode, bounds, title, resizing).
    /// Called by GameHostEmscripten at the start of each frame, before
    /// system events are dispatched and before `game->update()`.
    virtual void
    applyPendingWindowRequests() noexcept = 0;

    /// Commits the latest platform display metrics if they differ from the
    /// currently committed snapshot. Called by `GameHostEmscripten` during
    /// `doEvents()` after a canvas resize, fullscreen transition, or
    /// devicePixelRatio change. Returns the new committed snapshot, or
    /// nullopt when nothing changed since the last commit.
    [[nodiscard]] virtual std::optional<DisplayMetrics>
    commitDisplayMetricsIfChanged() noexcept = 0;

    [[nodiscard]] static std::tuple<std::shared_ptr<GameWindowEmscripten>, std::unique_ptr<Error>>
    create(
        const std::string& targetCanvas,
        i32 clientWidth,
        i32 clientHeight,
        const HighDPISettings& highDPI,
        const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept;
};

} // namespace pomdog::detail::emscripten
