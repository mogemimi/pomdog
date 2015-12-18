// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameWindow.hpp"
#include "Pomdog/Signals/EventQueue.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#import <Cocoa/Cocoa.h>

@class NSWindow;
@class NSView;
@class CocoaWindowDelegate;

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class GameWindowCocoa final: public GameWindow {
public:
    GameWindowCocoa(
        NSWindow* nativeWindow,
        std::shared_ptr<EventQueue> const& eventQueue);

    ~GameWindowCocoa();

    bool GetAllowUserResizing() const override;

    void SetAllowUserResizing(bool allowResizing) override;

    std::string GetTitle() const override;

    void SetTitle(std::string const& title) override;

    Rectangle GetClientBounds() const override;

    void SetClientBounds(Rectangle const& clientBounds) override;

    bool IsMouseCursorVisible() const override;

    void SetMouseCursorVisible(bool visible) override;

    void SetMouseCursor(MouseCursor cursor) override;

    ///@return true if the window is minimized, false otherwise.
    bool IsMinimized() const noexcept;

    void SetView(NSView* gameView) noexcept;

private:
    std::shared_ptr<EventQueue> eventQueue;
    __weak NSWindow* nativeWindow;
    __weak NSView* gameView;
    __strong CocoaWindowDelegate* windowDelegate;
    bool isMouseCursorVisible;
};

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
