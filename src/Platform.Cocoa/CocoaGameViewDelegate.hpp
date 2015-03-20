// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_COCOAGAMEVIEWDELEGATE_1E1DC5F4_6C23_4F8F_B80F_11565DF6CA0D_HPP
#define POMDOG_COCOAGAMEVIEWDELEGATE_1E1DC5F4_6C23_4F8F_B80F_11565DF6CA0D_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#import <Cocoa/Cocoa.h>

#include "MouseCocoa.hpp"
#include "../Application/SystemEventDispatcher.hpp"
#include <memory>

@interface CocoaGameViewDelegate : NSResponder

- (id)initWithEventDispatcher:(std::shared_ptr<Pomdog::Detail::SystemEventDispatcher>)dispatcher;
- (void)resetMouse:(std::shared_ptr<Pomdog::Detail::Cocoa::MouseCocoa>)mouse;
- (void)resetMouse;

@property (nonatomic, retain) NSView* view;

- (void)viewWillStartLiveResize;
- (void)viewDidEndLiveResize;

@end

#endif // !defined(POMDOG_COCOAGAMEVIEWDELEGATE_1E1DC5F4_6C23_4F8F_B80F_11565DF6CA0D_HPP)
