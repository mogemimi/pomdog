//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_COCOA_GAMEVIEWDELEGATE_1E1DC5F4_6C23_4F8F_B80F_11565DF6CA0D_HPP
#define POMDOG_SRC_COCOA_GAMEVIEWDELEGATE_1E1DC5F4_6C23_4F8F_B80F_11565DF6CA0D_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#import <Cocoa/Cocoa.h>

#include "MouseCocoa.hpp"
#include "../Application/SystemEventDispatcher.hpp"
#include <memory>

@interface CocoaGameViewDelegate : NSResponder

- (id)initWithEventDispatcher:(std::shared_ptr<Pomdog::Details::SystemEventDispatcher>)dispatcher;
- (void)resetMouse:(std::shared_ptr<Pomdog::Details::Cocoa::MouseCocoa>)mouse;
- (void)resetMouse;

@property (nonatomic, retain) NSView* view;

- (void)viewNeedsUpdateSurface;
- (void)viewWillStartLiveResize;
- (void)viewDidEndLiveResize;

@end

#endif // !defined(POMDOG_SRC_COCOA_GAMEVIEWDELEGATE_1E1DC5F4_6C23_4F8F_B80F_11565DF6CA0D_HPP)
