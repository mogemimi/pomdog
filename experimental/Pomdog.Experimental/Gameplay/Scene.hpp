// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <Pomdog/Pomdog.hpp>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace Pomdog {

struct GameScene final {
    std::function<void()> OnEnter;
    std::function<void()> OnUpdate;
    std::function<void()> OnExit;
    std::string name;
    ConnectionList connections;
    std::optional<std::int64_t> frameNumber;

    void Enter();

    void Update();

    void Exit();

    void SetName(const std::string& name) noexcept;

    std::string GetName() const noexcept;
};

class SceneDirector final {
public:
    explicit SceneDirector(const std::shared_ptr<GameHost> & gameHost);

    void Run(const std::shared_ptr<GameScene>& scene);

    void Transit(const std::shared_ptr<GameScene>& scene);

    void Update();

    std::shared_ptr<GameScene> GetCurrentScene() const noexcept;

private:
    std::shared_ptr<GameClock> clock;
    std::vector<std::shared_ptr<GameScene>> sceneStack;
    std::deque<std::shared_ptr<GameScene>> transitionQueue;
};

} // namespace Pomdog
