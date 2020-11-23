// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/DepthFormat.hpp"
#include "GameWindowX11.hpp"
#include "KeyboardX11.hpp"
#include "MouseX11.hpp"
#include "OpenGLContextX11.hpp"
#include "X11Context.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <memory>

namespace Pomdog {
class Game;
} // namespace Pomdog

namespace Pomdog::Detail::X11 {

class GameHostX11 final : public GameHost {
public:
    GameHostX11() noexcept;

    ~GameHostX11();

    [[nodiscard]] std::shared_ptr<Error>
    Initialize(const PresentationParameters& presentationParameters);

    void Run(Game& game);

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

} // namespace Pomdog::Detail::X11
