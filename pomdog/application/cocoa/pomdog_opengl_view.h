// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Cocoa/Cocoa.h>
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class SystemEventQueue;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {
class OpenGLContextCocoa;
} // namespace pomdog::detail::cocoa

@interface PomdogOpenGLView : NSView

- (instancetype)initWithFrame:(NSRect)frameRect;

- (void)setEventQueue:(std::shared_ptr<pomdog::detail::SystemEventQueue>)eventQueue;

- (void)setOpenGLContext:(std::shared_ptr<pomdog::detail::cocoa::OpenGLContextCocoa>)openGLContext;

- (void)setRenderCallback:(std::function<void()>)callback;

- (void)setResizingCallback:(std::function<void(bool)>)callback;

- (void)setMouseCursor:(NSCursor*)cursor;

@end
