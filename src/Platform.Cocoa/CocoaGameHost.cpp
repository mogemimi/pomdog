//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaGameHost.hpp"
#include "Game.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {

//-----------------------------------------------------------------------
CocoaGameHost::CocoaGameHost()
	: exitRequest(false)
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
		game->Draw();
	}
}
//-----------------------------------------------------------------------
void CocoaGameHost::Exit()
{
	exitRequest = true;
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
