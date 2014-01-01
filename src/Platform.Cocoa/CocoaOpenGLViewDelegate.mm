//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import "CocoaOpenGLViewDelegate.hpp"

#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Event/Event.hpp>
#include <Pomdog/Event/EventCodeHelper.hpp>
#include "CocoaMouse.hpp"

using Pomdog::Event;
using Pomdog::EventCodeHelper;
using Pomdog::ButtonState;

@implementation CocoaOpenGLViewDelegate
{
	std::shared_ptr<Pomdog::Details::SystemEventDispatcher> eventDispatcher;
	std::shared_ptr<Pomdog::Details::Cocoa::CocoaMouse> mouse_;
}
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
#pragma mark -
#pragma mark Mouse Event Handling
//-----------------------------------------------------------------------
- (void)mouseEntered:(NSEvent *)theEvent
{
	if (mouse_) {
		NSPoint locationInWindow = [theEvent locationInWindow];
		mouse_->SetPosition({locationInWindow.x, locationInWindow.y});
	}
}
//-----------------------------------------------------------------------
-(void)mouseMoved:(NSEvent *)theEvent
{
	if (mouse_) {
		NSPoint locationInWindow = [theEvent locationInWindow];
		mouse_->SetPosition({locationInWindow.x, locationInWindow.y});
	}
}
//-----------------------------------------------------------------------
- (void)mouseExited:(NSEvent *)theEvent
{
	if (mouse_) {
		NSPoint locationInWindow = [theEvent locationInWindow];
		mouse_->SetPosition({locationInWindow.x, locationInWindow.y});
	}
}
//-----------------------------------------------------------------------
-(void)mouseDown:(NSEvent *)theEvent
{
	if (mouse_) {
		mouse_->SetLeftButton(ButtonState::Pressed);
	}
}
//-----------------------------------------------------------------------
-(void)mouseUp:(NSEvent *)theEvent
{
	if (mouse_) {
		mouse_->SetLeftButton(ButtonState::Released);
	}
}
//-----------------------------------------------------------------------
-(void)rightMouseDown:(NSEvent *)theEvent
{
	if (mouse_) {
		mouse_->SetRightButton(ButtonState::Pressed);
	}
}
//-----------------------------------------------------------------------
-(void)rightMouseUp:(NSEvent *)theEvent
{
	if (mouse_) {
		mouse_->SetRightButton(ButtonState::Released);
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
		mouse_->SetMiddleButton(ButtonState::Pressed);
	}
	else if (buttonNumber == 3) {
		mouse_->SetXButton1(ButtonState::Pressed);
	}
	else if (buttonNumber == 4) {
		mouse_->SetXButton2(ButtonState::Pressed);
	}
}
//-----------------------------------------------------------------------
-(void)otherMouseUp:(NSEvent *)theEvent
{
	if (!mouse_) {
		return;
	}
	
	NSInteger buttonNumber = [theEvent buttonNumber];
	
	if (buttonNumber == 2) {
		mouse_->SetMiddleButton(ButtonState::Released);
	}
	else if (buttonNumber == 3) {
		mouse_->SetXButton1(ButtonState::Released);
	}
	else if (buttonNumber == 4) {
		mouse_->SetXButton2(ButtonState::Released);
	}
}
//-----------------------------------------------------------------------
-(void)scrollWheel:(NSEvent *)theEvent
{
	CGFloat scrollWheel = [theEvent scrollingDeltaY];
	mouse_->SetScrollWheel(scrollWheel);
}
//-----------------------------------------------------------------------
@end
