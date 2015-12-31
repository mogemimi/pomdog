// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameHost.hpp"
#include <memory>

namespace Pomdog {

class EventQueue;
class Game;
struct PresentationParameters;

namespace Detail {
namespace InputSystem {

class InputDeviceFactory;

} // namespace InputSystem
namespace Win32 {

class GameWindowWin32;

class GameHostWin32 final : public GameHost {
public:
    GameHostWin32(
        std::shared_ptr<GameWindowWin32> const& window,
        std::shared_ptr<EventQueue> const& eventQueue,
        PresentationParameters const& presentationParameters,
        std::unique_ptr<InputSystem::InputDeviceFactory> && inputDeviceFactory,
        bool useOpenGL);

    ~GameHostWin32();

    void Run(Game & game);

    void Exit() override;

    std::shared_ptr<Pomdog::GameWindow> Window() override;

    std::shared_ptr<Pomdog::GameClock> Clock() override;

    std::shared_ptr<Pomdog::GraphicsCommandQueue> GraphicsCommandQueue() override;

    std::shared_ptr<Pomdog::GraphicsDevice> GraphicsDevice() override;

    std::shared_ptr<Pomdog::AudioEngine> AudioEngine() override;

    std::shared_ptr<Pomdog::AssetManager> AssetManager() override;

    std::shared_ptr<Pomdog::Keyboard> Keyboard() override;

    std::shared_ptr<Pomdog::Mouse> Mouse() override;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Win32
} // namespace Detail
} // namespace Pomdog
