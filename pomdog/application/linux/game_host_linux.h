// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Game;
class Error;
struct GameHostOptions;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

namespace pomdog::detail {
class DiagnosticQueue;
} // namespace pomdog::detail

namespace pomdog::detail::linux {

/// GameHostLinux is the Linux (X11 + OpenGL 4) implementation of GameHost.
/// Drives a fixed-timestep game loop with a native X11 message pump,
/// OpenAL audio, and evdev/udev gamepad support.
class GameHostLinux : public GameHost {
public:
    GameHostLinux() noexcept;

    ~GameHostLinux() override;

    virtual void
    run(Game& game) = 0;

    /// Creates the host.
    ///
    /// Subsystems report failures that they handle on their own to
    /// `diagnostics`, which must not be null. The host emits them to
    /// `onDiagnostic` once per frame on the game loop thread; `onDiagnostic`
    /// may be empty, which discards them.
    [[nodiscard]] static std::tuple<std::shared_ptr<GameHostLinux>, std::unique_ptr<Error>>
    create(const gpu::PresentationParameters& presentationParameters,
        const GameHostOptions& options,
        std::shared_ptr<DiagnosticQueue> diagnostics,
        std::function<void(std::unique_ptr<Error> diagnostic)> onDiagnostic) noexcept;
};

} // namespace pomdog::detail::linux
