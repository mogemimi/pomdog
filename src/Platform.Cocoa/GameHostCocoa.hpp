// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Signals/EventQueue.hpp"
#include <memory>

@class PomdogOpenGLView;

namespace Pomdog {

class Game;
struct PresentationParameters;

namespace Detail {
namespace Cocoa {

class GameWindowCocoa;

class GameHostCocoa final : public GameHost {
public:
    GameHostCocoa(
        PomdogOpenGLView* openGLView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<EventQueue>& eventQueue,
        const PresentationParameters& presentationParameters);

    ~GameHostCocoa();

    void Run(
        const std::weak_ptr<Game>& game,
        std::function<void()>&& onCompleted);

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
