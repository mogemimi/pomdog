// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#import "CocoaWindowDelegate.hpp"
#include "../Application/SystemEvents.hpp"
#include "Pomdog/Signals/Event.hpp"
#include "Pomdog/Utility/Assert.hpp"

@implementation CocoaWindowDelegate
{
    std::shared_ptr<Pomdog::EventQueue> eventQueue;
}
//-----------------------------------------------------------------------
- (instancetype)initWithEventQueue:(std::shared_ptr<Pomdog::EventQueue>)eventQueueIn
{
    self = [super init];
    if (self) {
        POMDOG_ASSERT(eventQueueIn);
        eventQueue = eventQueueIn;
    }
    return self;
}
//-----------------------------------------------------------------------
- (BOOL)windowShouldClose:(id)sender
{
    using Pomdog::Detail::WindowShouldCloseEvent;
    eventQueue->Enqueue<WindowShouldCloseEvent>();
    return NO;
}
//-----------------------------------------------------------------------
- (void)windowWillClose:(NSNotification *)notification
{
    using Pomdog::Detail::WindowWillCloseEvent;
    eventQueue->Enqueue<WindowWillCloseEvent>();
}
//-----------------------------------------------------------------------
@end
