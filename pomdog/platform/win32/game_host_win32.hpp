// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/signals/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
class Error;
class Game;
struct PresentationParameters;
} // namespace Pomdog

namespace Pomdog::Detail {
class SystemEvent;
} // namespace Pomdog::Detail

namespace Pomdog::Detail {
class NativeGamepad;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Win32 {

class GameWindowWin32;

class GameHostWin32 final : public GameHost {
public:
    GameHostWin32();

    ~GameHostWin32();

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        const std::shared_ptr<GameWindowWin32>& window,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const std::shared_ptr<NativeGamepad>& gamepad,
        const PresentationParameters& presentationParameters,
        bool useOpenGL) noexcept;

    void Run(Game& game);

    void Exit() override;

    [[nodiscard]] std::shared_ptr<GameWindow>
    GetWindow() noexcept override;

    [[nodiscard]] std::shared_ptr<GameClock>
    GetClock() noexcept override;

    [[nodiscard]] std::shared_ptr<GraphicsCommandQueue>
    GetGraphicsCommandQueue() noexcept override;

    [[nodiscard]] std::shared_ptr<GraphicsDevice>
    GetGraphicsDevice() noexcept override;

    [[nodiscard]] std::shared_ptr<AudioEngine>
    GetAudioEngine() noexcept override;

    [[nodiscard]] std::shared_ptr<AssetManager>
    GetAssetManager() noexcept override;

    [[nodiscard]] std::shared_ptr<Keyboard>
    GetKeyboard() noexcept override;

    [[nodiscard]] std::shared_ptr<Mouse>
    GetMouse() noexcept override;

    [[nodiscard]] std::shared_ptr<Gamepad>
    GetGamepad() noexcept override;

    [[nodiscard]] std::shared_ptr<IOService>
    GetIOService() noexcept override;

    [[nodiscard]] std::shared_ptr<HTTPClient>
    GetHTTPClient() noexcept override;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog::Detail::Win32
