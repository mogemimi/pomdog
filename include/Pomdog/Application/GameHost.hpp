// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class AssetManager;
class AudioEngine;
class GameClock;
class Gamepad;
class GameWindow;
class GraphicsCommandQueue;
class GraphicsDevice;
class Keyboard;
class Mouse;

/// Base interface for platform-specific hosts, thats conduct the game lifecycle.
///
/// Instances of this class are unique.
class POMDOG_EXPORT GameHost : public std::enable_shared_from_this<GameHost> {
public:
    /// Constructs GameHost with platform-default components.
    GameHost() = default;
    GameHost(const GameHost&) = delete;
    GameHost& operator=(const GameHost&) = delete;

    virtual ~GameHost() = default;

    /// Send signal to the GameHost to exit main loop and stop running the game.
    virtual void Exit() = 0;

    /// @return Associated to this host platform-specific GameWindow.
    virtual std::shared_ptr<GameWindow> GetWindow() = 0;

    /// @return Associated to this host GameClock.
    virtual std::shared_ptr<GameClock> GetClock() = 0;

    /// @return Associated to this host GraphicsDevice with selected video backend.
    virtual std::shared_ptr<GraphicsDevice> GetGraphicsDevice() = 0;

    /// @return Associated to this host GraphicsCommandQueue.
    virtual std::shared_ptr<GraphicsCommandQueue> GetGraphicsCommandQueue() = 0;

    /// @return Associated to this host AudioEngine with selected audio backend.
    virtual std::shared_ptr<AudioEngine> GetAudioEngine() = 0;

    /// @return Associated to this host AssetManager.
    virtual std::shared_ptr<AssetManager> GetAssetManager() = 0;

    /// @return Associated to this host Keyboard.
    virtual std::shared_ptr<Keyboard> GetKeyboard() = 0;

    /// @return Associated to this host Mouse.
    virtual std::shared_ptr<Mouse> GetMouse() = 0;

    /// @return Associated to this host Gamepad.
    virtual std::shared_ptr<Gamepad> GetGamepad() = 0;
};

} // namespace Pomdog
