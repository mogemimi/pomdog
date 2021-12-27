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
class GraphicsCommandQueue;
class GraphicsDevice;
class HTTPClient;
class IOService;
class Keyboard;
class Mouse;

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
    virtual void Exit() = 0;

    /// @return Associated to this host platform-specific GameWindow.
    [[nodiscard]] virtual std::shared_ptr<GameWindow>
    GetWindow() noexcept = 0;

    /// @return Associated to this host GameClock.
    [[nodiscard]] virtual std::shared_ptr<GameClock>
    GetClock() noexcept = 0;

    /// @return Associated to this host GraphicsDevice with selected video backend.
    [[nodiscard]] virtual std::shared_ptr<GraphicsDevice>
    GetGraphicsDevice() noexcept = 0;

    /// @return Associated to this host GraphicsCommandQueue.
    [[nodiscard]] virtual std::shared_ptr<GraphicsCommandQueue>
    GetGraphicsCommandQueue() noexcept = 0;

    /// @return Associated to this host AudioEngine with selected audio backend.
    [[nodiscard]] virtual std::shared_ptr<AudioEngine>
    GetAudioEngine() noexcept = 0;

    /// @return Associated to this host AssetManager.
    [[nodiscard]] virtual std::shared_ptr<AssetManager>
    GetAssetManager() noexcept = 0;

    /// @return Associated to this host Keyboard.
    [[nodiscard]] virtual std::shared_ptr<Keyboard>
    GetKeyboard() noexcept = 0;

    /// @return Associated to this host Mouse.
    [[nodiscard]] virtual std::shared_ptr<Mouse>
    GetMouse() noexcept = 0;

    /// @return Associated to this host Gamepad.
    [[nodiscard]] virtual std::shared_ptr<Gamepad>
    GetGamepad() noexcept = 0;

    /// @return Associated to this host IOService.
    [[nodiscard]] virtual std::shared_ptr<IOService>
    GetIOService() noexcept = 0;

    /// @return Associated to this host HTTPClient.
    [[nodiscard]] virtual std::shared_ptr<HTTPClient>
    GetHTTPClient() noexcept = 0;
};

} // namespace pomdog
