// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "TestAppGame.hpp"
#include "MaidChanGame.hpp"
#include "GrassBlendingGame.hpp"
#include "ParticleTestGame.hpp"
#include "LightningTestGame.hpp"
#include "MaidBeamGame.hpp"
#include <cmath>
#include <utility>

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
    this->game = std::make_unique<MaidChanGame>(gameHost);
    //this->game = std::make_unique<GrassBlendingGame>(gameHost);
    //this->game = std::make_unique<MaidBeamGame>(gameHost);

    if (game) {
        game->Initialize();
    }
}
//-----------------------------------------------------------------------
void TestAppGame::Update()
{
    if (!game) {
        gameHost->Window()->Title(StringFormat(
            "%3.0f fps, FrameNumber = %6d",
            std::round(gameHost->Clock()->FrameRate()),
            gameHost->Clock()->FrameNumber()));
        return;
    }

    game->Update();
}
//-----------------------------------------------------------------------
void TestAppGame::Draw()
{
    if (!game) {
        auto graphicsContext = gameHost->GraphicsContext();
        graphicsContext->Clear(Color::CornflowerBlue);
        graphicsContext->Present();
        return;
    }

    game->Draw();
}
//-----------------------------------------------------------------------
} // namespace TestApp
