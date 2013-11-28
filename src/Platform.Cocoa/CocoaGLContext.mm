//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaGLContext.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include <OpenGL/gl.h>

namespace Pomdog {
namespace Details {
namespace Cocoa {

//-----------------------------------------------------------------------
CocoaGLContext::CocoaGLContext(NSOpenGLPixelFormat* pixelFormat)
	: openGLContext(nil)
	, pixelFormat(pixelFormat)
{
	POMDOG_ASSERT(pixelFormat != nil);
	
#if !__has_feature(objc_arc)
	[this->pixelFormat retain];
#endif

	openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext: nil];
	[openGLContext makeCurrentContext];
}
//-----------------------------------------------------------------------
CocoaGLContext::~CocoaGLContext()
{
#if !__has_feature(objc_arc)
	[openGLContext release];
	[this->pixelFormat release];
#endif
}
//-----------------------------------------------------------------------
void CocoaGLContext::BindCurrentContext()
{
	POMDOG_ASSERT(openGLContext != nil);
	[openGLContext makeCurrentContext];
}
//-----------------------------------------------------------------------
void CocoaGLContext::UnbindCurrentContext()
{
	POMDOG_ASSERT(openGLContext != nil);
	[NSOpenGLContext clearCurrentContext];
}
//-----------------------------------------------------------------------
void CocoaGLContext::SwapBuffers()
{
	POMDOG_ASSERT(openGLContext != nil);
	[openGLContext flushBuffer];
}
//-----------------------------------------------------------------------
NSOpenGLContext* CocoaGLContext::GetNSOpenGLContext()
{
	return openGLContext;
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
