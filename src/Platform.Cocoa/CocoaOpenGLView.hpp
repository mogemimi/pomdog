//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_COCOA_OPENGLVIEW_A756AF9C_916F_4DDB_AEDA_04BE15416A3E_HPP
#define POMDOG_SRC_COCOA_OPENGLVIEW_A756AF9C_916F_4DDB_AEDA_04BE15416A3E_HPP

#import <Cocoa/Cocoa.h>
#include <functional>

@class NSOpenGLContext;
@class CocoaGameViewDelegate;

@interface CocoaOpenGLView : NSView

@property (nonatomic, retain) NSOpenGLContext* openGLContext;
@property (nonatomic, retain) CocoaGameViewDelegate* delegate;

- (id)initWithFrame:(NSRect)frameRect;

- (void)clearGLContext;
- (void)reshape;

- (void)setRenderCallback:(std::function<void()>)callback;

@end

#endif // !defined(POMDOG_SRC_COCOA_OPENGLVIEW_A756AF9C_916F_4DDB_AEDA_04BE15416A3E_HPP)
