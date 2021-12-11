// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.hpp"
#include "pomdog/chrono/duration.hpp"
#include "pomdog/chrono/game_clock.hpp"
#include "pomdog/graphics/presentation_parameters.hpp"
#include "pomdog/platform/x11/game_window_x11.hpp"
#include "pomdog/platform/x11/keyboard_x11.hpp"
#include "pomdog/platform/x11/mouse_x11.hpp"
#include "pomdog/platform/x11/opengl_context_x11.hpp"
#include "pomdog/platform/x11/x11_context.hpp"
#include "pomdog/utility/errors.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <memory>

namespace pomdog {
class Game;
} // namespace pomdog

namespace pomdog::detail::x11 {

class GameHostX11 final : public GameHost {
public:
    GameHostX11() noexcept;

    ~GameHostX11() override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const PresentationParameters& presentationParameters);

    void Run(Game& game);

    void Exit() override;

    [[nodiscard]] std::shared_ptr<GameWindow>
    GetWindow() noexcept override;

    [[nodiscard]] std::shared_ptr<GameClock>
    GetClock() noexcept override;

    [[nodiscard]] std::shared_ptr<GraphicsDevice>
    GetGraphicsDevice() noexcept override;

    [[nodiscard]] std::shared_ptr<GraphicsCommandQueue>
    GetGraphicsCommandQueue() noexcept override;

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

} // namespace pomdog::detail::x11
