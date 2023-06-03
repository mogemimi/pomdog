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
    eventQueue->enqueue(SystemEvent{
        .kind = SystemEventKind::WindowShouldCloseEvent,
    });
    return NO;
}

- (void)windowWillClose:(NSNotification*)notification
{
    eventQueue->enqueue(SystemEvent{
        .kind = SystemEventKind::WindowWillCloseEvent,
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
        [nativeWindow_ setDelegate:nil];

#if !__has_feature(objc_arc)
        if (windowDelegate_ != nil) {
            [windowDelegate_ release];
        }
#endif
        windowDelegate_ = nil;

        if (nativeWindow_ != nil) {
            [nativeWindow_ close];
#if !__has_feature(objc_arc)
            [nativeWindow_ release];
#endif
        }
        nativeWindow_ = nil;
    });
}

std::unique_ptr<Error>
GameWindowCocoa::initialize(
    NSWindow* nativeWindowIn,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueueIn) noexcept
{
    nativeWindow_ = nativeWindowIn;
    eventQueue_ = eventQueueIn;

    POMDOG_ASSERT(nativeWindow_ != nil);

    // NOTE: Create a window delegate for handling window events.
    windowDelegate_ = [[PomdogNSWindowDelegate alloc] initWithEventQueue:eventQueue_];
    [nativeWindow_ setDelegate:windowDelegate_];

    return nullptr;
}

bool GameWindowCocoa::getAllowUserResizing() const
{
    NSUInteger styleMask = [nativeWindow_ styleMask];
    return (styleMask & NSWindowStyleMaskResizable) == NSWindowStyleMaskResizable;
}

void GameWindowCocoa::setAllowUserResizing(bool allowResizing)
{
    POMDOG_ASSERT(nativeWindow_ != nil);

    NSUInteger styleMask = [nativeWindow_ styleMask];
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
        [nativeWindow_ setStyleMask:styleMask];
    });
}

std::string GameWindowCocoa::getTitle() const
{
    std::string title = [[nativeWindow_ title] UTF8String];
    return title;
}

void GameWindowCocoa::setTitle(const std::string& title)
{
    dispatch_async(dispatch_get_main_queue(), [this, title = title] {
        auto str = [NSString stringWithUTF8String:title.data()];
        if (str != nil) {
            [nativeWindow_ setTitle:str];
        }
    });
}

Rectangle GameWindowCocoa::getClientBounds() const
{
    POMDOG_ASSERT([nativeWindow_ contentView] != nil);

    NSRect bounds = [[nativeWindow_ contentView] bounds];

    if (gameView_ != nil) {
        bounds = [gameView_ bounds];
    }
    NSRect rect = [nativeWindow_ convertRectToScreen:bounds];

    return Rectangle(
        rect.origin.x,
        rect.origin.y,
        bounds.size.width,
        bounds.size.height);
}

void GameWindowCocoa::setClientBounds(const Rectangle& clientBounds)
{
    NSRect bounds = NSMakeRect(
        clientBounds.x,
        clientBounds.y,
        clientBounds.width,
        clientBounds.height);

    dispatch_async(dispatch_get_main_queue(), [this, bounds] {
        [nativeWindow_ setFrame:bounds display:YES animate:NO];
    });
}

bool GameWindowCocoa::isMouseCursorVisible() const
{
    return isMouseCursorVisible_;
}

void GameWindowCocoa::setMouseCursorVisible(bool visibleIn)
{
    isMouseCursorVisible_ = visibleIn;

    if (isMouseCursorVisible_) {
        [NSCursor unhide];
    }
    else {
        [NSCursor hide];
    }
}

void GameWindowCocoa::setMouseCursor(MouseCursor cursor)
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

bool GameWindowCocoa::isMinimized() const noexcept
{
    return [nativeWindow_ isMiniaturized] == YES;
}

void GameWindowCocoa::setView(NSView* gameViewIn) noexcept
{
    gameView_ = gameViewIn;
}

} // namespace pomdog::detail::cocoa
