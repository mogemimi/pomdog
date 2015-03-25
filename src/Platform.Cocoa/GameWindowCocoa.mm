// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "GameWindowCocoa.hpp"
#include "OpenGLContextCocoa.hpp"
#include "CocoaOpenGLView.hpp"
#include "CocoaWindowDelegate.hpp"
#include "CocoaGameViewDelegate.hpp"
#include "Pomdog/Application/MouseCursor.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Cocoa {
//-----------------------------------------------------------------------
GameWindowCocoa::GameWindowCocoa(NSWindow* nativeWindowIn, std::shared_ptr<SystemEventDispatcher> const& eventDispatcher)
    : nativeWindow(nativeWindowIn)
    , openGLView(nil)
    , windowDelegate(nil)
    , viewDelegate(nil)
    , isMouseCursorVisible(true)
{
    POMDOG_ASSERT(nativeWindow);

#if !__has_feature(objc_arc)
    [nativeWindow retain];
#endif

    NSRect frameRect = [[nativeWindow contentView] bounds];

    // Create OpenGLView
    openGLView = [[CocoaOpenGLView alloc] initWithFrame:frameRect];
    [openGLView setHidden:NO];
    [openGLView setNeedsDisplay:YES];
    [[nativeWindow contentView] addSubview:openGLView];

    // Create WindowDelegate
    windowDelegate = [[CocoaWindowDelegate alloc] initWithEventDispatcher:eventDispatcher];
    [nativeWindow setDelegate:windowDelegate];

    // OpenGLView autoresize to fit nativeWindow's contentView
    [[nativeWindow contentView] setAutoresizesSubviews:YES];
    [openGLView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

    // Create ViewDelegate
    viewDelegate = [[CocoaGameViewDelegate alloc] initWithEventDispatcher:eventDispatcher];
    [viewDelegate setView:openGLView];
    [openGLView setDelegate:viewDelegate];
}
//-----------------------------------------------------------------------
GameWindowCocoa::~GameWindowCocoa()
{
    // Remove delegate from window:
    [nativeWindow setDelegate:nil];

    // Remove OpenGLView from window:
    [viewDelegate setView:nil];
    [openGLView setDelegate:nil];
    [openGLView removeFromSuperview];

#if !__has_feature(objc_arc)
    [viewDelegate release];
    [windowDelegate release];
    [openGLView release];
    [nativeWindow release];
#endif
}
//-----------------------------------------------------------------------
bool GameWindowCocoa::AllowPlayerResizing() const
{
    NSUInteger styleMask = [nativeWindow styleMask];
    return (styleMask & NSResizableWindowMask) == NSResizableWindowMask;
}
//-----------------------------------------------------------------------
void GameWindowCocoa::AllowPlayerResizing(bool allowResizing)
{
    POMDOG_ASSERT(nativeWindow);

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
    [nativeWindow setStyleMask:styleMask];
}
//-----------------------------------------------------------------------
std::string GameWindowCocoa::Title() const
{
    std::string title = [[nativeWindow title] UTF8String];
    return std::move(title);
}
//-----------------------------------------------------------------------
void GameWindowCocoa::Title(std::string const& title)
{
    [nativeWindow setTitle:[NSString stringWithUTF8String:title.c_str()]];
}
//-----------------------------------------------------------------------
Rectangle GameWindowCocoa::ClientBounds() const
{
    POMDOG_ASSERT([nativeWindow contentView] != nil);
    NSRect bounds = [[nativeWindow contentView] bounds];
    NSPoint origin = [nativeWindow frame].origin;

    NSSize windowSize = [nativeWindow frame].size;
    NSSize screenSize = [[nativeWindow screen] visibleFrame].size;

    return Rectangle(
        origin.x,
        screenSize.height - windowSize.height - origin.y,
        bounds.size.width,
        bounds.size.height);
}
//-----------------------------------------------------------------------
void GameWindowCocoa::ClientBounds(Rectangle const& clientBounds)
{
    auto bounds = NSMakeSize(clientBounds.Width, clientBounds.Height);
    [nativeWindow setContentSize:bounds];

    NSSize windowSize = [nativeWindow frame].size;
    NSSize screenSize = [[nativeWindow screen] visibleFrame].size;

    auto origin = NSMakePoint(clientBounds.X,
        screenSize.height - (clientBounds.Y + windowSize.height));
    [nativeWindow setFrameOrigin:origin];

    //NSRect bounds;
    //bounds.origin.x = clientBounds.X;
    //bounds.origin.y = clientBounds.Y;
    //bounds.size.width = clientBounds.Width;
    //bounds.size.height = clientBounds.Height;
    //[nativeWindow setFrame:bounds display:YES animate:YES];
}
//-----------------------------------------------------------------------
bool GameWindowCocoa::IsMouseCursorVisible() const
{
    return isMouseCursorVisible;
}
//-----------------------------------------------------------------------
void GameWindowCocoa::IsMouseCursorVisible(bool visibleIn)
{
    isMouseCursorVisible = visibleIn;

    if (isMouseCursorVisible) {
        [NSCursor unhide];
    }
    else {
        [NSCursor hide];
    }
}
//-----------------------------------------------------------------------
void GameWindowCocoa::SetMouseCursor(MouseCursor cursor)
{
    auto nativeCursor = ([cursor]()-> NSCursor* {
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
//-----------------------------------------------------------------------
#pragma mark - Low-Level API for GameHostCocoa
//-----------------------------------------------------------------------
bool GameWindowCocoa::IsMinimized() const
{
    return [nativeWindow isMiniaturized] == YES;
}
//-----------------------------------------------------------------------
void GameWindowCocoa::Close()
{
    // Removes the window from the screen list, which hides the window:
    //[nativeWindow orderOut:nil];

    [nativeWindow close];
}
//-----------------------------------------------------------------------
#pragma mark - OpenGLView
//-----------------------------------------------------------------------
void GameWindowCocoa::ResetGLContext(std::shared_ptr<OpenGLContextCocoa> const& contextIn)
{
    POMDOG_ASSERT(contextIn);
    openGLContext = contextIn;

    POMDOG_ASSERT(openGLView);
    POMDOG_ASSERT(openGLContext);
    [openGLView setOpenGLContext:openGLContext->NativeOpenGLContext()];
}
//-----------------------------------------------------------------------
void GameWindowCocoa::ResetGLContext()
{
    openGLContext.reset();

    POMDOG_ASSERT(openGLView);
    [openGLView clearGLContext];
}
//-----------------------------------------------------------------------
void GameWindowCocoa::SetRenderCallbackOnLiveResizing(std::function<void()> const& callback)
{
    POMDOG_ASSERT(callback);
    POMDOG_ASSERT(openGLView);
    [openGLView setRenderCallback: callback];
}
//-----------------------------------------------------------------------
void GameWindowCocoa::SetRenderCallbackOnLiveResizing()
{
    [openGLView setRenderCallback: {}];
}
//-----------------------------------------------------------------------
void GameWindowCocoa::BindToDelegate(std::shared_ptr<MouseCocoa> mouse)
{
    POMDOG_ASSERT(mouse);
    [viewDelegate resetMouse:mouse];
}
//-----------------------------------------------------------------------
}// namespace Cocoa
}// namespace Detail
}// namespace Pomdog
