//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import <Cocoa/Cocoa.h>
#include <memory>
#include "../../src/OSX/GLContextOSX.hpp"

@class NSOpenGLContext, NSOpenGLPixelFormat;

@interface CocoaOpenGLView : NSView
{
@private
	std::unique_ptr<Pomdog::Details::OSX::GLContextOSX> contextOSX;
}

- (id)initWithFrame:(NSRect)frameRect;

- (void)update;
- (void)reshape;

- (void)prepareOpenGL;
- (void)clearGLContext;

@end
