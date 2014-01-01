//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_COCOA_OPENGLVIEW_A756AF9C_916F_4DDB_AEDA_04BE15416A3E_HPP
#define POMDOG_SRC_COCOA_OPENGLVIEW_A756AF9C_916F_4DDB_AEDA_04BE15416A3E_HPP

#import <Cocoa/Cocoa.h>

@class NSOpenGLContext, NSOpenGLPixelFormat;

@interface CocoaOpenGLView : NSView
{
@private
	NSOpenGLContext* openGLContext_;
	NSResponder* delegate_;
}

@property (nonatomic, retain) NSOpenGLContext* openGLContext;
@property (nonatomic, retain) NSResponder* delegate;

- (id)initWithFrame:(NSRect)frameRect;

- (void)clearGLContext;
- (void)update;
- (void)reshape;

@end

#endif // !defined(POMDOG_SRC_COCOA_OPENGLVIEW_A756AF9C_916F_4DDB_AEDA_04BE15416A3E_HPP)
