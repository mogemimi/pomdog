// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.hpp"
#include "pomdog/math/rectangle.hpp"
#include "pomdog/signals/forward_declarations.hpp"
#import <Cocoa/Cocoa.h>
#include <memory>

@class NSWindow;
@class NSView;
@class PomdogNSWindowDelegate;

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail {
class SystemEvent;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {

class GameWindowCocoa final : public GameWindow {
public:
    GameWindowCocoa() noexcept;

    ~GameWindowCocoa() noexcept override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        NSWindow* nativeWindow,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue) noexcept;

    bool GetAllowUserResizing() const override;

    void SetAllowUserResizing(bool allowResizing) override;

    std::string GetTitle() const override;

    void SetTitle(const std::string& title) override;

    Rectangle GetClientBounds() const override;

    void SetClientBounds(const Rectangle& clientBounds) override;

    bool IsMouseCursorVisible() const override;

    void SetMouseCursorVisible(bool visible) override;

    void SetMouseCursor(MouseCursor cursor) override;

    ///@return true if the window is minimized, false otherwise.
    bool IsMinimized() const noexcept;

    void SetView(NSView* gameView) noexcept;

private:
    std::shared_ptr<EventQueue<SystemEvent>> eventQueue;
    __weak NSWindow* nativeWindow = nil;
    __weak NSView* gameView = nil;
    __strong PomdogNSWindowDelegate* windowDelegate = nil;
    bool isMouseCursorVisible = true;
};

} // namespace pomdog::detail::cocoa
