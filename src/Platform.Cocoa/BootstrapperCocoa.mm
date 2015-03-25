// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Platform/Cocoa/BootstrapperCocoa.hpp"
#include "GameWindowCocoa.hpp"
#include "GameHostCocoa.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Cocoa {
//-----------------------------------------------------------------------
BootstrapperCocoa & BootstrapperCocoa::SetSurfaceFormat(SurfaceFormat surfaceFormatIn)
{
	surfaceFormat = surfaceFormatIn;
	return *this;
}
//-----------------------------------------------------------------------
BootstrapperCocoa & BootstrapperCocoa::SetDepthFormat(DepthFormat depthFormatIn)
{
	depthFormat = depthFormatIn;
	return *this;
}
//-----------------------------------------------------------------------
BootstrapperCocoa & BootstrapperCocoa::SetPresentationInterval(std::uint32_t presentationIntervalIn)
{
	POMDOG_ASSERT(presentationIntervalIn > 0);
	POMDOG_ASSERT(presentationIntervalIn <= 120);
	presentationInterval = presentationIntervalIn;
	return *this;
}
//-----------------------------------------------------------------------
std::shared_ptr<GameHost> BootstrapperCocoa::CreateGameHost(NSWindow* nativeWindow)
{
	NSRect bounds = [[nativeWindow contentView] bounds];

	PresentationParameters presentationParameters;
	presentationParameters.SurfaceFormat = surfaceFormat;
	presentationParameters.DepthFormat = depthFormat;
	presentationParameters.PresentationInterval = presentationInterval;
	presentationParameters.BackBufferWidth = bounds.size.width;
	presentationParameters.BackBufferHeight = bounds.size.height;
	presentationParameters.IsFullScreen = false;

	auto eventDispatcher = std::make_shared<SystemEventDispatcher>();

	POMDOG_ASSERT(nativeWindow != nil);
	auto gameWindow = std::make_shared<GameWindowCocoa>(nativeWindow, eventDispatcher);

	auto gameHost = std::make_shared<GameHostCocoa>(gameWindow,
		eventDispatcher, presentationParameters);

	return std::move(gameHost);
}
//-----------------------------------------------------------------------
}// namespace Cocoa
}// namespace Detail
}// namespace Pomdog
