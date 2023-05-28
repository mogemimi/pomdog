// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/input/x11/mouse_x11.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Game;
class Error;
} // namespace pomdog

namespace pomdog::detail {
class GameClockImpl;
class TimeSource;
} // namespace pomdog::detail

namespace pomdog::gpu::detail {
class CommandQueueImmediate;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu::detail::gl4 {
class GraphicsContextGL4;
class GraphicsDeviceGL4;
} // namespace pomdog::gpu::detail::gl4

namespace pomdog::detail::linux {
class GamepadLinux;
} // namespace pomdog::detail::linux

namespace pomdog::detail::x11 {
class X11Context;
class KeyboardX11;
class MouseX11;
class OpenGLContextX11;
class GameWindowX11;
} // namespace pomdog::detail::x11

namespace pomdog::detail::openal {
class AudioEngineAL;
} // namespace pomdog::detail::openal

namespace pomdog::detail::linux {

class GameHostLinux final : public GameHost {
public:
    GameHostLinux() noexcept;

    ~GameHostLinux() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const gpu::PresentationParameters& presentationParameters);

    void run(Game& game);

    void exit() override;

    [[nodiscard]] std::shared_ptr<GameWindow>
    getWindow() noexcept override;

    [[nodiscard]] std::shared_ptr<GameClock>
    getClock() noexcept override;

    [[nodiscard]] std::shared_ptr<gpu::GraphicsDevice>
    getGraphicsDevice() noexcept override;

    [[nodiscard]] std::shared_ptr<gpu::CommandQueue>
    getCommandQueue() noexcept override;

    [[nodiscard]] std::shared_ptr<AudioEngine>
    getAudioEngine() noexcept override;

    [[nodiscard]] std::shared_ptr<AssetManager>
    getAssetManager() noexcept override;

    [[nodiscard]] std::shared_ptr<Keyboard>
    getKeyboard() noexcept override;

    [[nodiscard]] std::shared_ptr<Mouse>
    getMouse() noexcept override;

    [[nodiscard]] std::shared_ptr<Gamepad>
    getGamepad() noexcept override;

    [[nodiscard]] std::shared_ptr<IOService>
    getIOService() noexcept override;

    [[nodiscard]] std::shared_ptr<HTTPClient>
    getHTTPClient() noexcept override;

private:
    void messagePump();

    void processEvent(::XEvent& event);

    void renderFrame(Game& game);

private:
    std::shared_ptr<TimeSource> timeSource_;
    std::shared_ptr<GameClockImpl> clock_;
    std::shared_ptr<x11::X11Context> x11Context_;
    std::shared_ptr<x11::GameWindowX11> window_;
    std::shared_ptr<x11::OpenGLContextX11> openGLContext_;
    std::shared_ptr<gpu::detail::gl4::GraphicsDeviceGL4> graphicsDevice_;
    std::shared_ptr<gpu::detail::gl4::GraphicsContextGL4> graphicsContext_;
    std::shared_ptr<gpu::detail::CommandQueueImmediate> commandQueue_;
    std::shared_ptr<openal::AudioEngineAL> audioEngine_;
    std::shared_ptr<AssetManager> assetManager_;
    std::unique_ptr<x11::KeyboardX11> keyboard_;
    x11::MouseX11 mouse_;
    std::unique_ptr<GamepadLinux> gamepad_;
    std::unique_ptr<IOService> ioService_;
    std::unique_ptr<HTTPClient> httpClient_;
    Duration presentationInterval_;
    PixelFormat backBufferSurfaceFormat_;
    PixelFormat backBufferDepthStencilFormat_;
    bool exitRequest_ = false;
};

} // namespace pomdog::detail::linux
