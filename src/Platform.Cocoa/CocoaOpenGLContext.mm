//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaOpenGLContext.hpp"
#include "../RenderSystem.GL4/OpenGLPrerequisites.hpp"
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {
namespace Cocoa {
//-----------------------------------------------------------------------
CocoaOpenGLContext::CocoaOpenGLContext(NSOpenGLPixelFormat* pixelFormatIn)
	: openGLContext(nil)
	, pixelFormat(pixelFormatIn)
{
	POMDOG_ASSERT(pixelFormat != nil);
	
#if !__has_feature(objc_arc)
	[this->pixelFormat retain];
#endif

	openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext: nil];
	[openGLContext makeCurrentContext];

	{
		CGLContextObj cglContext = [openGLContext CGLContextObj];
		constexpr GLint swapInterval = 1;
		CGLSetParameter(cglContext, kCGLCPSwapInterval, &swapInterval);
	}
	{
		constexpr GLint swapInterval = 1;
		[openGLContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
	}
}
//-----------------------------------------------------------------------
CocoaOpenGLContext::~CocoaOpenGLContext()
{
#if !__has_feature(objc_arc)
	[openGLContext release];
	[this->pixelFormat release];
#endif
	this->pixelFormat = nil;
}
//-----------------------------------------------------------------------
void CocoaOpenGLContext::MakeCurrentContext()
{
	POMDOG_ASSERT(openGLContext != nil);
	[openGLContext makeCurrentContext];
}
//-----------------------------------------------------------------------
void CocoaOpenGLContext::ClearCurrentContext()
{
	POMDOG_ASSERT(openGLContext != nil);
	[NSOpenGLContext clearCurrentContext];
}
//-----------------------------------------------------------------------
void CocoaOpenGLContext::SwapBuffers()
{
	POMDOG_ASSERT(openGLContext != nil);
	[openGLContext flushBuffer];
}
//-----------------------------------------------------------------------
void CocoaOpenGLContext::LockContext()
{
	POMDOG_ASSERT(openGLContext != nil);
	CGLLockContext([openGLContext CGLContextObj]);
}
//-----------------------------------------------------------------------
void CocoaOpenGLContext::UnlockContext()
{
	POMDOG_ASSERT(openGLContext != nil);
	CGLUnlockContext([openGLContext CGLContextObj]);
}
//-----------------------------------------------------------------------
NSOpenGLContext* CocoaOpenGLContext::NativeOpenGLContext()
{
	return openGLContext;
}
//-----------------------------------------------------------------------
}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
