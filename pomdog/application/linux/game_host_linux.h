// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
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

namespace pomdog::detail::linux {

/// GameHostLinux is the Linux (X11 + OpenGL 4) implementation of GameHost.
/// Drives a fixed-timestep game loop with a native X11 message pump,
/// OpenAL audio, evdev/udev gamepad support, and optional IOService networking.
class GameHostLinux : public GameHost {
public:
    GameHostLinux() noexcept;

    ~GameHostLinux() override;

    virtual void
    run(Game& game) = 0;

    [[nodiscard]] static std::tuple<std::shared_ptr<GameHostLinux>, std::unique_ptr<Error>>
    create(const gpu::PresentationParameters& presentationParameters,
        const GameHostOptions& options) noexcept;
};

} // namespace pomdog::detail::linux
