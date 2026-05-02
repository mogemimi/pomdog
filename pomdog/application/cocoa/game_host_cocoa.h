// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

@class PomdogOpenGLView;

namespace pomdog {
class Error;
class Game;
struct GameHostOptions;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

namespace pomdog::detail {
class SystemEventQueue;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {
class GameWindowCocoa;
} // namespace pomdog::detail::cocoa

namespace pomdog::detail::cocoa {

/// GameHostCocoa is the macOS (Cocoa + OpenGL) implementation of GameHost.
/// Manages the game loop using a CVDisplayLink-driven render callback,
/// OpenGL context, audio, input, and the system event queue.
class GameHostCocoa : public GameHost {
public:
    GameHostCocoa();

    ~GameHostCocoa() override;

    [[nodiscard]] static std::tuple<std::shared_ptr<GameHostCocoa>, std::unique_ptr<Error>>
    create(
        PomdogOpenGLView* openGLView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<SystemEventQueue>& eventQueue,
        const gpu::PresentationParameters& presentationParameters,
        const GameHostOptions& options);

    [[nodiscard]] virtual std::unique_ptr<Error>
    run(
        const std::weak_ptr<Game>& game,
        std::function<void()>&& onCompleted) = 0;
};

} // namespace pomdog::detail::cocoa
