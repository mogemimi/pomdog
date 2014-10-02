//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import "CocoaWindowDelegate.hpp"

#include "../Application/SystemEventDispatcher.hpp"
#include <Pomdog/Event/Event.hpp>
#include <Pomdog/Utility/Assert.hpp>

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
	using Pomdog::Details::WindowShouldCloseEvent;
	eventDispatcher->Enqueue<WindowShouldCloseEvent>();
    return NO;
}
//-----------------------------------------------------------------------
- (void)windowWillClose:(NSNotification *)notification
{
	using Pomdog::Details::WindowWillCloseEvent;
	eventDispatcher->Enqueue<WindowWillCloseEvent>();
}
//-----------------------------------------------------------------------
@end
