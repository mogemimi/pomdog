// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEHOST_FDAA974D_DBD8_44E8_84CD_F6FAA243957B_HPP
#define POMDOG_GAMEHOST_FDAA974D_DBD8_44E8_84CD_F6FAA243957B_HPP

#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class AssetManager;
class AudioEngine;
class Game;
class GameClock;
class GameWindow;
class GraphicsContext;
class GraphicsDevice;
class Keyboard;
class Mouse;

class POMDOG_EXPORT GameHost: public std::enable_shared_from_this<GameHost> {
public:
    GameHost() = default;
    GameHost(GameHost const&) = delete;
    GameHost & operator=(GameHost const&) = delete;

    virtual ~GameHost() = default;

    virtual void Run(Game & game) = 0;

    virtual void Exit() = 0;

    virtual std::shared_ptr<GameWindow> Window() = 0;

    virtual std::shared_ptr<GameClock> Clock() = 0;

    virtual std::shared_ptr<GraphicsContext> GraphicsContext() = 0;

    virtual std::shared_ptr<GraphicsDevice> GraphicsDevice() = 0;

    virtual std::shared_ptr<AudioEngine> AudioEngine() = 0;

    virtual std::shared_ptr<AssetManager> AssetManager() = 0;

    virtual std::shared_ptr<Keyboard> Keyboard() = 0;

    virtual std::shared_ptr<Mouse> Mouse() = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEHOST_FDAA974D_DBD8_44E8_84CD_F6FAA243957B_HPP)
