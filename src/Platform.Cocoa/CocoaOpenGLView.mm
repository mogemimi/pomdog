//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#import "CocoaOpenGLView.hpp"
#import "CocoaGameViewDelegate.hpp"
#include "../RenderSystem.GL4/OpenGLPrerequisites.hpp"
#include "Pomdog/Utility/Assert.hpp"

@implementation CocoaOpenGLView {
@private
	std::function<void()> renderCallback_;
	NSOpenGLContext* openGLContext_;
	CocoaGameViewDelegate* delegate_;
	NSTrackingRectTag trackingRect;
	BOOL wasAcceptingMouseEvents;
	bool viewLiveResizing;
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
		wasAcceptingMouseEvents = NO;
	}
	return self;
}
//-----------------------------------------------------------------------
- (BOOL)isFlipped
{
	return TRUE;
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
	if ([openGLContext_ view] == self) {
		//[openGLContext_ update];
	}
}
//-----------------------------------------------------------------------
- (void)reshape
{
	///@todo Not implemented
}
//-----------------------------------------------------------------------
- (void)lockFocus
{
	[super lockFocus];

	if (openGLContext_ == nil) {
		return;
	}
	
	if ([openGLContext_ view] != self) {
		[openGLContext_ setView:self];
	}
	[openGLContext_ makeCurrentContext];
}
//-----------------------------------------------------------------------
- (void)drawRect:(NSRect)dirtyRect
{
	[super drawRect:dirtyRect];

	if (viewLiveResizing && renderCallback_) {
		renderCallback_();
	}
}
//-----------------------------------------------------------------------
- (void)setRenderCallback:(std::function<void()>)callback
{
	renderCallback_ = callback;
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
#pragma mark - Mouse-Tracking and Cursor
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
#pragma mark - Mouse Event Delegated
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
- (void)mouseDown:(NSEvent *)theEvent
{
	[[self delegate] mouseDown:theEvent];
}
//-----------------------------------------------------------------------
- (void)mouseDragged:(NSEvent *)theEvent
{
	[[self delegate] mouseDragged:theEvent];
}
//-----------------------------------------------------------------------
- (void)mouseUp:(NSEvent *)theEvent
{
	[[self delegate] mouseUp:theEvent];
}
//-----------------------------------------------------------------------
- (void)rightMouseDown:(NSEvent *)theEvent
{
	[[self delegate] rightMouseDown:theEvent];
}
//-----------------------------------------------------------------------
- (void)rightMouseDragged:(NSEvent *)theEvent
{
	[[self delegate] rightMouseDragged:theEvent];
}
//-----------------------------------------------------------------------
- (void)rightMouseUp:(NSEvent *)theEvent
{
	[[self delegate] rightMouseUp:theEvent];
}
//-----------------------------------------------------------------------
- (void)otherMouseDown:(NSEvent *)theEvent
{
	[[self delegate] otherMouseDown:theEvent];
}
//-----------------------------------------------------------------------
- (void)otherMouseDragged:(NSEvent *)theEvent
{
	[[self delegate] otherMouseDragged:theEvent];
}
//-----------------------------------------------------------------------
- (void)otherMouseUp:(NSEvent *)theEvent
{
	[[self delegate] otherMouseUp:theEvent];
}
//-----------------------------------------------------------------------
- (void)scrollWheel:(NSEvent *)theEvent
{
	[[self delegate] scrollWheel:theEvent];
}
//-----------------------------------------------------------------------
#pragma mark - Keyboard Event Delegated
//-----------------------------------------------------------------------
- (void)keyDown:(NSEvent *)theEvent
{
	[[self delegate] keyDown:theEvent];
}
//-----------------------------------------------------------------------
- (void)keyUp:(NSEvent *)theEvent
{
	[[self delegate] keyUp:theEvent];
}
//-----------------------------------------------------------------------
#pragma mark - View Event Delegated
//-----------------------------------------------------------------------
- (void)viewWillStartLiveResize
{
	viewLiveResizing = true;
	[[self delegate] viewWillStartLiveResize];
}
//-----------------------------------------------------------------------
- (void)viewDidEndLiveResize
{
	viewLiveResizing = false;
	[[self delegate] viewDidEndLiveResize];
}
//-----------------------------------------------------------------------
@end
