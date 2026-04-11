// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/application/system_events.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/signals/event_queue.h"
#include "pomdog/signals/scoped_connection.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class Game;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

namespace pomdog::detail {
class GameClockImpl;
class TimeSource;
} // namespace pomdog::detail

namespace pomdog::gpu::detail::gl4 {
class GraphicsContextGL4;
class GraphicsDeviceGL4;
} // namespace pomdog::gpu::detail::gl4

namespace pomdog::gpu::detail {
class CommandQueueImmediate;
} // namespace pomdog::gpu::detail

namespace pomdog::detail::openal {
class AudioEngineAL;
} // namespace pomdog::detail::openal

namespace pomdog::detail {
class KeyboardImpl;
class MouseImpl;
} // namespace pomdog::detail

namespace pomdog::detail::emscripten {
class GameWindowEmscripten;
class OpenGLContextEmscripten;
class KeyboardEmscripten;
class MouseEmscripten;
class GamepadServiceEmscripten;
class TouchscreenEmscripten;
} // namespace pomdog::detail::emscripten

namespace pomdog::detail::emscripten {

class GameHostEmscripten final : public GameHost {
public:
    GameHostEmscripten() noexcept;

    ~GameHostEmscripten() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const gpu::PresentationParameters& presentationParameters,
        const std::string& targetCanvas);

    void run(Game& game, int argc, const char* const* argv);

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

    [[nodiscard]] std::shared_ptr<Keyboard>
    getKeyboard() noexcept override;

    [[nodiscard]] std::shared_ptr<Mouse>
    getMouse() noexcept override;

    [[nodiscard]] std::shared_ptr<Gamepad>
    getGamepad() noexcept override;

    [[nodiscard]] std::shared_ptr<GamepadService>
    getGamepadService() noexcept override;

    [[nodiscard]] std::shared_ptr<Touchscreen>
    getTouchscreen() noexcept override;

    [[nodiscard]] std::shared_ptr<IOService>
    getIOService() noexcept override;

    [[nodiscard]] std::shared_ptr<HTTPClient>
    getHTTPClient() noexcept override;

    void renderFrame();

private:
    void processSystemEvents(const SystemEvent& event);

    std::shared_ptr<EventQueue<SystemEvent>> eventQueue_;
    ScopedConnection systemEventConnection_;
    std::shared_ptr<TimeSource> timeSource_;
    std::shared_ptr<GameClockImpl> clock_;
    std::shared_ptr<GameWindowEmscripten> window_;
    std::shared_ptr<OpenGLContextEmscripten> openGLContext_;
    std::shared_ptr<gpu::detail::gl4::GraphicsDeviceGL4> graphicsDevice_;
    std::shared_ptr<gpu::detail::gl4::GraphicsContextGL4> graphicsContext_;
    std::shared_ptr<gpu::detail::CommandQueueImmediate> commandQueue_;
    std::shared_ptr<openal::AudioEngineAL> audioEngine_;
    std::shared_ptr<KeyboardImpl> keyboardImpl_;
    std::shared_ptr<KeyboardEmscripten> keyboard_;
    std::shared_ptr<MouseImpl> mouseImpl_;
    std::unique_ptr<MouseEmscripten> mouse_;
    std::shared_ptr<GamepadServiceEmscripten> gamepad_;
    std::shared_ptr<TouchscreenEmscripten> touchscreen_;
    unsafe_ptr<Game> game_ = nullptr;
    Duration presentationInterval_ = Duration::zero();
    bool exitRequest_ = false;
};

} // namespace pomdog::detail::emscripten
