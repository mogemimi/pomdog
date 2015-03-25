// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_OPENGLCONTEXTCOCOA_306ECA78_18CF_4A87_A039_0C823668622C_HPP
#define POMDOG_OPENGLCONTEXTCOCOA_306ECA78_18CF_4A87_A039_0C823668622C_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "../RenderSystem.GL4/OpenGLContext.hpp"
#import <Cocoa/Cocoa.h>

@class NSOpenGLContext, NSOpenGLPixelFormat;

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class OpenGLContextCocoa final: public RenderSystem::GL4::OpenGLContext {
public:
	OpenGLContextCocoa() = delete;

	explicit OpenGLContextCocoa(NSOpenGLPixelFormat* pixelFormat);

	~OpenGLContextCocoa();

	void MakeCurrent() override;

	void ClearCurrent() override;

	void SwapBuffers() override;

	void Lock();

	void Unlock();

	NSOpenGLContext* NativeOpenGLContext();

private:
	NSOpenGLContext* openGLContext;
};

}// namespace Cocoa
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_OPENGLCONTEXTCOCOA_306ECA78_18CF_4A87_A039_0C823668622C_HPP)
