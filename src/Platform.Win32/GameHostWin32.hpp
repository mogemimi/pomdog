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

    std::shared_ptr<GameWindow> GetWindow() override;

    std::shared_ptr<GameClock> GetClock() override;

    std::shared_ptr<GraphicsCommandQueue> GetGraphicsCommandQueue() override;

    std::shared_ptr<GraphicsDevice> GetGraphicsDevice() override;

    std::shared_ptr<AudioEngine> GetAudioEngine() override;

    std::shared_ptr<AssetManager> GetAssetManager() override;

    std::shared_ptr<Keyboard> GetKeyboard() override;

    std::shared_ptr<Mouse> GetMouse() override;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Win32
} // namespace Detail
} // namespace Pomdog
