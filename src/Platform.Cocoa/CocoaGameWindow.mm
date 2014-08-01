//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaGameWindow.hpp"
#include "CocoaOpenGLContext.hpp"
#include "CocoaOpenGLView.hpp"
#include "CocoaWindowDelegate.hpp"
#include "CocoaGameViewDelegate.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include <utility>

namespace Pomdog {
namespace Details {
namespace Cocoa {

//-----------------------------------------------------------------------
CocoaGameWindow::CocoaGameWindow(NSWindow* window, std::shared_ptr<SystemEventDispatcher> eventDispatcher)
	: nativeWindow(window)
	, openGLView(nil)
	, windowDelegate(nil)
	, viewDelegate(nil)
{
#if !__has_feature(objc_arc)
	[this->nativeWindow retain];
#endif

	//NSRect frameRect = [this->nativeWindow frame];
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
	
	// Create WindowDelegate
	viewDelegate = [[CocoaGameViewDelegate alloc] initWithEventDispatcher:eventDispatcher];
	[viewDelegate setView:openGLView];
	[openGLView setDelegate:viewDelegate];
}
//-----------------------------------------------------------------------
CocoaGameWindow::~CocoaGameWindow()
{
	// Remove delegate from window:
	[nativeWindow setDelegate:nil];
	
	// Remove OpenGLView from window:
	[viewDelegate setView:nil];
	[openGLView setDelegate:nil];
	[openGLView removeFromSuperview];
	
#if !__has_feature(objc_arc)
	[openGLView release];
	[nativeWindow release];
#endif
}
//-----------------------------------------------------------------------
bool CocoaGameWindow::AllowPlayerResizing() const
{
	NSUInteger styleMask = [nativeWindow styleMask];
	return (styleMask & NSResizableWindowMask) == NSResizableWindowMask;
}
//-----------------------------------------------------------------------
void CocoaGameWindow::AllowPlayerResizing(bool allowResizing)
{
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
std::string CocoaGameWindow::Title() const
{
	std::string title = [[nativeWindow title] UTF8String];
	return std::move(title);
}
//-----------------------------------------------------------------------
void CocoaGameWindow::Title(std::string const& title)
{
	[nativeWindow setTitle:[NSString stringWithUTF8String:title.c_str()]];
}
//-----------------------------------------------------------------------
Rectangle CocoaGameWindow::ClientBounds() const
{
	POMDOG_ASSERT([nativeWindow contentView] != nil);
	NSRect bounds = [[nativeWindow contentView] bounds];
	NSPoint origin = [nativeWindow frame].origin;
	
	NSSize windowSize = [nativeWindow frame].size;
	NSSize screenSize = [[nativeWindow screen] visibleFrame].size;
	
	Rectangle rect {
		static_cast<std::int32_t>(origin.x),
		static_cast<std::int32_t>(screenSize.height - windowSize.height - origin.y),
		static_cast<std::int32_t>(bounds.size.width),
		static_cast<std::int32_t>(bounds.size.height)
	};
	return std::move(rect);
}
//-----------------------------------------------------------------------
void CocoaGameWindow::ClientBounds(Rectangle const& clientBounds)
{
	NSSize bounds;
	bounds.width = clientBounds.Width;
	bounds.height = clientBounds.Height;
	[nativeWindow setContentSize:bounds];

	NSSize windowSize = [nativeWindow frame].size;
	NSSize screenSize = [[nativeWindow screen] visibleFrame].size;

	NSPoint origin;
	origin.x = clientBounds.X;
	origin.y = (screenSize.height - (clientBounds.Y + windowSize.height));
	[nativeWindow setFrameOrigin:origin];

	//NSRect bounds;
	//bounds.origin.x = clientBounds.X;
	//bounds.origin.y = clientBounds.Y;
	//bounds.size.width = clientBounds.Width;
	//bounds.size.height = clientBounds.Height;
	//[nativeWindow setFrame:bounds display:YES animate:YES];
}
//-----------------------------------------------------------------------
#pragma mark - Low-Level API for CocoaGameHost
//-----------------------------------------------------------------------
bool CocoaGameWindow::IsMinimized() const
{
	return [nativeWindow isMiniaturized] == YES;
}
//-----------------------------------------------------------------------
void CocoaGameWindow::Close()
{
	// Removes the window from the screen list, which hides the window:
	//[nativeWindow orderOut:nil];
	
	[nativeWindow close];
}
//-----------------------------------------------------------------------
#pragma mark - OpenGLView
//-----------------------------------------------------------------------
void CocoaGameWindow::ResetGLContext(std::shared_ptr<CocoaOpenGLContext> context)
{
	POMDOG_ASSERT(context);
	
	openGLContext = std::move(context);
	
	POMDOG_ASSERT(openGLContext);
	[openGLView setOpenGLContext:openGLContext->NativeOpenGLContext()];
}
//-----------------------------------------------------------------------
void CocoaGameWindow::ResetGLContext()
{
	openGLContext.reset();
	[openGLView clearGLContext];
}
//-----------------------------------------------------------------------
#pragma mark - Input Device
//-----------------------------------------------------------------------
void CocoaGameWindow::BindToDelegate(std::shared_ptr<MouseCocoa> mouse)
{
	POMDOG_ASSERT(mouse);
	[viewDelegate resetMouse:mouse];
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
