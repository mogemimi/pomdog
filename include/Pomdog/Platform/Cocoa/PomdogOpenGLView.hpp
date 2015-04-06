// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_OPENGLVIEW_5390FBCF_HPP
#define POMDOG_OPENGLVIEW_5390FBCF_HPP

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/detail/ForwardDeclarations.hpp"
#include <memory>
#include <functional>

#import <Cocoa/Cocoa.h>

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class OpenGLContextCocoa;

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog

POMDOG_EXPORT @interface PomdogOpenGLView : NSView

- (id)initWithFrame:(NSRect)frameRect;

- (void)setEventQueue:(std::shared_ptr<Pomdog::EventQueue>)eventQueue;

- (void)setOpenGLContext:(std::shared_ptr<
    Pomdog::Detail::Cocoa::OpenGLContextCocoa>)openGLContext;

- (void)setRenderCallback:(std::function<void()>)callback;

- (void)setResizingCallback:(std::function<void(bool)>)callback;

@end

#endif // POMDOG_OPENGLVIEW_5390FBCF_HPP
