// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <Pomdog/Pomdog.hpp>

namespace TestApp {

using namespace Pomdog;

class TestAppGame: public Game {
public:
    explicit TestAppGame(std::shared_ptr<GameHost> const& gameHost);

    ~TestAppGame();

    void Initialize();

    void Update();

    void Draw();

private:
    std::shared_ptr<GameHost> gameHost;
    std::unique_ptr<Game> game;
    std::shared_ptr<GraphicsCommandQueue> commandQueue;
    std::shared_ptr<GraphicsCommandList> commandList;
};

} // namespace TestApp
