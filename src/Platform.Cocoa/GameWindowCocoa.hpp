// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEWINDOWCOCOA_BA8D88CA_HPP
#define POMDOG_GAMEWINDOWCOCOA_BA8D88CA_HPP

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
    GameWindowCocoa(NSWindow* nativeWindow,
        std::shared_ptr<EventQueue> const& eventQueue);

    ~GameWindowCocoa();

    bool AllowPlayerResizing() const override;

    void AllowPlayerResizing(bool allowResizing) override;

    std::string Title() const override;

    void Title(std::string const& title) override;

    Rectangle ClientBounds() const override;

    void ClientBounds(Rectangle const& clientBounds) override;

    bool IsMouseCursorVisible() const override;

    void IsMouseCursorVisible(bool visible) override;

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

#endif // POMDOG_GAMEWINDOWCOCOA_BA8D88CA_HPP
