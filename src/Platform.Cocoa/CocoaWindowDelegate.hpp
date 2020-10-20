// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Signals/detail/ForwardDeclarations.hpp"
#import <Cocoa/Cocoa.h>
#include <memory>

namespace Pomdog::Detail {
class SystemEvent;
} // namespace Pomdog::Detail

@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

- (instancetype)initWithEventQueue:(std::shared_ptr<Pomdog::EventQueue<Pomdog::Detail::SystemEvent>>)eventQueue;

@end
