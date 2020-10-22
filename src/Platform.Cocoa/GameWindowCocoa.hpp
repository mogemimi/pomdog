// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameWindow.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Signals/EventQueue.hpp"
#import <Cocoa/Cocoa.h>

@class NSWindow;
@class NSView;
@class CocoaWindowDelegate;

namespace Pomdog::Detail {
class SystemEvent;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Cocoa {

class GameWindowCocoa final : public GameWindow {
public:
    GameWindowCocoa(
        NSWindow* nativeWindow,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue);

    ~GameWindowCocoa();

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
    __strong CocoaWindowDelegate* windowDelegate = nil;
    bool isMouseCursorVisible = true;
};

} // namespace Pomdog::Detail::Cocoa
