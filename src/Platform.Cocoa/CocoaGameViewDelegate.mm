//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import "CocoaGameViewDelegate.hpp"

#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Event/Event.hpp>
#include "CocoaMouse.hpp"

using Pomdog::Event;
using Pomdog::ButtonState;

namespace {

static Pomdog::Point2D ToPoint2D(NSPoint const& point)
{
	//return Pomdog::Point2D(point.x, point.y);
	
	///@todo badcode
	return Pomdog::Point2D(point.x - 2, point.y - 2);
}

}// unnamed namespace

@implementation CocoaGameViewDelegate
{
	std::shared_ptr<Pomdog::Details::SystemEventDispatcher> eventDispatcher;
	std::shared_ptr<Pomdog::Details::Cocoa::CocoaMouse> mouse_;
	NSView* view_;
}

@synthesize view = view_;

//-----------------------------------------------------------------------
- (id)initWithEventDispatcher:(std::shared_ptr<Pomdog::Details::SystemEventDispatcher>)dispatcher
{
	self = [super init];
	if (self) {
		// insert code here to initialize delegate
		POMDOG_ASSERT(dispatcher);
		eventDispatcher = dispatcher;
	}
	return self;
}
//-----------------------------------------------------------------------
- (void)resetMouse:(std::shared_ptr<Pomdog::Details::Cocoa::CocoaMouse>)mouse
{
	mouse_ = mouse;
}
//-----------------------------------------------------------------------
- (void)resetMouse
{
	mouse_.reset();
}
//-----------------------------------------------------------------------
#pragma mark - Mouse Event Handling
//-----------------------------------------------------------------------
- (void)mouseEntered:(NSEvent *)theEvent
{
	if (mouse_) {
		POMDOG_ASSERT(view_ != nil);
		NSPoint locationInView = [view_ convertPoint:[theEvent locationInWindow] fromView:nil];
		mouse_->Position(ToPoint2D(locationInView));
	}
}
//-----------------------------------------------------------------------
-(void)mouseMoved:(NSEvent *)theEvent
{
	if (mouse_) {
		POMDOG_ASSERT(view_ != nil);
		NSPoint locationInView = [view_ convertPoint:[theEvent locationInWindow] fromView:nil];
		mouse_->Position(ToPoint2D(locationInView));
	}
}
//-----------------------------------------------------------------------
- (void)mouseExited:(NSEvent *)theEvent
{
	if (mouse_) {
		POMDOG_ASSERT(view_ != nil);
		NSPoint locationInView = [view_ convertPoint:[theEvent locationInWindow] fromView:nil];
		mouse_->Position(ToPoint2D(locationInView));
	}
}
//-----------------------------------------------------------------------
-(void)mouseDown:(NSEvent *)theEvent
{
	if (mouse_) {
		mouse_->LeftButton(ButtonState::Pressed);
	
		POMDOG_ASSERT(view_ != nil);
		NSPoint locationInView = [view_ convertPoint:[theEvent locationInWindow] fromView:nil];
		mouse_->Position(ToPoint2D(locationInView));
	}
}
//-----------------------------------------------------------------------
-(void)mouseDragged:(NSEvent *)theEvent
{
	if (mouse_) {
		mouse_->LeftButton(ButtonState::Pressed);
		
		POMDOG_ASSERT(view_ != nil);
		NSPoint locationInView = [view_ convertPoint:[theEvent locationInWindow] fromView:nil];
		mouse_->Position(ToPoint2D(locationInView));
	}
}
//-----------------------------------------------------------------------
-(void)mouseUp:(NSEvent *)theEvent
{
	if (mouse_) {
		mouse_->LeftButton(ButtonState::Released);
		
		POMDOG_ASSERT(view_ != nil);
		NSPoint locationInView = [view_ convertPoint:[theEvent locationInWindow] fromView:nil];
		mouse_->Position(ToPoint2D(locationInView));
	}
}
//-----------------------------------------------------------------------
-(void)rightMouseDown:(NSEvent *)theEvent
{
	if (mouse_) {
		mouse_->RightButton(ButtonState::Pressed);
		
		POMDOG_ASSERT(view_ != nil);
		NSPoint locationInView = [view_ convertPoint:[theEvent locationInWindow] fromView:nil];
		mouse_->Position(ToPoint2D(locationInView));
	}
}
//-----------------------------------------------------------------------
-(void)rightMouseDragged:(NSEvent *)theEvent
{
	if (mouse_) {
		mouse_->RightButton(ButtonState::Pressed);
		
		POMDOG_ASSERT(view_ != nil);
		NSPoint locationInView = [view_ convertPoint:[theEvent locationInWindow] fromView:nil];
		mouse_->Position(ToPoint2D(locationInView));
	}
}
//-----------------------------------------------------------------------
-(void)rightMouseUp:(NSEvent *)theEvent
{
	if (mouse_) {
		mouse_->RightButton(ButtonState::Released);
		
		POMDOG_ASSERT(view_ != nil);
		NSPoint locationInView = [view_ convertPoint:[theEvent locationInWindow] fromView:nil];
		mouse_->Position(ToPoint2D(locationInView));
	}
}
//-----------------------------------------------------------------------
-(void)otherMouseDown:(NSEvent *)theEvent
{
	if (!mouse_) {
		return;
	}
	
	NSInteger buttonNumber = [theEvent buttonNumber];
	
	if (buttonNumber == 2) {
		mouse_->MiddleButton(ButtonState::Pressed);
	}
	else if (buttonNumber == 3) {
		mouse_->XButton1(ButtonState::Pressed);
	}
	else if (buttonNumber == 4) {
		mouse_->XButton2(ButtonState::Pressed);
	}
	
	POMDOG_ASSERT(view_ != nil);
	NSPoint locationInView = [view_ convertPoint:[theEvent locationInWindow] fromView:nil];
	mouse_->Position(ToPoint2D(locationInView));
}
//-----------------------------------------------------------------------
-(void)otherMouseDragged:(NSEvent *)theEvent
{
	if (!mouse_) {
		return;
	}
	
	NSInteger buttonNumber = [theEvent buttonNumber];
	
	if (buttonNumber == 2) {
		mouse_->MiddleButton(ButtonState::Pressed);
	}
	else if (buttonNumber == 3) {
		mouse_->XButton1(ButtonState::Pressed);
	}
	else if (buttonNumber == 4) {
		mouse_->XButton2(ButtonState::Pressed);
	}
	
	POMDOG_ASSERT(view_ != nil);
	NSPoint locationInView = [view_ convertPoint:[theEvent locationInWindow] fromView:nil];
	mouse_->Position(ToPoint2D(locationInView));
}
//-----------------------------------------------------------------------
-(void)otherMouseUp:(NSEvent *)theEvent
{
	if (!mouse_) {
		return;
	}
	
	NSInteger buttonNumber = [theEvent buttonNumber];
	
	if (buttonNumber == 2) {
		mouse_->MiddleButton(ButtonState::Released);
	}
	else if (buttonNumber == 3) {
		mouse_->XButton1(ButtonState::Released);
	}
	else if (buttonNumber == 4) {
		mouse_->XButton2(ButtonState::Released);
	}
	
	POMDOG_ASSERT(view_ != nil);
	NSPoint locationInView = [view_ convertPoint:[theEvent locationInWindow] fromView:nil];
	mouse_->Position(ToPoint2D(locationInView));
}
//-----------------------------------------------------------------------
-(void)scrollWheel:(NSEvent *)theEvent
{
	double scrollingDeltaY = static_cast<double>([theEvent scrollingDeltaY]);
	if ([theEvent hasPreciseScrollingDeltas])
	{
		//NSLog(@"hasPreciseScrollingDeltas=YES,scrollingDeltaY=%f", scrollingDeltaY);
		mouse_->WheelDelta(scrollingDeltaY * 0.1);
	}
	else
	{
		//NSLog(@"hasPreciseScrollingDeltas=NO, scrollingDeltaY=%f", scrollingDeltaY);
		mouse_->WheelDelta(scrollingDeltaY);
	}
}
//-----------------------------------------------------------------------
#pragma mark - View Event Handling
//-----------------------------------------------------------------------
- (void)viewNeedsUpdateSurface
{
	using Pomdog::Details::ViewNeedsUpdateSurfaceEvent;
	eventDispatcher->Enqueue<ViewNeedsUpdateSurfaceEvent>();
}
//-----------------------------------------------------------------------
- (void)viewWillStartLiveResize
{
	using Pomdog::Details::ViewWillStartLiveResizeEvent;
	eventDispatcher->Enqueue<ViewWillStartLiveResizeEvent>();
}
//-----------------------------------------------------------------------
- (void)viewDidEndLiveResize
{
	using Pomdog::Details::ViewDidEndLiveResizeEvent;
	eventDispatcher->Enqueue<ViewDidEndLiveResizeEvent>();
}
//-----------------------------------------------------------------------
@end
