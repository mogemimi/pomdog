//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_COCOAWINDOWDELEGATE_56FEB6D8_A0C6_4621_9A92_A8ADFFC725B3_HPP
#define POMDOG_SRC_COCOAWINDOWDELEGATE_56FEB6D8_A0C6_4621_9A92_A8ADFFC725B3_HPP

#include "../Application/SystemEventDispatcher.hpp"
#import <Cocoa/Cocoa.h>
#include <memory>

@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

- (id)initWithEventDispatcher:(std::shared_ptr<Pomdog::Details::SystemEventDispatcher>)dispatcher;

@end

#endif // !defined(POMDOG_SRC_COCOAWINDOWDELEGATE_56FEB6D8_A0C6_4621_9A92_A8ADFFC725B3_HPP)
