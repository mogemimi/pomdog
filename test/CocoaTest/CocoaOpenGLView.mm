//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import "CocoaOpenGLView.h"
#include <OpenGL/gl.h>
#include <Pomdog/Math/Color.hpp>

@implementation CocoaOpenGLView

@synthesize openGLContext = openGLContext_;
@synthesize pixelFormat = pixelFormat_;

- (id)initWithFrame:(NSRect)frame
{
	self = [super initWithFrame:frame];
	if (self) {
		[[NSNotificationCenter defaultCenter] addObserver:self
			selector:@selector(_surfaceNeedsUpdate:)
			name:NSViewGlobalFrameDidChangeNotification
			object:self];
	}
	return self;
}

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format
{
	self = [super initWithFrame:frameRect];
	if (self) {
		pixelFormat_ = format;
		[[NSNotificationCenter defaultCenter] addObserver:self
			selector:@selector(_surfaceNeedsUpdate:)
			name:NSViewGlobalFrameDidChangeNotification
			object:self];
	}
	return self;
}

- (void) _surfaceNeedsUpdate:(NSNotification*)notification
{
	[self update];
}

- (void) awakeFromNib
{
	[super awakeFromNib];
	
	[self prepareOpenGL];
}

+ (NSOpenGLPixelFormat*)defaultPixelFormat
{
	NSOpenGLPixelFormatAttribute attribute[] = {
		NSOpenGLPFAWindow,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFANoRecovery,
		NSOpenGLPFAAccelerated,
		0
	};
	return [[NSOpenGLPixelFormat alloc] initWithAttributes:attribute];
}

- (void)prepareOpenGL
{
	if (openGLContext_) {
		return;
	}
	
	if ([self pixelFormat] == nil) {
		[self setPixelFormat:[CocoaOpenGLView defaultPixelFormat]];
	}
	
	[self setOpenGLContext:[[NSOpenGLContext alloc] initWithFormat:pixelFormat_ shareContext: nil]];
	[[self openGLContext] makeCurrentContext];
}

- (void)update {
}

- (void)reshape {
}

- (void)lockFocus
{
	NSOpenGLContext* context = [self openGLContext];
	
	[super lockFocus];
	if ([context view] != self) {
		[context setView:self];
	}
	[context makeCurrentContext];
}

-(void) viewDidMoveToWindow
{
	NSOpenGLContext* context = [self openGLContext];

	[super viewDidMoveToWindow];
	if ([self window] == nil) {
		[context clearDrawable];
	}
}

- (void)clearGLContext {
	if (openGLContext_ != nil) {
		[self setOpenGLContext:nil];
	}
}

- (void)drawRect:(NSRect)dirtyRect
{
	NSOpenGLContext* context = [self openGLContext];

	[context makeCurrentContext];
	
	auto color = Pomdog::Color::CornflowerBlue;
	glClearColor(color.r, color.g, color.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glFlush();

	[context flushBuffer];
}

@end
