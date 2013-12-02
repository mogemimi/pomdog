//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Application/detail/Platform.Cocoa/BootstrapperCocoa.hpp>
#include <Pomdog/Application/detail/PlatformSystem.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include "CocoaGameWindow.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {
//-----------------------------------------------------------------------
BootstrapperCocoa::BootstrapperCocoa()
{
	using Pomdog::Details::PlatformSystem;
	
	gameHost = PlatformSystem::CreateGameHost();
}
//-----------------------------------------------------------------------
void BootstrapperCocoa::BuildOpenGLView(NSWindow* nativeWindow)
{
	POMDOG_ASSERT(nativeWindow != nil);
	gameWindow = std::make_shared<CocoaGameWindow>(nativeWindow);
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
