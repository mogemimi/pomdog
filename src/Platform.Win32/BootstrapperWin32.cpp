//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Pomdog/Application/detail/Platform.Win32/BootstrapperWin32.hpp"
#include "GameHostWin32.hpp"
#include "GameWindowWin32.hpp"
#include "../RenderSystem/PresentationParameters.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Logging/Log.hpp"

namespace Pomdog {
namespace Details {
namespace Win32 {
//-----------------------------------------------------------------------
BootstrapperWin32::BootstrapperWin32(HINSTANCE hInstanceIn, int nCmdShowIn, int widthIn, int heightIn, bool isFullscreenIn)
	: hInstance(hInstanceIn)
	, nCmdShow(nCmdShowIn)
	, width(widthIn)
	, height(heightIn)
	, isFullscreen(isFullscreenIn)
{}
//-----------------------------------------------------------------------
void BootstrapperWin32::Run(std::function<void(std::shared_ptr<GameHost> const& gameHost)> const& runGame)
{
	Details::RenderSystem::PresentationParameters presentationParameters;
	presentationParameters.BackBufferHeight = height;
	presentationParameters.BackBufferWidth = width;
	presentationParameters.DepthFormat = DepthFormat::Depth24Stencil8;
	presentationParameters.IsFullScreen = isFullscreen;

	auto systemEventDispatcher = std::make_shared<SystemEventDispatcher>();
	
	auto gameWindow = std::make_shared<GameWindowWin32>(hInstance, nCmdShow,
		systemEventDispatcher, presentationParameters);
	auto gameHost = std::make_shared<GameHostWin32>(gameWindow,
		systemEventDispatcher, presentationParameters);

	POMDOG_ASSERT(runGame);
	runGame(gameHost);

	gameHost.reset();
	gameWindow.reset();
}
//-----------------------------------------------------------------------
}// namespace Win32
}// namespace Details
}// namespace Pomdog
