// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_COCOAWINDOWDELEGATE_56FEB6D8_A0C6_4621_9A92_A8ADFFC725B3_HPP
#define POMDOG_COCOAWINDOWDELEGATE_56FEB6D8_A0C6_4621_9A92_A8ADFFC725B3_HPP

#include "../Application/SystemEventDispatcher.hpp"
#import <Cocoa/Cocoa.h>
#include <memory>

@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

- (id)initWithEventDispatcher:(std::shared_ptr<Pomdog::Detail::SystemEventDispatcher>)dispatcher;

@end

#endif // !defined(POMDOG_COCOAWINDOWDELEGATE_56FEB6D8_A0C6_4621_9A92_A8ADFFC725B3_HPP)
