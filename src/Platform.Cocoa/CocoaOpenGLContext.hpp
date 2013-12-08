//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_COCOA_OPENGLCONTEXT_HPP
#define POMDOG_SRC_COCOA_OPENGLCONTEXT_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../RenderSystem.GL4/OpenGLContext.hpp"
#import <Cocoa/Cocoa.h>

@class NSOpenGLContext, NSOpenGLPixelFormat;

namespace Pomdog {
namespace Details {
namespace Cocoa {

class CocoaOpenGLContext final: public RenderSystem::GL4::OpenGLContext
{
public:
	CocoaOpenGLContext() = delete;

	explicit CocoaOpenGLContext(NSOpenGLPixelFormat* pixelFormat);
	
	~CocoaOpenGLContext();

	void BindCurrentContext() override;

	void UnbindCurrentContext() override;

	void SwapBuffers() override;
	
	void LockContext();
	
	void UnlockContext();
	
	NSOpenGLContext* GetNSOpenGLContext();
	
private:
	NSOpenGLContext* openGLContext;
	NSOpenGLPixelFormat* pixelFormat;
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_COCOA_OPENGLCONTEXT_HPP)
