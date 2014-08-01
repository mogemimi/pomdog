//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Application/detail/Platform.Cocoa/BootstrapperCocoa.hpp>
#include "CocoaGameWindow.hpp"
#include "CocoaGameHost.hpp"
#include "../RenderSystem/PresentationParameters.hpp"
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {
namespace Cocoa {
//-----------------------------------------------------------------------
void BootstrapperCocoa::BeginRun(NSWindow* nativeWindow)
{
	auto eventDispatcher = std::make_shared<SystemEventDispatcher>();

	POMDOG_ASSERT(nativeWindow != nil);
	gameWindow = std::make_shared<CocoaGameWindow>(nativeWindow, eventDispatcher);

	using Details::RenderSystem::PresentationParameters;
	
	PresentationParameters presentationParameters;
	presentationParameters.DepthFormat = DepthFormat::Depth24Stencil8;
	auto clientBounds = gameWindow->ClientBounds();
	presentationParameters.BackBufferWidth = clientBounds.Width;
	presentationParameters.BackBufferHeight = clientBounds.Height;
	presentationParameters.IsFullScreen = false;
	
	gameHost = std::make_shared<CocoaGameHost>(gameWindow, eventDispatcher, presentationParameters);
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
