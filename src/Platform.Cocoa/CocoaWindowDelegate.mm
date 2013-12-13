//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import "CocoaWindowDelegate.hpp"

#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Event/Event.hpp>
#include <Pomdog/Event/EventCodeHelper.hpp>

using Pomdog::Event;
using Pomdog::EventCodeHelper;

static auto WindowWillCloseEvent = EventCodeHelper::CreateCode("WindowWillClose");
static auto WindowShouldCloseEvent = EventCodeHelper::CreateCode("WindowShouldClose");

@implementation CocoaWindowDelegate
{
	std::shared_ptr<Pomdog::Details::SystemEventDispatcher> eventDispatcher;
}
//-----------------------------------------------------------------------
- (id)initWithEventDispatcher:(std::shared_ptr<Pomdog::Details::SystemEventDispatcher>)dispatcher
{
	self = [super init];
	if (self) {
		// insert code here to initialize delegate
		POMDOG_ASSERT(dispatcher);
		eventDispatcher = dispatcher;
	}
	return self;
}
//-----------------------------------------------------------------------
- (BOOL)windowShouldClose:(id)sender
{
	auto event = std::make_shared<Event>(WindowShouldCloseEvent);
	eventDispatcher->Enqueue(event);
    return NO;
}
//-----------------------------------------------------------------------
- (void)windowWillClose:(NSNotification *)notification
{
	auto event = std::make_shared<Event>(WindowWillCloseEvent);
	eventDispatcher->Enqueue(event);
}
//-----------------------------------------------------------------------
@end
