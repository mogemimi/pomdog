//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import "CocoaOpenGLView.hpp"
#include <OpenGL/gl.h>
#include <Pomdog/Utility/Assert.hpp>

@implementation CocoaOpenGLView

@synthesize openGLContext = openGLContext_;

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
}

@end
