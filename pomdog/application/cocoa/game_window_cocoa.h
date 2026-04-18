// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Cocoa/Cocoa.h>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

@class NSWindow;
@class NSView;
@class PomdogNSWindowDelegate;

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail {
class SystemEventQueue;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {

class GameWindowCocoa final : public GameWindow {
public:
    GameWindowCocoa() noexcept;

    ~GameWindowCocoa() noexcept override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        NSWindow* nativeWindow,
        const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept;

    bool getAllowUserResizing() const override;

    void setAllowUserResizing(bool allowResizing) override;

    std::string getTitle() const override;

    void setTitle(const std::string& title) override;

    Rect2D getClientBounds() const override;

    void setClientBounds(const Rect2D& clientBounds) override;

    bool isMouseCursorVisible() const override;

    void setMouseCursorVisible(bool visible) override;

    void setMouseCursor(MouseCursor cursor) override;

    ///@return true if the window is minimized, false otherwise.
    [[nodiscard]] bool isMinimized() const noexcept;

    void setView(NSView* gameView) noexcept;

private:
    std::shared_ptr<SystemEventQueue> eventQueue_;
    __weak NSWindow* nativeWindow_ = nil;
    __weak NSView* gameView_ = nil;
    __strong PomdogNSWindowDelegate* windowDelegate_ = nil;
    bool isMouseCursorVisible_ = true;
};

} // namespace pomdog::detail::cocoa
