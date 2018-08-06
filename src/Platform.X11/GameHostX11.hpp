// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

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
#include "Pomdog/Utility/Assert.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <memory>

namespace Pomdog {

class Game;

namespace Detail {
namespace X11 {

class GameHostX11 final : public GameHost {
public:
    GameHostX11() = delete;

    explicit GameHostX11(const PresentationParameters& presentationParameters);

    ~GameHostX11();

    void Run(Game & game);

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

} // namespace X11
} // namespace Detail
} // namespace Pomdog
