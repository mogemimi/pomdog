// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Signals/detail/ForwardDeclarations.hpp"
#include <functional>
#include <memory>
#import <Cocoa/Cocoa.h>

namespace Pomdog::Detail::Cocoa {

class OpenGLContextCocoa;

} // namespace Pomdog::Detail::Cocoa

@interface PomdogOpenGLView : NSView

- (instancetype)initWithFrame:(NSRect)frameRect;

- (void)setEventQueue:(std::shared_ptr<Pomdog::EventQueue>)eventQueue;

- (void)setOpenGLContext:(
    std::shared_ptr<Pomdog::Detail::Cocoa::OpenGLContextCocoa>)openGLContext;

- (void)setRenderCallback:(std::function<void()>)callback;

- (void)setResizingCallback:(std::function<void(bool)>)callback;

@end
