// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/basic/win32/windows_headers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class Game;
struct GameHostOptions;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
enum class GraphicsBackend : u8;
} // namespace pomdog::gpu

namespace pomdog::detail {
class SystemEventQueue;
} // namespace pomdog::detail

namespace pomdog::detail::win32 {
class GameWindowWin32;
} // namespace pomdog::detail::win32

namespace pomdog::detail::win32 {

/// GameHostWin32 is the Windows implementation of GameHost.
/// Runs a fixed-timestep game loop driven by Win32 message pump, supporting
/// Direct3D 11, OpenGL 4, and Vulkan graphics backends together with XAudio2
/// audio, DirectInput gamepads, keyboard, mouse, and optional networking.
class GameHostWin32 : public GameHost {
public:
    GameHostWin32();

    ~GameHostWin32() override;

    /// Runs the game loop until exit() is requested or rendering cannot
    /// continue.
    ///
    /// Returns an error when the loop stopped because rendering cannot
    /// continue, such as when the back buffers cannot be recreated or the
    /// device is lost.
    [[nodiscard]] virtual std::unique_ptr<Error>
    run(Game& game) = 0;

    /// Creates the host.
    ///
    /// `onDiagnostic` receives failures that a subsystem already handled,
    /// such as a gamepad that could not be opened. It is called on the main
    /// thread between frames and may be empty.
    [[nodiscard]] static std::tuple<std::shared_ptr<GameHostWin32>, std::unique_ptr<Error>>
    create(
        const std::shared_ptr<GameWindowWin32>& window,
        HINSTANCE hInstance,
        const std::shared_ptr<SystemEventQueue>& eventQueue,
        const gpu::PresentationParameters& presentationParameters,
        const GameHostOptions& options,
        std::function<void(std::unique_ptr<Error> diagnostic)> onDiagnostic) noexcept;
};

} // namespace pomdog::detail::win32
