// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class AssetManager;
class AudioEngine;
class GameClock;
class Gamepad;
class GameWindow;
class HTTPClient;
class IOService;
class Keyboard;
class Mouse;
} // namespace pomdog

namespace pomdog::gpu {
class CommandQueue;
class GraphicsDevice;
} // namespace pomdog::gpu

namespace pomdog {

/// Base interface for platform-specific hosts, thats conduct the game lifecycle.
///
/// Instances of this class are unique.
class POMDOG_EXPORT GameHost : public std::enable_shared_from_this<GameHost> {
public:
    /// Constructs GameHost with platform-default components.
    GameHost();
    GameHost(const GameHost&) = delete;
    GameHost& operator=(const GameHost&) = delete;

    virtual ~GameHost();

    /// Send signal to the GameHost to exit main loop and stop running the game.
    virtual void exit() = 0;

    /// @return Associated to this host platform-specific GameWindow.
    [[nodiscard]] virtual std::shared_ptr<GameWindow>
    getWindow() noexcept = 0;

    /// @return Associated to this host GameClock.
    [[nodiscard]] virtual std::shared_ptr<GameClock>
    getClock() noexcept = 0;

    /// @return Associated to this host GraphicsDevice with selected video backend.
    [[nodiscard]] virtual std::shared_ptr<gpu::GraphicsDevice>
    getGraphicsDevice() noexcept = 0;

    /// @return Associated to this host CommandQueue.
    [[nodiscard]] virtual std::shared_ptr<gpu::CommandQueue>
    getCommandQueue() noexcept = 0;

    /// @return Associated to this host AudioEngine with selected audio backend.
    [[nodiscard]] virtual std::shared_ptr<AudioEngine>
    getAudioEngine() noexcept = 0;

    /// @return Associated to this host AssetManager.
    [[nodiscard]] virtual std::shared_ptr<AssetManager>
    getAssetManager() noexcept = 0;

    /// @return Associated to this host Keyboard.
    [[nodiscard]] virtual std::shared_ptr<Keyboard>
    getKeyboard() noexcept = 0;

    /// @return Associated to this host Mouse.
    [[nodiscard]] virtual std::shared_ptr<Mouse>
    getMouse() noexcept = 0;

    /// @return Associated to this host Gamepad.
    [[nodiscard]] virtual std::shared_ptr<Gamepad>
    getGamepad() noexcept = 0;

    /// @return Associated to this host IOService.
    [[nodiscard]] virtual std::shared_ptr<IOService>
    getIOService() noexcept = 0;

    /// @return Associated to this host HTTPClient.
    [[nodiscard]] virtual std::shared_ptr<HTTPClient>
    getHTTPClient() noexcept = 0;
};

} // namespace pomdog
