//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_COCOA_OPENGLVIEWDELEGATE_C4D070E8_5E8C_11E3_A36C_A8206655A22B_HPP
#define POMDOG_SRC_COCOA_OPENGLVIEWDELEGATE_C4D070E8_5E8C_11E3_A36C_A8206655A22B_HPP

#import <Cocoa/Cocoa.h>

#include <memory>
#include "../Application/SystemEventDispatcher.hpp"
#include "CocoaMouse.hpp"

@interface CocoaOpenGLViewDelegate : NSResponder

- (id)initWithEventDispatcher:(std::shared_ptr<Pomdog::Details::SystemEventDispatcher>)dispatcher;
- (void)resetMouse:(std::shared_ptr<Pomdog::Details::Cocoa::CocoaMouse>)mouse;
- (void)resetMouse;

@end

#endif // !defined(POMDOG_SRC_COCOA_OPENGLVIEWDELEGATE_C4D070E8_5E8C_11E3_A36C_A8206655A22B_HPP)
