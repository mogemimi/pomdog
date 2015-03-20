//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "OpenGLContextCocoa.hpp"
#include "../RenderSystem.GL4/OpenGLPrerequisites.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Cocoa {
//-----------------------------------------------------------------------
OpenGLContextCocoa::OpenGLContextCocoa(NSOpenGLPixelFormat* pixelFormat)
	: openGLContext(nil)
{
	POMDOG_ASSERT(pixelFormat != nil);

	openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext: nil];
	[openGLContext makeCurrentContext];

	constexpr GLint swapInterval = 1;
	[openGLContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
}
//-----------------------------------------------------------------------
OpenGLContextCocoa::~OpenGLContextCocoa()
{
#if !__has_feature(objc_arc)
	[openGLContext release];
#endif
	openGLContext = nil;
}
//-----------------------------------------------------------------------
void OpenGLContextCocoa::MakeCurrent()
{
	POMDOG_ASSERT(openGLContext != nil);
	[openGLContext makeCurrentContext];
}
//-----------------------------------------------------------------------
void OpenGLContextCocoa::ClearCurrent()
{
	POMDOG_ASSERT(openGLContext != nil);
	[NSOpenGLContext clearCurrentContext];
}
//-----------------------------------------------------------------------
void OpenGLContextCocoa::SwapBuffers()
{
	POMDOG_ASSERT(openGLContext != nil);
	[openGLContext flushBuffer];
}
//-----------------------------------------------------------------------
void OpenGLContextCocoa::Lock()
{
	POMDOG_ASSERT(openGLContext != nil);
	CGLLockContext([openGLContext CGLContextObj]);
}
//-----------------------------------------------------------------------
void OpenGLContextCocoa::Unlock()
{
	POMDOG_ASSERT(openGLContext != nil);
	CGLUnlockContext([openGLContext CGLContextObj]);
}
//-----------------------------------------------------------------------
NSOpenGLContext* OpenGLContextCocoa::NativeOpenGLContext()
{
	return openGLContext;
}
//-----------------------------------------------------------------------
}// namespace Cocoa
}// namespace Detail
}// namespace Pomdog
