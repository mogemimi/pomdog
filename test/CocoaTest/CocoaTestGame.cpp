//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaTestGame.hpp"
#include <Pomdog/Math/Color.hpp>

namespace Pomdog {

CocoaTestGame::CocoaTestGame(GameHost & host)
	: gameHost(host.shared_from_this())
{
	//graphicsContext = host.GetGraphicsContext();
}

void CocoaTestGame::Initialize()
{
}

void CocoaTestGame::Update()
{
}

void CocoaTestGame::Draw()
{
//	auto color = Pomdog::Color::CornflowerBlue;
//	graphicsContext->Clear(color);
//	
//	graphicsContext->Present();
}

}// namespace Pomdog
