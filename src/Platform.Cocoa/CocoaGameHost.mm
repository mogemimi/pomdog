//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaGameHost.hpp"
#include "Game.hpp"
#include <utility>
#include "CocoaGameWindow.hpp"
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {
namespace Cocoa {

//-----------------------------------------------------------------------
CocoaGameHost::CocoaGameHost(std::shared_ptr<CocoaGameWindow> window)
	: exitRequest(false)
	, gameWindow(window)
{
}
//-----------------------------------------------------------------------
CocoaGameHost::~CocoaGameHost()
{
}
//-----------------------------------------------------------------------
void CocoaGameHost::Run(std::weak_ptr<Game> weakGame)
{
	auto game = weakGame.lock();
	
	if (!game) {
		return;
	}
	
	game->Initialize();
	
	while (!exitRequest)
	{
		game->Update();
		RenderFrame(*game);
	}
}
//-----------------------------------------------------------------------
void CocoaGameHost::Exit()
{
	exitRequest = true;
}
//-----------------------------------------------------------------------
void CocoaGameHost::RenderFrame(Game & game)
{
	POMDOG_ASSERT(gameWindow);

	if (!gameWindow || gameWindow->IsMinimized()) {
		// skip rendering
		return;
	}
	game.Draw();
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
