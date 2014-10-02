//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_COCOA_WINDOWDELEGATE_56FEB6D8_A0C6_4621_9A92_A8ADFFC725B3_HPP
#define POMDOG_SRC_COCOA_WINDOWDELEGATE_56FEB6D8_A0C6_4621_9A92_A8ADFFC725B3_HPP

#import <Cocoa/Cocoa.h>

#include "../Application/SystemEventDispatcher.hpp"
#include <memory>

@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

- (id)initWithEventDispatcher:(std::shared_ptr<Pomdog::Details::SystemEventDispatcher>)dispatcher;

@end

#endif // !defined(POMDOG_SRC_COCOA_WINDOWDELEGATE_56FEB6D8_A0C6_4621_9A92_A8ADFFC725B3_HPP)
