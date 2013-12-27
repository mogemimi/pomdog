//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaOpenGLContext.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include "../RenderSystem.GL4/OpenGLPrerequisites.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {
//-----------------------------------------------------------------------
CocoaOpenGLContext::CocoaOpenGLContext(NSOpenGLPixelFormat* pixelFormat_)
	: openGLContext(nil)
	, pixelFormat(pixelFormat_)
{
	POMDOG_ASSERT(this->pixelFormat != nil);
	
#if !__has_feature(objc_arc)
	[this->pixelFormat retain];
#endif

	openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext: nil];
			
	[openGLContext makeCurrentContext];
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
NSOpenGLContext* CocoaOpenGLContext::GetNSOpenGLContext()
{
	return openGLContext;
}
//-----------------------------------------------------------------------
}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
