// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/DepthFormat.hpp"
#include "OpenGLContextX11.hpp"
#include "GameWindowX11.hpp"
#include "KeyboardX11.hpp"
#include "MouseX11.hpp"
#include "X11Context.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Application/GameClock.hpp"
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

    explicit GameHostX11(PresentationParameters const& presentationParameters);

    ~GameHostX11();

    void Run(Game & game);

    void Exit() override;

    std::shared_ptr<Pomdog::GameWindow> Window() override;

    std::shared_ptr<Pomdog::GameClock> Clock() override;

    std::shared_ptr<Pomdog::GraphicsDevice> GraphicsDevice() override;

    std::shared_ptr<Pomdog::GraphicsCommandQueue> GraphicsCommandQueue() override;

    std::shared_ptr<Pomdog::AudioEngine> AudioEngine() override;

    std::shared_ptr<Pomdog::AssetManager> AssetManager() override;

    std::shared_ptr<Pomdog::Keyboard> Keyboard() override;

    std::shared_ptr<Pomdog::Mouse> Mouse() override;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace X11
} // namespace Detail
} // namespace Pomdog
