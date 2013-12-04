//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_COCOA_WINDOWDELEGATE_DD858E33_5D15_11E3_9D79_A8206655A22B_HPP
#define POMDOG_SRC_COCOA_WINDOWDELEGATE_DD858E33_5D15_11E3_9D79_A8206655A22B_HPP

#import <Cocoa/Cocoa.h>

#include <memory>
#include "../Application/SystemEventDispatcher.hpp"

@interface CocoaWindowDelegate : NSObject <NSWindowDelegate>

- (id)initWithEventDispatcher:(std::shared_ptr<Pomdog::Details::SystemEventDispatcher>)dispatcher;

@end

#endif // !defined(POMDOG_SRC_COCOA_WINDOWDELEGATE_DD858E33_5D15_11E3_9D79_A8206655A22B_HPP)
