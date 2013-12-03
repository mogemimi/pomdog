//
//  Copyright (C) 2013 mogemimi.
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
void BootstrapperCocoa::BuildOpenGLView(NSWindow* nativeWindow)
{
	POMDOG_ASSERT(nativeWindow != nil);
	auto window = std::make_shared<CocoaGameWindow>(nativeWindow);
	auto host = std::make_shared<CocoaGameHost>(window);
	
	gameWindow = window;
	gameHost = host;
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
