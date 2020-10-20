// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#import "CocoaWindowDelegate.hpp"
#include "../Application/SystemEvents.hpp"
#include "Pomdog/Signals/EventQueue.hpp"
#include "Pomdog/Utility/Assert.hpp"

using Pomdog::Detail::SystemEvent;
using Pomdog::Detail::SystemEventKind;
using Pomdog::EventQueue;

@implementation CocoaWindowDelegate {
    std::shared_ptr<EventQueue<SystemEvent>> eventQueue;
}

- (instancetype)initWithEventQueue:(std::shared_ptr<EventQueue<SystemEvent>>)eventQueueIn
{
    self = [super init];
    if (self) {
        POMDOG_ASSERT(eventQueueIn);
        eventQueue = std::move(eventQueueIn);
    }
    return self;
}

- (BOOL)windowShouldClose:(id)sender
{
    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::WindowShouldCloseEvent,
    });
    return NO;
}

- (void)windowWillClose:(NSNotification *)notification
{
    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::WindowWillCloseEvent,
    });
}

@end
