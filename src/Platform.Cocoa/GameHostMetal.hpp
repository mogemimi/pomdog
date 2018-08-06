// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Signals/EventQueue.hpp"
#include <memory>

#import <MetalKit/MTKView.h>

namespace Pomdog {

class Game;
struct PresentationParameters;

namespace Detail {
namespace Cocoa {

class GameWindowCocoa;

class GameHostMetal final : public GameHost {
public:
    GameHostMetal(
        MTKView* metalView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<EventQueue>& eventQueue,
        const PresentationParameters& presentationParameters);

    ~GameHostMetal();

    void InitializeGame(
        const std::weak_ptr<Game>& game,
        const std::function<void()>& onCompleted);

    void GameLoop();

    bool IsMetalSupported() const;

    void Exit() override;

    std::shared_ptr<GameWindow> GetWindow() override;

    std::shared_ptr<GameClock> GetClock() override;

    std::shared_ptr<GraphicsDevice> GetGraphicsDevice() override;

    std::shared_ptr<GraphicsCommandQueue> GetGraphicsCommandQueue() override;

    std::shared_ptr<AudioEngine> GetAudioEngine() override;

    std::shared_ptr<AssetManager> GetAssetManager() override;

    std::shared_ptr<Keyboard> GetKeyboard() override;

    std::shared_ptr<Mouse> GetMouse() override;

    std::shared_ptr<Gamepad> GetGamepad() override;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
