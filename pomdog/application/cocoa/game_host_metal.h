// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/signals/event_queue.h"
#include <memory>

#import <MetalKit/MTKView.h>

namespace pomdog {
class Error;
class Game;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

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
    initialize(
        MTKView* metalView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const gpu::PresentationParameters& presentationParameters);

    [[nodiscard]] std::unique_ptr<Error>
    initializeGame(
        const std::weak_ptr<Game>& game,
        const std::function<void()>& onCompleted);

    void gameLoop();

    [[nodiscard]] bool
    isMetalSupported() const noexcept;

    void exit() override;

    [[nodiscard]] std::shared_ptr<GameWindow>
    getWindow() noexcept override;

    [[nodiscard]] std::shared_ptr<GameClock>
    getClock() noexcept override;

    [[nodiscard]] std::shared_ptr<gpu::GraphicsDevice>
    getGraphicsDevice() noexcept override;

    [[nodiscard]] std::shared_ptr<gpu::CommandQueue>
    getCommandQueue() noexcept override;

    [[nodiscard]] std::shared_ptr<AudioEngine>
    getAudioEngine() noexcept override;

    [[nodiscard]] std::shared_ptr<AssetManager>
    getAssetManager() noexcept override;

    [[nodiscard]] std::shared_ptr<Keyboard>
    getKeyboard() noexcept override;

    [[nodiscard]] std::shared_ptr<Mouse>
    getMouse() noexcept override;

    [[nodiscard]] std::shared_ptr<Gamepad>
    getGamepad() noexcept override;

    [[nodiscard]] std::shared_ptr<IOService>
    getIOService() noexcept override;

    [[nodiscard]] std::shared_ptr<HTTPClient>
    getHTTPClient() noexcept override;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace pomdog::detail::cocoa
