// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "TestAppGame.hpp"
#include "MaidChanGame.hpp"
#include "GrassBlendingGame.hpp"
#include "ParticleTestGame.hpp"
#include "LightningTestGame.hpp"
#include "MaidBeamGame.hpp"
#include <cmath>
#include <utility>

namespace TestApp {

TestAppGame::TestAppGame(std::shared_ptr<GameHost> const& gameHostIn)
    : gameHost(gameHostIn)
    , commandQueue(gameHostIn->GraphicsCommandQueue())
{}

TestAppGame::~TestAppGame() = default;

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

    auto graphicsDevice = gameHost->GraphicsDevice();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    commandList->Reset();
    commandList->Clear(Color::CornflowerBlue);
    commandList->Close();
}

void TestAppGame::Update()
{
    if (!game) {
        auto window = gameHost->Window();
        window->SetTitle(StringHelper::Format(
            "%3.0f fps, FrameNumber = %6d",
            std::round(gameHost->Clock()->FrameRate()),
            gameHost->Clock()->FrameNumber()));
        return;
    }

    game->Update();
}

void TestAppGame::Draw()
{
    if (!game) {
        commandQueue->Reset();
        commandQueue->PushbackCommandList(commandList);
        commandQueue->Present();
        return;
    }

    game->Draw();
}

} // namespace TestApp
