// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Cocoa/Cocoa.h>
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

@class NSWindow;
@class NSView;

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail {
class SystemEventQueue;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {

class GameWindowCocoa : public GameWindow {
public:
    GameWindowCocoa();

    ~GameWindowCocoa() override;

    [[nodiscard]] static std::tuple<std::shared_ptr<GameWindowCocoa>, std::unique_ptr<Error>>
    create(
        NSWindow* nativeWindow,
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
};

} // namespace pomdog::detail::cocoa
