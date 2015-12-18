// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_GAMEHOST_FDAA974D_HPP
#define POMDOG_GAMEHOST_FDAA974D_HPP

#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class AssetManager;
class AudioEngine;
class GameClock;
class GameWindow;
class GraphicsCommandQueue;
class GraphicsDevice;
class Keyboard;
class Mouse;

class POMDOG_EXPORT GameHost : public std::enable_shared_from_this<GameHost> {
public:
    GameHost() = default;
    GameHost(GameHost const&) = delete;
    GameHost & operator=(GameHost const&) = delete;

    virtual ~GameHost() = default;

    virtual void Exit() = 0;

    virtual std::shared_ptr<GameWindow> Window() = 0;

    virtual std::shared_ptr<GameClock> Clock() = 0;

    virtual std::shared_ptr<GraphicsDevice> GraphicsDevice() = 0;

    virtual std::shared_ptr<GraphicsCommandQueue> GraphicsCommandQueue() = 0;

    virtual std::shared_ptr<AudioEngine> AudioEngine() = 0;

    virtual std::shared_ptr<AssetManager> AssetManager() = 0;

    virtual std::shared_ptr<Keyboard> Keyboard() = 0;

    virtual std::shared_ptr<Mouse> Mouse() = 0;
};

} // namespace Pomdog

#endif // POMDOG_GAMEHOST_FDAA974D_HPP
