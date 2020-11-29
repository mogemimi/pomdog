// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Signals/EventQueue.hpp"
#include <memory>

@class PomdogOpenGLView;

namespace Pomdog {
class Error;
class Game;
struct PresentationParameters;
} // namespace Pomdog

namespace Pomdog::Detail {
class SystemEvent;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Cocoa {
class GameWindowCocoa;
} // namespace Pomdog::Detail::Cocoa

namespace Pomdog::Detail::Cocoa {

class GameHostCocoa final : public GameHost {
public:
    GameHostCocoa();

    ~GameHostCocoa() override;

    [[nodiscard]] std::shared_ptr<Error>
    Initialize(
        PomdogOpenGLView* openGLView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const PresentationParameters& presentationParameters);

    [[nodiscard]] std::shared_ptr<Error>
    Run(
        const std::weak_ptr<Game>& game,
        std::function<void()>&& onCompleted);

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
