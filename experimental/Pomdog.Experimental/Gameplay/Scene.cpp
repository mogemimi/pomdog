// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Scene.hpp"

namespace Pomdog {

void GameScene::Enter()
{
    if (OnEnter) {
        OnEnter();
    }
}

void GameScene::Update()
{
    if (OnUpdate) {
        OnUpdate();
    }
}

void GameScene::Exit()
{
    if (OnExit) {
        OnExit();
    }
}

void GameScene::SetName(const std::string& nameIn) noexcept
{
    this->name = nameIn;
}

std::string GameScene::GetName() const noexcept
{
    return name;
}

SceneDirector::SceneDirector(const std::shared_ptr<GameHost>& gameHost)
    : clock(gameHost->GetClock())
{
}

void SceneDirector::Run(const std::shared_ptr<GameScene>& scene)
{
    POMDOG_ASSERT(scene);
    POMDOG_ASSERT(sceneStack.empty());
    sceneStack.push_back(scene);
}

void SceneDirector::Transit(const std::shared_ptr<GameScene>& scene)
{
    POMDOG_ASSERT(scene);

    if (!sceneStack.empty()) {
        auto oldScene = sceneStack.back();
        POMDOG_ASSERT(oldScene);
        transitionQueue.push_back(oldScene);
        sceneStack.pop_back();
    }
    sceneStack.push_back(scene);
}

void SceneDirector::Update()
{
    POMDOG_ASSERT(clock);

    auto currentScene = GetCurrentScene();
    if (currentScene) {
        if (!currentScene->frameNumber) {
            currentScene->Enter();
        }

        POMDOG_ASSERT(!currentScene->frameNumber ||
            *currentScene->frameNumber < clock->GetFrameNumber());

        currentScene->frameNumber = clock->GetFrameNumber();
        currentScene->Update();
    }

    if (!transitionQueue.empty()) {
        for (auto& oldScene : transitionQueue) {
            if (!oldScene->frameNumber) {
                oldScene->Enter();
            }
            oldScene->Exit();
        }
        transitionQueue.clear();
    }
}

std::shared_ptr<GameScene> SceneDirector::GetCurrentScene() const noexcept
{
    if (!transitionQueue.empty()) {
        POMDOG_ASSERT(transitionQueue.front());
        return transitionQueue.front();
    }

    if (sceneStack.empty()) {
        return {};
    }
    // TODO: Please write and run test code.
    return sceneStack.back();
}

} // namespace Pomdog
