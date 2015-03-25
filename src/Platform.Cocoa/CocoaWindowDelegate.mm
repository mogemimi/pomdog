// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#import "CocoaWindowDelegate.hpp"
#include "../Application/SystemEventDispatcher.hpp"
#include "Pomdog/Event/Event.hpp"
#include "Pomdog/Utility/Assert.hpp"

@implementation CocoaWindowDelegate
{
	std::shared_ptr<Pomdog::Detail::SystemEventDispatcher> eventDispatcher;
}
//-----------------------------------------------------------------------
- (id)initWithEventDispatcher:(std::shared_ptr<Pomdog::Detail::SystemEventDispatcher>)dispatcher
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
	using Pomdog::Detail::WindowShouldCloseEvent;
	eventDispatcher->Enqueue<WindowShouldCloseEvent>();
    return NO;
}
//-----------------------------------------------------------------------
- (void)windowWillClose:(NSNotification *)notification
{
	using Pomdog::Detail::WindowWillCloseEvent;
	eventDispatcher->Enqueue<WindowWillCloseEvent>();
}
//-----------------------------------------------------------------------
@end
