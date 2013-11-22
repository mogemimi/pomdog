//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_OSX_GLCONTEXTOSX_HPP
#define POMDOG_OSX_GLCONTEXTOSX_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../GL4/GLContext.hpp"
#import <Cocoa/Cocoa.h>

@class NSOpenGLContext, NSOpenGLPixelFormat;

namespace Pomdog {
namespace Details {
namespace OSX {

class GLContextOSX final: public GL4::GLContext
{
public:
	GLContextOSX() = delete;

	explicit GLContextOSX(NSOpenGLPixelFormat* pixelFormat);
	~GLContextOSX();

	void BindCurrentContext();

	void UnbindCurrentContext();

	void SwapBuffers();
	
	NSOpenGLContext* GetNSOpenGLContext();
	
private:
	NSOpenGLContext* openGLContext;
	NSOpenGLPixelFormat* pixelFormat;
};

}// namespace OSX
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_OSX_GLCONTEXTOSX_HPP)
