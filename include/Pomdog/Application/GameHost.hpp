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

class POMDOG_EXPORT GameHost : public std::enable_shared_from_this<GameHost> {
public:
    GameHost() = default;
    GameHost(const GameHost&) = delete;
    GameHost & operator=(const GameHost&) = delete;

    virtual ~GameHost() = default;

    virtual void Exit() = 0;

    virtual std::shared_ptr<GameWindow> GetWindow() = 0;

    virtual std::shared_ptr<GameClock> GetClock() = 0;

    virtual std::shared_ptr<GraphicsDevice> GetGraphicsDevice() = 0;

    virtual std::shared_ptr<GraphicsCommandQueue> GetGraphicsCommandQueue() = 0;

    virtual std::shared_ptr<AudioEngine> GetAudioEngine() = 0;

    virtual std::shared_ptr<AssetManager> GetAssetManager() = 0;

    virtual std::shared_ptr<Keyboard> GetKeyboard() = 0;

    virtual std::shared_ptr<Mouse> GetMouse() = 0;

    virtual std::shared_ptr<Gamepad> GetGamepad() = 0;
};

} // namespace Pomdog
