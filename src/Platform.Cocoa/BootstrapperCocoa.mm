//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Platform/Cocoa/BootstrapperCocoa.hpp"
#include "CocoaGameWindow.hpp"
#include "CocoaGameHost.hpp"
#include "../RenderSystem/PresentationParameters.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {
//-----------------------------------------------------------------------
BootstrapperCocoa::BootstrapperCocoa(NSWindow* nativeWindowIn)
	: nativeWindow(nativeWindowIn)
{}
//-----------------------------------------------------------------------
void BootstrapperCocoa::Run(std::function<void(std::shared_ptr<GameHost> const&)> const& runGame)
{
	auto eventDispatcher = std::make_shared<SystemEventDispatcher>();

	POMDOG_ASSERT(nativeWindow != nil);
	auto gameWindow = std::make_shared<CocoaGameWindow>(nativeWindow, eventDispatcher);
	auto clientBounds = gameWindow->ClientBounds();

	using Details::RenderSystem::PresentationParameters;
	
	PresentationParameters presentationParameters;
	presentationParameters.DepthFormat = DepthFormat::Depth24Stencil8;
	presentationParameters.BackBufferWidth = clientBounds.Width;
	presentationParameters.BackBufferHeight = clientBounds.Height;
	presentationParameters.IsFullScreen = false;
	
	auto gameHost = std::make_shared<CocoaGameHost>(gameWindow,
		eventDispatcher, presentationParameters);
	
	POMDOG_ASSERT(runGame);
	runGame(gameHost);

	gameHost.reset();
	gameWindow.reset();
}
//-----------------------------------------------------------------------
}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
