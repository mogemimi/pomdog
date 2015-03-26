// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_COCOAGAMEVIEWDELEGATE_1E1DC5F4_HPP
#define POMDOG_COCOAGAMEVIEWDELEGATE_1E1DC5F4_HPP

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

#endif // POMDOG_COCOAGAMEVIEWDELEGATE_1E1DC5F4_HPP
