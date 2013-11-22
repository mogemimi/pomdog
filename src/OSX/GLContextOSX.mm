//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GLContextOSX.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include <OpenGL/gl.h>

namespace Pomdog {
namespace Details {
namespace OSX {

//-----------------------------------------------------------------------
GLContextOSX::GLContextOSX(NSOpenGLPixelFormat* pixelFormat)
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
GLContextOSX::~GLContextOSX()
{
#if !__has_feature(objc_arc)
	[openGLContext release];
	[this->pixelFormat release];
#endif
}
//-----------------------------------------------------------------------
void GLContextOSX::BindCurrentContext()
{
	POMDOG_ASSERT(openGLContext != nil);
	[openGLContext makeCurrentContext];
}
//-----------------------------------------------------------------------
void GLContextOSX::UnbindCurrentContext()
{
	POMDOG_ASSERT(openGLContext != nil);
	[NSOpenGLContext clearCurrentContext];
}
//-----------------------------------------------------------------------
void GLContextOSX::SwapBuffers()
{
	POMDOG_ASSERT(openGLContext != nil);
	[openGLContext flushBuffer];
}
//-----------------------------------------------------------------------
NSOpenGLContext* GLContextOSX::GetNSOpenGLContext()
{
	return openGLContext;
}

}// namespace OSX
}// namespace Details
}// namespace Pomdog
