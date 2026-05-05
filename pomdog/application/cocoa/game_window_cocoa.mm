// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/cocoa/game_window_cocoa.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/application/window_mode.h"
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

- (instancetype)initWithEventQueue:(std::shared_ptr<pomdog::detail::SystemEventQueue>)eventQueue
                 windowModeChanged:(std::function<void(pomdog::WindowMode)>)onWindowModeChanged;

@end

@implementation PomdogNSWindowDelegate {
    std::shared_ptr<SystemEventQueue> eventQueue;
    std::function<void(pomdog::WindowMode)> windowModeChangedCallback_;
}

- (instancetype)initWithEventQueue:(std::shared_ptr<SystemEventQueue>)eventQueueIn
                 windowModeChanged:(std::function<void(pomdog::WindowMode)>)callback
{
    self = [super init];
    if (self) {
        POMDOG_ASSERT(eventQueueIn);
        eventQueue = std::move(eventQueueIn);
        windowModeChangedCallback_ = std::move(callback);
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

- (void)windowDidEnterFullScreen:(NSNotification*)notification
{
    // NOTE: macOS native fullscreen transition completed.
    if (windowModeChangedCallback_) {
        windowModeChangedCallback_(pomdog::WindowMode::Fullscreen);
    }
}

- (void)windowDidExitFullScreen:(NSNotification*)notification
{
    // NOTE: macOS native fullscreen exit completed.
    if (windowModeChangedCallback_) {
        windowModeChangedCallback_(pomdog::WindowMode::Windowed);
    }
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

    // NOTE: Pending requests - set by `request...()` and applied at the next
    //       `applyPendingWindowRequests()` call at the frame boundary.
    std::optional<bool> pendingAllowUserResizing_;
    std::optional<std::string> pendingTitle_;
    std::optional<Rect2D> pendingClientBounds_;
    std::optional<WindowMode> pendingWindowMode_;

    // Committed window state.
    WindowMode windowMode_ = WindowMode::Windowed;
    NSRect savedWindowedFrame_ = NSZeroRect;

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
        // The callback updates windowMode_ and enqueues WindowModeChangedEvent
        // when macOS native fullscreen transitions complete (async).
        windowDelegate_ = [[PomdogNSWindowDelegate alloc]
            initWithEventQueue:eventQueue_
             windowModeChanged:[this](pomdog::WindowMode newMode) {
                 windowMode_ = newMode;
                 if (eventQueue_) {
                     using pomdog::detail::SystemEventKind;
                     eventQueue_->enqueue(SystemEvent{
                         .kind = SystemEventKind::WindowModeChangedEvent,
                         .data = WindowModeChangedEvent{.windowMode = newMode},
                     });
                 }
             }];
        [nativeWindow_ setDelegate:windowDelegate_];

        // NOTE: Enable native macOS fullscreen capability.
        [nativeWindow_ setCollectionBehavior:
                [nativeWindow_ collectionBehavior] | NSWindowCollectionBehaviorFullScreenPrimary];

        return nullptr;
    }

    bool getAllowUserResizing() const override
    {
        NSUInteger styleMask = [nativeWindow_ styleMask];
        return (styleMask & NSWindowStyleMaskResizable) == NSWindowStyleMaskResizable;
    }

    void requestAllowUserResizing(bool allowResizing) override
    {
        pendingAllowUserResizing_ = allowResizing;
    }

    std::string getTitle() const override
    {
        std::string title = [[nativeWindow_ title] UTF8String];
        return title;
    }

    void requestTitle(const std::string& title) override
    {
        pendingTitle_ = title;
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

    [[nodiscard]] std::unique_ptr<Error>
    requestClientBounds(const Rect2D& clientBoundsIn) noexcept override
    {
        if (clientBoundsIn.width <= 0 || clientBoundsIn.height <= 0) {
            return errors::make("requestClientBounds: width and height must be positive");
        }
        pendingClientBounds_ = clientBoundsIn;
        return nullptr;
    }

    [[nodiscard]] std::optional<Rect2D>
    getPendingClientBounds() const noexcept override
    {
        return pendingClientBounds_;
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

    WindowMode
    getWindowMode() const noexcept override
    {
        return windowMode_;
    }

    [[nodiscard]] std::unique_ptr<Error>
    requestWindowMode(WindowMode windowMode) noexcept override
    {
        if (windowMode == WindowMode::BrowserSoftFullscreen) {
            return errors::make("BrowserSoftFullscreen is not supported on macOS.");
        }
        pendingWindowMode_ = windowMode;
        return nullptr;
    }

    [[nodiscard]] std::optional<WindowMode>
    getPendingWindowMode() const noexcept override
    {
        return pendingWindowMode_;
    }

    void
    applyPendingWindowRequests() noexcept override
    {
        if (pendingWindowMode_.has_value()) {
            applyWindowMode(*pendingWindowMode_);
            pendingWindowMode_ = std::nullopt;
        }

        if (pendingClientBounds_.has_value()) {
            const Rect2D bounds = *pendingClientBounds_;
            pendingClientBounds_ = std::nullopt;
            NSRect nsRect = NSMakeRect(bounds.x, bounds.y, bounds.width, bounds.height);
            dispatch_async(dispatch_get_main_queue(), [this, nsRect] {
                [nativeWindow_ setFrame:nsRect display:YES animate:NO];
            });
        }

        if (pendingTitle_.has_value()) {
            const std::string title = *pendingTitle_;
            pendingTitle_ = std::nullopt;
            dispatch_async(dispatch_get_main_queue(), [this, title] {
                auto str = [NSString stringWithUTF8String:title.data()];
                if (str != nil) {
                    [nativeWindow_ setTitle:static_cast<NSString* _Nonnull>(str)];
                }
            });
        }

        if (pendingAllowUserResizing_.has_value()) {
            const bool allow = *pendingAllowUserResizing_;
            pendingAllowUserResizing_ = std::nullopt;
            POMDOG_ASSERT(nativeWindow_ != nil);
            NSUInteger styleMask = [nativeWindow_ styleMask];
            if (allow) {
                styleMask |= NSWindowStyleMaskResizable;
            }
            else {
                styleMask &= ~NSWindowStyleMaskResizable;
            }
            dispatch_async(dispatch_get_main_queue(), [this, styleMask] {
                [nativeWindow_ setStyleMask:styleMask];
            });
        }
    }

    void setView(NSView* gameViewIn) noexcept override
    {
        gameView_ = gameViewIn;
    }

private:
    void applyWindowMode(WindowMode windowMode) noexcept
    {
        if (windowMode_ == windowMode) {
            return;
        }

        // NOTE: Exit current mode first.
        if (windowMode_ == WindowMode::Fullscreen) {
            // NOTE: macOS native fullscreen exit is asynchronous.
            // windowDidExitFullScreen delegate will update windowMode_ and enqueue
            // WindowModeChangedEvent when the transition completes.
            // We cannot chain into the target mode here; caller must re-request
            // after the fullscreen exit completes.
            dispatch_async(dispatch_get_main_queue(), [this] {
                [nativeWindow_ toggleFullScreen:nil];
            });
            return;
        }
        else if (windowMode_ == WindowMode::BorderlessWindowed) {
            // NOTE: Restore titled window style before entering any new mode.
            const bool canResize = getAllowUserResizing();
            const NSRect frame = savedWindowedFrame_;
            dispatch_async(dispatch_get_main_queue(), [this, canResize, frame] {
                NSUInteger mask =
                    NSWindowStyleMaskTitled |
                    NSWindowStyleMaskClosable |
                    NSWindowStyleMaskMiniaturizable;
                if (canResize) {
                    mask |= NSWindowStyleMaskResizable;
                }
                [nativeWindow_ setStyleMask:mask];
                if (!NSIsEmptyRect(frame)) {
                    [nativeWindow_ setFrame:frame display:YES animate:NO];
                }
                [nativeWindow_ makeKeyAndOrderFront:nil];
            });
            savedWindowedFrame_ = NSZeroRect;
            windowMode_ = WindowMode::Windowed;
            if (eventQueue_) {
                eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::WindowModeChangedEvent,
                    .data = WindowModeChangedEvent{.windowMode = WindowMode::Windowed},
                });
            }
            if (windowMode == WindowMode::Windowed) {
                return;
            }
        }
        else if (windowMode_ == WindowMode::Maximized) {
            dispatch_async(dispatch_get_main_queue(), [this] {
                if ([nativeWindow_ isZoomed]) {
                    [nativeWindow_ zoom:nil];
                }
            });
            windowMode_ = WindowMode::Windowed;
            if (eventQueue_) {
                eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::WindowModeChangedEvent,
                    .data = WindowModeChangedEvent{.windowMode = WindowMode::Windowed},
                });
            }
            if (windowMode == WindowMode::Windowed) {
                return;
            }
        }

        // NOTE: Enter new mode.
        switch (windowMode) {
        case WindowMode::Fullscreen: {
            // NOTE: macOS native fullscreen transition is asynchronous.
            // windowDidEnterFullScreen delegate will update windowMode_ when complete.
            dispatch_async(dispatch_get_main_queue(), [this] {
                [nativeWindow_ toggleFullScreen:nil];
            });
            // NOTE: Do not update windowMode_ optimistically; wait for delegate.
            return;
        }
        case WindowMode::BorderlessWindowed: {
            const bool canResize = getAllowUserResizing();
            dispatch_async(dispatch_get_main_queue(), [this, canResize] {
                // NOTE: Save the current frame so we can restore it when switching back to Windowed.
                savedWindowedFrame_ = [nativeWindow_ frame];
                NSUInteger mask = NSWindowStyleMaskBorderless;
                if (canResize) {
                    mask |= NSWindowStyleMaskResizable;
                }
                [nativeWindow_ setStyleMask:mask];
                [nativeWindow_ makeKeyAndOrderFront:nil];
            });
            break;
        }
        case WindowMode::Maximized: {
            dispatch_async(dispatch_get_main_queue(), [this] {
                if (![nativeWindow_ isZoomed]) {
                    [nativeWindow_ zoom:nil];
                }
            });
            break;
        }
        case WindowMode::Windowed:
            // Already transitioned above (from Maximized/BorderlessWindowed).
            break;
        case WindowMode::BrowserSoftFullscreen:
            // NOTE: Validated and rejected in requestWindowMode(); should not reach here.
            return;
        }

        // NOTE: Optimistically update the mode for synchronous transitions
        // (BorderlessWindowed, Maximized, Windowed). Fullscreen is updated by delegate.
        windowMode_ = windowMode;
        if (eventQueue_) {
            eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::WindowModeChangedEvent,
                .data = WindowModeChangedEvent{.windowMode = windowMode_},
            });
        }
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
