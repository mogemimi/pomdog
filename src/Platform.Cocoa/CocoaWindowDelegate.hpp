// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Signals/EventQueue.hpp"
#import <Cocoa/Cocoa.h>
#include <memory>

@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

- (instancetype)initWithEventQueue:(std::shared_ptr<Pomdog::EventQueue>)eventQueue;

@end
