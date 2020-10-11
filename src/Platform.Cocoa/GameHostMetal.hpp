// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Signals/EventQueue.hpp"
#include <memory>

#import <MetalKit/MTKView.h>

namespace Pomdog {
class Game;
struct PresentationParameters;
} // namespace Pomdog

namespace Pomdog::Detail::Cocoa {

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

    [[nodiscard]] bool IsMetalSupported() const noexcept;

    void Exit() override;

    [[nodiscard]] std::shared_ptr<GameWindow>
    GetWindow() noexcept override;

    [[nodiscard]] std::shared_ptr<GameClock>
    GetClock() noexcept override;

    [[nodiscard]] std::shared_ptr<GraphicsDevice>
    GetGraphicsDevice() noexcept override;

    [[nodiscard]] std::shared_ptr<GraphicsCommandQueue>
    GetGraphicsCommandQueue() noexcept override;

    [[nodiscard]] std::shared_ptr<AudioEngine>
    GetAudioEngine() noexcept override;

    [[nodiscard]] std::shared_ptr<AssetManager>
    GetAssetManager() noexcept override;

    [[nodiscard]] std::shared_ptr<Keyboard>
    GetKeyboard() noexcept override;

    [[nodiscard]] std::shared_ptr<Mouse>
    GetMouse() noexcept override;

    [[nodiscard]] std::shared_ptr<Gamepad>
    GetGamepad() noexcept override;

    [[nodiscard]] std::shared_ptr<IOService>
    GetIOService() noexcept override;

    [[nodiscard]] std::shared_ptr<HTTPClient>
    GetHTTPClient() noexcept override;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog::Detail::Cocoa
