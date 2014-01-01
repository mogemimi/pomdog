//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Application/detail/Platform.Cocoa/BootstrapperCocoa.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include "CocoaGameWindow.hpp"
#include "CocoaGameHost.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {
//-----------------------------------------------------------------------
BootstrapperCocoa::BootstrapperCocoa()
{
}
//-----------------------------------------------------------------------
void BootstrapperCocoa::BeginRun(NSWindow* nativeWindow)
{
	auto eventDispatcher = std::make_shared<SystemEventDispatcher>();

	POMDOG_ASSERT(nativeWindow != nil);
	auto window = std::make_shared<CocoaGameWindow>(nativeWindow, eventDispatcher);
	auto host = std::make_shared<CocoaGameHost>(window, eventDispatcher);
	
	gameWindow = window;
	gameHost = host;
}
//-----------------------------------------------------------------------
void BootstrapperCocoa::EndRun()
{
	gameHost.reset();
	gameWindow.reset();
	
	///@note Shutdown your application:
	[NSApp terminate:nil];
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
