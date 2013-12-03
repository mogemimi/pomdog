//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import "CocoaOpenGLView.hpp"
#include <OpenGL/gl.h>
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Utility/Assert.hpp>

@implementation CocoaOpenGLView
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
- (void)awakeFromNib
{
	[super awakeFromNib];
	
	[self prepareOpenGL];
}
//-----------------------------------------------------------------------
+ (NSOpenGLPixelFormat*)defaultPixelFormat
{
	NSOpenGLPixelFormatAttribute attribute[] = {
		NSOpenGLPFAWindow,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFAStencilSize, 8,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFANoRecovery,
		NSOpenGLPFAAccelerated,
		0
	};
	return [[NSOpenGLPixelFormat alloc] initWithAttributes:attribute];
}
//-----------------------------------------------------------------------
- (void)prepareOpenGL
{
	if (contextOSX && contextOSX->GetNSOpenGLContext()) {
		return;
	}
	
	NSOpenGLPixelFormat* pixelFormat = [CocoaOpenGLView defaultPixelFormat];

	contextOSX.reset(new Pomdog::Details::Cocoa::CocoaOpenGLContext(pixelFormat));
	graphicsContext.reset(new Pomdog::Details::GL4::GraphicsContextGL4(contextOSX));
}
//-----------------------------------------------------------------------
- (void)clearGLContext
{
	contextOSX.reset();
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
	if (!contextOSX) {
		return;
	}

	NSOpenGLContext* context = contextOSX->GetNSOpenGLContext();
	
	[super lockFocus];
	if ([context view] != self) {
		[context setView:self];
	}
	[context makeCurrentContext];
}
//-----------------------------------------------------------------------
-(void)viewDidMoveToWindow
{
	if (!contextOSX) {
		return;
	}

	NSOpenGLContext* context = contextOSX->GetNSOpenGLContext();

	[super viewDidMoveToWindow];
	if ([self window] == nil) {
		[context clearDrawable];
	}
}
//-----------------------------------------------------------------------
- (void)drawRect:(NSRect)dirtyRect
{
	NSOpenGLContext* openGLContext = contextOSX->GetNSOpenGLContext();
	POMDOG_ASSERT(openGLContext);
	
	CGLLockContext((CGLContextObj)[openGLContext CGLContextObj]);

	contextOSX->BindCurrentContext();
	
	auto color = Pomdog::Color::CornflowerBlue;
	graphicsContext->Clear(color);
	
	graphicsContext->Present();

	CGLUnlockContext((CGLContextObj)[openGLContext CGLContextObj]);
}

@end
