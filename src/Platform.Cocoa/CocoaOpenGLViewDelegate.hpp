//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_COCOA_OPENGLVIEWDELEGATE_98EB2FAF_25C9_49AD_89F9_8596AE7E8025_HPP
#define POMDOG_SRC_COCOA_OPENGLVIEWDELEGATE_98EB2FAF_25C9_49AD_89F9_8596AE7E8025_HPP

#import <Cocoa/Cocoa.h>

#include <memory>
#include "../Application/SystemEventDispatcher.hpp"
#include "CocoaMouse.hpp"

@interface CocoaOpenGLViewDelegate : NSResponder

- (id)initWithEventDispatcher:(std::shared_ptr<Pomdog::Details::SystemEventDispatcher>)dispatcher;
- (void)resetMouse:(std::shared_ptr<Pomdog::Details::Cocoa::CocoaMouse>)mouse;
- (void)resetMouse;

@end

#endif // !defined(POMDOG_SRC_COCOA_OPENGLVIEWDELEGATE_98EB2FAF_25C9_49AD_89F9_8596AE7E8025_HPP)
