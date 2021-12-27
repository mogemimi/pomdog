// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/signals/forward_declarations.h"
#import <Cocoa/Cocoa.h>
#include <functional>
#include <memory>

namespace pomdog::detail {
class SystemEvent;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {
class OpenGLContextCocoa;
} // namespace pomdog::detail::cocoa

@interface PomdogOpenGLView : NSView

- (instancetype)initWithFrame:(NSRect)frameRect;

- (void)setEventQueue:(std::shared_ptr<pomdog::EventQueue<pomdog::detail::SystemEvent>>)eventQueue;

- (void)setOpenGLContext:(std::shared_ptr<pomdog::detail::cocoa::OpenGLContextCocoa>)openGLContext;

- (void)setRenderCallback:(std::function<void()>)callback;

- (void)setResizingCallback:(std::function<void(bool)>)callback;

- (void)setMouseCursor:(NSCursor*)cursor;

@end
