// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class AudioEngine;
class GameClock;
class Gamepad;
class GamepadService;
class GameWindow;
class HTTPClient;
class IOService;
class Keyboard;
class Mouse;
class Touchscreen;
} // namespace pomdog

namespace pomdog::gpu {
class CommandQueue;
class GraphicsDevice;
enum class PresentMode : u8;
} // namespace pomdog::gpu

namespace pomdog {

/// Base interface for platform-specific hosts that run the game lifecycle.
///
/// The host owns the platform window, the main loop, and every engine
/// subsystem (graphics, audio, input, networking). Application code never
/// creates a GameHost directly; the platform Bootstrap creates one and hands
/// it to `Game::initialize()`.
///
/// Notes for application developers:
/// - The getters below return handles to subsystems owned by the host. It is
///   safe to store them in your Game for its whole lifetime. Subsystems that
///   were disabled via `GameHostOptions`, or that the platform does not
///   provide, are returned as nullptr; check before use when you changed the
///   defaults or target multiple platforms.
/// - Call all members from the thread that runs the game loop (the thread
///   that calls `Game::update()` / `Game::draw()`).
/// - `request...()` / `set...()` members are deferred: they take effect at
///   the next frame boundary so that update/draw within one frame observe a
///   consistent state.
///
/// Instances of this class are unique.
class POMDOG_EXPORT GameHost : public std::enable_shared_from_this<GameHost> {
public:
    /// Creates a GameHost with platform-default components.
    GameHost();

    GameHost(const GameHost&) = delete;
    GameHost& operator=(const GameHost&) = delete;

    virtual ~GameHost();

    /// Requests that the host exit the main loop.
    ///
    /// The request takes effect at a frame boundary rather than immediately,
    /// so it is safe to call from update() or draw(); at most the remainder
    /// of the current frame runs before the loop stops.
    virtual void
    exit() = 0;

    /// Returns the window the game renders into. Never null.
    ///
    /// Use it to change the title, client size, or window mode, and to
    /// observe size, DPI, and window-mode changes.
    [[nodiscard]] virtual std::shared_ptr<GameWindow>
    getWindow() noexcept = 0;

    /// Returns the clock that tracks total game time, frame duration, and
    /// frame rate of the main loop. Never null.
    [[nodiscard]] virtual std::shared_ptr<GameClock>
    getClock() noexcept = 0;

    /// Returns the graphics device of the selected backend, used to create
    /// GPU resources such as buffers, textures, and pipeline states. Never
    /// null; bootstrapping fails when no graphics backend is available.
    [[nodiscard]] virtual std::shared_ptr<gpu::GraphicsDevice>
    getGraphicsDevice() noexcept = 0;

    /// Returns the command queue used to submit command lists and present
    /// the back buffer. Never null.
    [[nodiscard]] virtual std::shared_ptr<gpu::CommandQueue>
    getCommandQueue() noexcept = 0;

    /// Returns the audio engine, or nullptr when audio is disabled
    /// (`GameHostOptions::enableAudio` is false).
    [[nodiscard]] virtual std::shared_ptr<AudioEngine>
    getAudioEngine() noexcept = 0;

    /// Returns the keyboard input device, or nullptr in headless mode
    /// (`GameHostOptions::headless` is true).
    [[nodiscard]] virtual std::shared_ptr<Keyboard>
    getKeyboard() noexcept = 0;

    /// Returns the mouse input device, or nullptr in headless mode
    /// (`GameHostOptions::headless` is true).
    ///
    /// On the web (Emscripten) backend, mouse events are captured from the
    /// target canvas element only.
    [[nodiscard]] virtual std::shared_ptr<Mouse>
    getMouse() noexcept = 0;

    /// Returns the gamepad assigned to player one, or nullptr when the
    /// gamepad subsystem is disabled (`GameHostOptions::enableGamepad` is
    /// false, or headless mode).
    ///
    /// The returned instance stays valid while no physical controller is
    /// connected; use its capabilities/state queries to detect connection.
    [[nodiscard]] virtual std::shared_ptr<Gamepad>
    getGamepad() noexcept = 0;

    /// Returns the gamepad service that manages gamepads for all player
    /// indices, or nullptr when the gamepad subsystem is disabled
    /// (`GameHostOptions::enableGamepad` is false, or headless mode).
    [[nodiscard]] virtual std::shared_ptr<GamepadService>
    getGamepadService() noexcept = 0;

    /// Returns the touchscreen input device, or nullptr on platforms without
    /// touch input.
    ///
    /// Currently only the web (Emscripten) backend provides a touchscreen;
    /// the Windows, macOS, and Linux desktop backends return nullptr.
    [[nodiscard]] virtual std::shared_ptr<Touchscreen>
    getTouchscreen() noexcept = 0;

    /// Returns the I/O service for asynchronous networking, or nullptr when
    /// networking is disabled (`GameHostOptions::enableNetwork` is false) or
    /// the platform does not support it (the web backend).
    [[nodiscard]] virtual std::shared_ptr<IOService>
    getIOService() noexcept = 0;

    /// Returns the HTTP client, or nullptr when networking is disabled
    /// (`GameHostOptions::enableNetwork` is false) or the platform does not
    /// support it (the web backend).
    [[nodiscard]] virtual std::shared_ptr<HTTPClient>
    getHTTPClient() noexcept = 0;

    /// Returns the current frame-rate cap in frames per second, or
    /// std::nullopt when no explicit cap is set (run as fast as the graphics
    /// backend and V-Sync allow).
    [[nodiscard]] virtual std::optional<i32>
    getMaxFramesPerSecond() const noexcept = 0;

    /// Sets the maximum frames per second setting (the frame-rate cap).
    ///
    /// Pass std::nullopt to remove any explicit FPS cap. The cap and the V-Sync
    /// mode (`getPresentMode()`) are independent settings; when both are active,
    /// the effective frame rate is the lower of the two. In particular, setting a
    /// cap equal to or close to the display's refresh rate while V-Sync is enabled
    /// can cause double pacing (occasional frame drops) because both mechanisms
    /// independently consume a frame's worth of wait time. Leave the cap unset
    /// (nullopt, the default) when relying on V-Sync to pace at the display's
    /// native rate, and use an explicit cap only to limit below the display rate
    /// (e.g. 30 fps for a battery-saving mode) or when V-Sync is disabled.
    /// The change is deferred and applied at the next frame boundary so that
    /// update/draw within the current frame see a consistent setting.
    ///
    /// On the web (Emscripten) backend the cap is approximated by skipping
    /// animation frames, so the effective rate is the display refresh rate
    /// divided by an integer (e.g. 30 fps on a 60 Hz display, 40 fps on a
    /// 120 Hz display, for a requested cap of 30).
    virtual void
    setMaxFramesPerSecond(std::optional<i32> maxFPS) noexcept = 0;

    /// Returns the effective present mode currently applied.
    ///
    /// This is the mode that was actually applied by the graphics backend,
    /// which may differ from a previously requested mode when that mode is not
    /// supported (e.g. `Adaptive` and `Mailbox` fall back to `VSync` on Direct3D 11).
    [[nodiscard]] virtual gpu::PresentMode
    getPresentMode() const noexcept = 0;

    /// Requests a present mode.
    ///
    /// The request is deferred and applied at the next frame boundary so that
    /// update/draw within the current frame see a consistent setting. The
    /// platform may fall back to a different mode when the requested one is not
    /// supported; observe the result via `getPresentMode()`.
    ///
    /// On the web (Emscripten) backend the browser always paces rendering via
    /// `requestAnimationFrame`, so the mode is fixed to `VSync` and requests
    /// are ignored.
    virtual void
    requestPresentMode(gpu::PresentMode mode) noexcept = 0;
};

} // namespace pomdog
