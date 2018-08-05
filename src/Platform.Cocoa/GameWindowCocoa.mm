// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GameWindowCocoa.hpp"
#include "CocoaWindowDelegate.hpp"
#include "Pomdog/Application/MouseCursor.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Cocoa {

GameWindowCocoa::GameWindowCocoa(
    NSWindow* nativeWindowIn,
    const std::shared_ptr<EventQueue>& eventQueueIn)
    : eventQueue(eventQueueIn)
    , nativeWindow(nativeWindowIn)
    , gameView(nil)
    , windowDelegate(nil)
    , isMouseCursorVisible(true)
{
    POMDOG_ASSERT(nativeWindow != nil);

    // Create WindowDelegate
    windowDelegate = [[CocoaWindowDelegate alloc] initWithEventQueue:eventQueue];
    [nativeWindow setDelegate:windowDelegate];
}

GameWindowCocoa::~GameWindowCocoa()
{
    // Remove delegate from window
    [nativeWindow setDelegate:nil];

    windowDelegate = nil;
    nativeWindow = nil;
}

bool GameWindowCocoa::GetAllowUserResizing() const
{
    NSUInteger styleMask = [nativeWindow styleMask];
    return (styleMask & NSResizableWindowMask) == NSResizableWindowMask;
}

void GameWindowCocoa::SetAllowUserResizing(bool allowResizing)
{
    POMDOG_ASSERT(nativeWindow != nil);

    NSUInteger styleMask = [nativeWindow styleMask];
    if (allowResizing) {
        styleMask |= NSResizableWindowMask;
        POMDOG_ASSERT((styleMask & NSResizableWindowMask) == NSResizableWindowMask);
    }
    else {
        styleMask |= NSResizableWindowMask;
        styleMask ^= NSResizableWindowMask;
        POMDOG_ASSERT((styleMask & NSResizableWindowMask) != NSResizableWindowMask);
    }

    dispatch_async(dispatch_get_main_queue(), [=] {
        [nativeWindow setStyleMask:styleMask];
    });
}

std::string GameWindowCocoa::GetTitle() const
{
    std::string title = [[nativeWindow title] UTF8String];
    return title;
}

void GameWindowCocoa::SetTitle(const std::string& title)
{
    dispatch_async(dispatch_get_main_queue(), [=] {
        [nativeWindow setTitle:[NSString stringWithUTF8String:title.c_str()]];
    });
}

Rectangle GameWindowCocoa::GetClientBounds() const
{
    POMDOG_ASSERT([nativeWindow contentView] != nil);

    NSRect bounds = [[nativeWindow contentView] bounds];

    if (gameView != nil) {
        bounds = [gameView bounds];
    }
    NSRect rect = [nativeWindow convertRectToScreen:bounds];

    return Rectangle(
        rect.origin.x,
        rect.origin.y,
        bounds.size.width,
        bounds.size.height);
}

void GameWindowCocoa::SetClientBounds(const Rectangle& clientBounds)
{
    NSRect bounds = NSMakeRect(
        clientBounds.X,
        clientBounds.Y,
        clientBounds.Width,
        clientBounds.Height);

    dispatch_async(dispatch_get_main_queue(), [=] {
        [nativeWindow setFrame:bounds display:YES animate:NO];
    });
}

bool GameWindowCocoa::IsMouseCursorVisible() const
{
    return isMouseCursorVisible;
}

void GameWindowCocoa::SetMouseCursorVisible(bool visibleIn)
{
    isMouseCursorVisible = visibleIn;

    if (isMouseCursorVisible) {
        [NSCursor unhide];
    }
    else {
        [NSCursor hide];
    }
}

void GameWindowCocoa::SetMouseCursor(MouseCursor cursor)
{
    auto nativeCursor = ([cursor]() -> NSCursor* {
        switch (cursor) {
        case MouseCursor::Arrow:
            return [NSCursor arrowCursor];
        case MouseCursor::IBeam:
            return [NSCursor IBeamCursor];
        case MouseCursor::PointingHand:
            return [NSCursor pointingHandCursor];
        case MouseCursor::ResizeHorizontal:
            return [NSCursor resizeLeftRightCursor];
        case MouseCursor::ResizeVertical:
            return [NSCursor resizeUpDownCursor];
        }
    })();

    [nativeCursor set];
}

// MARK: - Low-Level API for GameHostCocoa

bool GameWindowCocoa::IsMinimized() const noexcept
{
    return [nativeWindow isMiniaturized] == YES;
}

void GameWindowCocoa::SetView(NSView* gameViewIn) noexcept
{
    gameView = gameViewIn;
}

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
