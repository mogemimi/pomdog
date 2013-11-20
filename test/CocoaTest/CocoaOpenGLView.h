//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import <Cocoa/Cocoa.h>

@class NSOpenGLContext, NSOpenGLPixelFormat;

@interface CocoaOpenGLView : NSView
{
@private
	NSOpenGLContext*     openGLContext_;
	NSOpenGLPixelFormat* pixelFormat_;
}

@property (nonatomic, retain) NSOpenGLContext*     openGLContext;
@property (nonatomic, retain) NSOpenGLPixelFormat* pixelFormat;

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format;

- (void)update;
- (void)reshape;

- (void)prepareOpenGL;
- (void)clearGLContext;

@end
