//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaGameWindow.hpp"
#include <utility>
#include <Pomdog/Utility/Assert.hpp>

#include "CocoaOpenGLContext.hpp"
#include "CocoaOpenGLView.hpp"
#include "CocoaWindowDelegate.hpp"
#include "CocoaOpenGLViewDelegate.hpp"

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
	
	// Create WindowDelegate
	viewDelegate = [[CocoaOpenGLViewDelegate alloc] initWithEventDispatcher:eventDispatcher];
	[openGLView setDelegate:viewDelegate];
}
//-----------------------------------------------------------------------
CocoaGameWindow::~CocoaGameWindow()
{
	// Remove delegate from window:
	[nativeWindow setDelegate:nil];
	
	// Remove OpenGLView from window:
	[openGLView removeFromSuperview];
	
#if !__has_feature(objc_arc)
	[openGLView release];
	[nativeWindow release];
#endif
}
//-----------------------------------------------------------------------
bool CocoaGameWindow::GetAllowPlayerResizing() const
{
	NSUInteger styleMask = [nativeWindow styleMask];
	return (styleMask & NSResizableWindowMask) == NSResizableWindowMask;
}
//-----------------------------------------------------------------------
void CocoaGameWindow::SetAllowPlayerResizing(bool allowResizing)
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
std::string CocoaGameWindow::GetCaption() const
{
	std::string caption = [[nativeWindow title] UTF8String];
	return std::move(caption);
}
//-----------------------------------------------------------------------
void CocoaGameWindow::SetCaption(std::string const& caption)
{
	[nativeWindow setTitle:[NSString stringWithUTF8String:caption.c_str()]];
}
//-----------------------------------------------------------------------
Rectangle CocoaGameWindow::GetClientBounds() const
{
	POMDOG_ASSERT(openGLView != nil);
	NSRect bounds = [openGLView frame];
	Rectangle rect {
		bounds.origin.x,
		bounds.origin.y,
		bounds.size.width,
		bounds.size.height
	};
	return std::move(rect);
}
//-----------------------------------------------------------------------
void CocoaGameWindow::SetClientBounds(Rectangle const& clientBounds)
{
	NSRect bounds;
	bounds.origin.x = clientBounds.x;
	bounds.origin.y = clientBounds.y;
	bounds.size.width = clientBounds.width;
	bounds.size.height = clientBounds.height;

	[nativeWindow setFrame:bounds display:YES animate:YES];
}
//-----------------------------------------------------------------------
#pragma mark -
#pragma mark Low-Level API for CocoaGameHost
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
#pragma mark -
#pragma mark OpenGLView
//-----------------------------------------------------------------------
void CocoaGameWindow::ResetGLContext(std::shared_ptr<CocoaOpenGLContext> context)
{
	POMDOG_ASSERT(context);
	
	openGLContext = std::move(context);
	
	POMDOG_ASSERT(openGLContext);
	[openGLView setOpenGLContext:openGLContext->GetNSOpenGLContext()];
}
//-----------------------------------------------------------------------
void CocoaGameWindow::ResetGLContext()
{
	openGLContext.reset();
	[openGLView clearGLContext];
}
//-----------------------------------------------------------------------
#pragma mark -
#pragma mark Input Device
//-----------------------------------------------------------------------
void CocoaGameWindow::BindToDelegate(std::shared_ptr<CocoaMouse> mouse)
{
	POMDOG_ASSERT(mouse);
	[viewDelegate resetMouse:mouse];
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
