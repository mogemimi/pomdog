// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/cocoa/game_window_cocoa.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::SystemEvent;
using pomdog::detail::SystemEventKind;
using pomdog::detail::SystemEventQueue;

@interface PomdogNSWindowDelegate : NSObject <NSWindowDelegate>

- (instancetype)initWithEventQueue:(std::shared_ptr<pomdog::detail::SystemEventQueue>)eventQueue;

@end

@implementation PomdogNSWindowDelegate {
    std::shared_ptr<SystemEventQueue> eventQueue;
}

- (instancetype)initWithEventQueue:(std::shared_ptr<SystemEventQueue>)eventQueueIn
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
        .data = {},
    });
    return NO;
}

- (void)windowWillClose:(NSNotification*)notification
{
    eventQueue->enqueue(SystemEvent{
        .kind = SystemEventKind::WindowWillCloseEvent,
        .data = {},
    });
}

@end

namespace pomdog::detail::cocoa {
namespace {

class GameWindowCocoaImpl final : public GameWindowCocoa {
private:
    std::shared_ptr<SystemEventQueue> eventQueue_;
    __weak NSWindow* nativeWindow_ = nil;
    __weak NSView* gameView_ = nil;
    __strong PomdogNSWindowDelegate* windowDelegate_ = nil;
    bool isMouseCursorVisible_ = true;

public:
    ~GameWindowCocoaImpl() noexcept override
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

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        NSWindow* nativeWindowIn,
        const std::shared_ptr<SystemEventQueue>& eventQueueIn) noexcept
    {
        nativeWindow_ = nativeWindowIn;
        eventQueue_ = eventQueueIn;

        POMDOG_ASSERT(nativeWindow_ != nil);

        // NOTE: Create a window delegate for handling window events.
        windowDelegate_ = [[PomdogNSWindowDelegate alloc] initWithEventQueue:eventQueue_];
        [nativeWindow_ setDelegate:windowDelegate_];

        return nullptr;
    }

    bool getAllowUserResizing() const override
    {
        NSUInteger styleMask = [nativeWindow_ styleMask];
        return (styleMask & NSWindowStyleMaskResizable) == NSWindowStyleMaskResizable;
    }

    void setAllowUserResizing(bool allowResizing) override
    {
        POMDOG_ASSERT(nativeWindow_ != nil);

        NSUInteger styleMask = [nativeWindow_ styleMask];
        if (allowResizing) {
            styleMask |= NSWindowStyleMaskResizable;
            POMDOG_ASSERT((styleMask & NSWindowStyleMaskResizable) == NSWindowStyleMaskResizable);
        }
        else {
            styleMask &= ~NSWindowStyleMaskResizable;
            POMDOG_ASSERT((styleMask & NSWindowStyleMaskResizable) != NSWindowStyleMaskResizable);
        }

        dispatch_async(dispatch_get_main_queue(), [this, styleMask] {
            [nativeWindow_ setStyleMask:styleMask];
        });
    }

    std::string getTitle() const override
    {
        std::string title = [[nativeWindow_ title] UTF8String];
        return title;
    }

    void setTitle(const std::string& title) override
    {
        dispatch_async(dispatch_get_main_queue(), [this, title = title] {
            auto str = [NSString stringWithUTF8String:title.data()];
            if (str != nil) {
                [nativeWindow_ setTitle:static_cast<NSString* _Nonnull>(str)];
            }
        });
    }

    Rect2D getClientBounds() const override
    {
        POMDOG_ASSERT([nativeWindow_ contentView] != nil);

        NSRect bounds = [[nativeWindow_ contentView] bounds];

        if (gameView_ != nil) {
            bounds = [gameView_ bounds];
        }
        NSRect rect = [nativeWindow_ convertRectToScreen:bounds];

        return Rect2D{
            static_cast<i32>(rect.origin.x),
            static_cast<i32>(rect.origin.y),
            static_cast<i32>(bounds.size.width),
            static_cast<i32>(bounds.size.height)};
    }

    void setClientBounds(const Rect2D& clientBounds) override
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

    bool isMouseCursorVisible() const override
    {
        return isMouseCursorVisible_;
    }

    void setMouseCursorVisible(bool visibleIn) override
    {
        isMouseCursorVisible_ = visibleIn;

        if (isMouseCursorVisible_) {
            [NSCursor unhide];
        }
        else {
            [NSCursor hide];
        }
    }

    void setMouseCursor(MouseCursor cursor) override
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

    bool isMinimized() const noexcept override
    {
        return [nativeWindow_ isMiniaturized] == YES;
    }

    void setView(NSView* gameViewIn) noexcept override
    {
        gameView_ = gameViewIn;
    }
};

} // namespace

GameWindowCocoa::GameWindowCocoa() = default;

GameWindowCocoa::~GameWindowCocoa() = default;

std::tuple<std::shared_ptr<GameWindowCocoa>, std::unique_ptr<Error>>
GameWindowCocoa::create(
    NSWindow* nativeWindow,
    const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept
{
    auto window = std::make_shared<GameWindowCocoaImpl>();
    if (auto err = window->initialize(nativeWindow, eventQueue); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(window), nullptr);
}

} // namespace pomdog::detail::cocoa
