// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/detail/ForwardDeclarations.hpp"
#include <functional>
#include <memory>

#import <Cocoa/Cocoa.h>

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class OpenGLContextCocoa;

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog

POMDOG_EXPORT @interface PomdogOpenGLView : NSView

- (instancetype)initWithFrame:(NSRect)frameRect;

- (void)setEventQueue:(std::shared_ptr<Pomdog::EventQueue>)eventQueue;

- (void)setOpenGLContext:(
    std::shared_ptr<Pomdog::Detail::Cocoa::OpenGLContextCocoa>)openGLContext;

- (void)setRenderCallback:(std::function<void()>)callback;

- (void)setResizingCallback:(std::function<void(bool)>)callback;

@end
