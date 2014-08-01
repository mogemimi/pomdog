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
		CGLContextObj coreOpenGLContext = static_cast<CGLContextObj>([openGLContext CGLContextObj]);
		constexpr GLint swapInterval = 1;
		CGLSetParameter(coreOpenGLContext, kCGLCPSwapInterval, &swapInterval);
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
void CocoaOpenGLContext::BindCurrentContext()
{
	POMDOG_ASSERT(openGLContext != nil);
	[openGLContext makeCurrentContext];
}
//-----------------------------------------------------------------------
void CocoaOpenGLContext::UnbindCurrentContext()
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
	CGLContextObj coreOpenGLContext = static_cast<CGLContextObj>([openGLContext CGLContextObj]);
	CGLLockContext(coreOpenGLContext);
}
//-----------------------------------------------------------------------
void CocoaOpenGLContext::UnlockContext()
{
	POMDOG_ASSERT(openGLContext != nil);
	CGLContextObj coreOpenGLContext = static_cast<CGLContextObj>([openGLContext CGLContextObj]);
	CGLUnlockContext(coreOpenGLContext);
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
