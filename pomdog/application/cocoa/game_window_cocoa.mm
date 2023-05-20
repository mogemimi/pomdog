// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/cocoa/game_window_cocoa.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/application/system_events.h"
#include "pomdog/signals/event_queue.h"
#include "pomdog/utility/assert.h"
#include <utility>

using pomdog::detail::SystemEvent;
using pomdog::detail::SystemEventKind;
using pomdog::EventQueue;

@interface PomdogNSWindowDelegate : NSObject <NSWindowDelegate>

- (instancetype)initWithEventQueue:(std::shared_ptr<pomdog::EventQueue<pomdog::detail::SystemEvent>>)eventQueue;

@end

@implementation PomdogNSWindowDelegate {
    std::shared_ptr<EventQueue<SystemEvent>> eventQueue;
}

- (instancetype)initWithEventQueue:(std::shared_ptr<EventQueue<SystemEvent>>)eventQueueIn
{
    self = [super init];
    if (self) {
        POMDOG_ASSERT(eventQueueIn);
        eventQueue = std::move(eventQueueIn);
    }
    return self;
}

- (BOOL)windowShouldClose:(id)sender
{
    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::WindowShouldCloseEvent,
    });
    return NO;
}

- (void)windowWillClose:(NSNotification*)notification
{
    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::WindowWillCloseEvent,
    });
}

@end

namespace pomdog::detail::cocoa {

GameWindowCocoa::GameWindowCocoa() noexcept
{
}

GameWindowCocoa::~GameWindowCocoa() noexcept
{
    dispatch_async(dispatch_get_main_queue(), ^(void) {
        // NOTE: Remove delegate from window.
        [nativeWindow setDelegate:nil];

#if !__has_feature(objc_arc)
        if (windowDelegate != nil) {
            [windowDelegate release];
        }
#endif
        windowDelegate = nil;

        if (nativeWindow != nil) {
            [nativeWindow close];
#if !__has_feature(objc_arc)
            [nativeWindow release];
#endif
        }
        nativeWindow = nil;
    });
}

std::unique_ptr<Error>
GameWindowCocoa::Initialize(
    NSWindow* nativeWindowIn,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueueIn) noexcept
{
    this->nativeWindow = nativeWindowIn;
    this->eventQueue = eventQueueIn;

    POMDOG_ASSERT(nativeWindow != nil);

    // NOTE: Create a window delegate for handling window events.
    windowDelegate = [[PomdogNSWindowDelegate alloc] initWithEventQueue:eventQueue];
    [nativeWindow setDelegate:windowDelegate];

    return nullptr;
}

bool GameWindowCocoa::GetAllowUserResizing() const
{
    NSUInteger styleMask = [nativeWindow styleMask];
    return (styleMask & NSWindowStyleMaskResizable) == NSWindowStyleMaskResizable;
}

void GameWindowCocoa::SetAllowUserResizing(bool allowResizing)
{
    POMDOG_ASSERT(nativeWindow != nil);

    NSUInteger styleMask = [nativeWindow styleMask];
    if (allowResizing) {
        styleMask |= NSWindowStyleMaskResizable;
        POMDOG_ASSERT((styleMask & NSWindowStyleMaskResizable) == NSWindowStyleMaskResizable);
    }
    else {
        styleMask |= NSWindowStyleMaskResizable;
        styleMask ^= NSWindowStyleMaskResizable;
        POMDOG_ASSERT((styleMask & NSWindowStyleMaskResizable) != NSWindowStyleMaskResizable);
    }

    dispatch_async(dispatch_get_main_queue(), [this, styleMask] {
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
    dispatch_async(dispatch_get_main_queue(), [this, title = title] {
        auto str = [NSString stringWithUTF8String:title.data()];
        if (str != nil) {
            [nativeWindow setTitle:str];
        }
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
        clientBounds.x,
        clientBounds.y,
        clientBounds.width,
        clientBounds.height);

    dispatch_async(dispatch_get_main_queue(), [this, bounds] {
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

} // namespace pomdog::detail::cocoa
