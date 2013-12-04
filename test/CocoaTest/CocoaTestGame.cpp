//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaTestGame.hpp"
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Application/GameWindow.hpp>
#include <Pomdog/Graphics/GraphicsContext.hpp>
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
CocoaTestGame::CocoaTestGame(std::shared_ptr<GameHost> host)
	: gameHost(std::move(host))
{
	graphicsContext = gameHost->GetGraphicsContext();
}
//-----------------------------------------------------------------------
void CocoaTestGame::Initialize()
{
	auto window = gameHost->GetWindow();
	window->SetCaption("Cocoa Test Game");
}
//-----------------------------------------------------------------------
void CocoaTestGame::Update()
{
}
//-----------------------------------------------------------------------
void CocoaTestGame::Draw()
{
	auto color = Pomdog::Color::CornflowerBlue;
	graphicsContext->Clear(color);
	
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
