//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_COCOA_OPENGLVIEW_330827E8_5CE1_11E3_978C_A8206655A22B_HPP
#define POMDOG_SRC_COCOA_OPENGLVIEW_330827E8_5CE1_11E3_978C_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#import <Cocoa/Cocoa.h>
#include <memory>
#include "../../src/Platform.Cocoa/CocoaOpenGLContext.hpp"
#include "../../src/RenderSystem.GL4/GraphicsContextGL4.hpp"

@class NSOpenGLContext, NSOpenGLPixelFormat;

@interface CocoaOpenGLView : NSView
{
@private
	std::shared_ptr<Pomdog::Details::Cocoa::CocoaOpenGLContext> contextOSX;
	std::unique_ptr<Pomdog::Details::GL4::GraphicsContextGL4> graphicsContext;
}

- (id)initWithFrame:(NSRect)frameRect;

- (void)prepareOpenGL;

- (void)update;
- (void)reshape;

@end

#endif // !defined(POMDOG_SRC_COCOA_OPENGLVIEW_330827E8_5CE1_11E3_978C_A8206655A22B_HPP)
