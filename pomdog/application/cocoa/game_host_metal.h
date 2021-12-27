// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/signals/event_queue.h"
#include <memory>

#import <MetalKit/MTKView.h>

namespace pomdog {
class Error;
class Game;
struct PresentationParameters;
} // namespace pomdog

namespace pomdog::detail {
class SystemEvent;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {
class GameWindowCocoa;
} // namespace pomdog::detail::cocoa

namespace pomdog::detail::cocoa {

class GameHostMetal final : public GameHost {
public:
    GameHostMetal();

    ~GameHostMetal() override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        MTKView* metalView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const PresentationParameters& presentationParameters);

    [[nodiscard]] std::unique_ptr<Error>
    InitializeGame(
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

} // namespace pomdog::detail::cocoa
