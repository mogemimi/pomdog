//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "TestAppGame.hpp"
#include <utility>
#include "MaidChanGame.hpp"
#include "GrassBlendingGame.hpp"
#include "ParticleTestGame.hpp"
#include "LightningTestGame.hpp"

namespace TestApp {
//-----------------------------------------------------------------------
TestAppGame::TestAppGame(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
{}
//-----------------------------------------------------------------------
TestAppGame::~TestAppGame() = default;
//-----------------------------------------------------------------------
void TestAppGame::Initialize()
{
	//this->game = std::make_unique<LightningTestGame>(gameHost);
	//this->game = std::make_unique<ParticleTestGame>(gameHost);
	//this->game = std::make_unique<MaidChanGame>(gameHost);
	this->game = std::make_unique<GrassBlendingGame>(gameHost);
	
	game->Initialize();
}
//-----------------------------------------------------------------------
void TestAppGame::Update()
{
	if (!game) {
		return;
	}

	game->Update();
}
//-----------------------------------------------------------------------
void TestAppGame::Draw()
{
	if (!game)
	{
		auto graphicsContext = gameHost->GraphicsContext();
		graphicsContext->Clear(Color::CornflowerBlue);
		graphicsContext->Present();
		return;
	}
	
	game->Draw();
}
//-----------------------------------------------------------------------
}// namespace TestApp
