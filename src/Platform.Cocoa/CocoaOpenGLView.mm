//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import "CocoaOpenGLView.hpp"
#include "../RenderSystem.GL4/OpenGLPrerequisites.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Math/Rectangle.hpp>

@implementation CocoaOpenGLView
{
	NSTrackingRectTag trackingRect;
	BOOL wasAcceptingMouseEvents;
}

@synthesize openGLContext = openGLContext_;
@synthesize delegate = delegate_;

//-----------------------------------------------------------------------
- (id)initWithFrame:(NSRect)frameRect
{
	self = [super initWithFrame:frameRect];
	if (self) {
		[[NSNotificationCenter defaultCenter] addObserver:self
			selector:@selector(_surfaceNeedsUpdate:)
			name:NSViewGlobalFrameDidChangeNotification
			object:self];
	}
	wasAcceptingMouseEvents = NO;
	return self;
}
//-----------------------------------------------------------------------
- (void)_surfaceNeedsUpdate:(NSNotification*)notification
{
	[self update];
}
//-----------------------------------------------------------------------
- (void)clearGLContext
{
	if (openGLContext_ != nil) {
		[self setOpenGLContext:nil];
	}
}
//-----------------------------------------------------------------------
- (void)update
{
}
//-----------------------------------------------------------------------
- (void)reshape
{
	///@todo Not implemented
	//NSRect bounds = [self bounds];
	//Pomdog::Rectangle rect{bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height};
	// send event
	//[delegate reshape:bounds];
}
//-----------------------------------------------------------------------
- (void)lockFocus
{
	[super lockFocus];

	NSOpenGLContext* context = [self openGLContext];
	if (!context) {
		return;
	}
	
	if ([context view] != self) {
		[context setView:self];
	}
	[context makeCurrentContext];
}
//-----------------------------------------------------------------------
-(void)viewDidMoveToWindow
{
	[super viewDidMoveToWindow];

	if ([self window] == nil) {
		[[self openGLContext] clearDrawable];
	}
	
	trackingRect = [self addTrackingRect:[self bounds] owner:self userData:NULL assumeInside:NO];
}
//-----------------------------------------------------------------------
#pragma mark -
#pragma mark Mouse-Tracking and Cursor
//-----------------------------------------------------------------------
- (void)setFrame:(NSRect)frame
{
	[super setFrame:frame];
	[self removeTrackingRect:trackingRect];
	trackingRect = [self addTrackingRect:[self bounds] owner:self userData:NULL assumeInside:NO];
}
//-----------------------------------------------------------------------
- (void)setBounds:(NSRect)bounds
{
	[super setBounds:bounds];
	[self removeTrackingRect:trackingRect];
	trackingRect = [self addTrackingRect:[self bounds] owner:self userData:NULL assumeInside:NO];
}
//-----------------------------------------------------------------------
- (void)viewWillMoveToWindow:(NSWindow *)newWindow
{
	if ([self window] && trackingRect) {
		[self removeTrackingRect:trackingRect];
	}
}
//-----------------------------------------------------------------------
#pragma mark -
#pragma mark Mouse Event Delegated
//-----------------------------------------------------------------------
- (void)mouseEntered:(NSEvent *)theEvent
{
	if (![self delegate]) {
		return;
	}

	wasAcceptingMouseEvents = [[self window] acceptsMouseMovedEvents];
	[[self window] setAcceptsMouseMovedEvents:YES];
	[[self window] makeFirstResponder:self];
	
	[[self delegate] mouseEntered:theEvent];
}
//-----------------------------------------------------------------------
-(void)mouseMoved:(NSEvent *)theEvent
{
	[[self delegate] mouseMoved:theEvent];
}
//-----------------------------------------------------------------------
- (void)mouseExited:(NSEvent *)theEvent
{
	[[self window] setAcceptsMouseMovedEvents:wasAcceptingMouseEvents];
	
	[[self delegate] mouseExited:theEvent];
}
//-----------------------------------------------------------------------
-(void)mouseDown:(NSEvent *)theEvent
{
	[[self delegate] mouseDown:theEvent];
}
//-----------------------------------------------------------------------
-(void)mouseUp:(NSEvent *)theEvent
{
	[[self delegate] mouseUp:theEvent];
}
//-----------------------------------------------------------------------
-(void)rightMouseDown:(NSEvent *)theEvent
{
	[[self delegate] rightMouseDown:theEvent];
}
//-----------------------------------------------------------------------
-(void)rightMouseUp:(NSEvent *)theEvent
{
	[[self delegate] rightMouseUp:theEvent];
}
//-----------------------------------------------------------------------
-(void)otherMouseDown:(NSEvent *)theEvent
{
	[[self delegate] otherMouseDown:theEvent];
}
//-----------------------------------------------------------------------
-(void)otherMouseUp:(NSEvent *)theEvent
{
	[[self delegate] otherMouseUp:theEvent];
}
//-----------------------------------------------------------------------
-(void)scrollWheel:(NSEvent *)theEvent
{
	[[self delegate] scrollWheel:theEvent];
}
//-----------------------------------------------------------------------
@end
