// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/application/game_host_options.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class Game;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

namespace pomdog::detail {
class DiagnosticQueue;
} // namespace pomdog::detail

namespace pomdog::detail::emscripten {

/// GameHostEmscripten is the Emscripten/WebAssembly implementation of GameHost.
/// Uses emscripten_set_main_loop_arg for a requestAnimationFrame-based render
/// loop with WebGL (OpenGL ES 2/3 via GL4 backend), WebAudio, and canvas-based
/// input.
class GameHostEmscripten : public GameHost {
public:
    GameHostEmscripten() noexcept;

    ~GameHostEmscripten() override;

    /// Initializes the game and hands the main loop to the browser.
    ///
    /// Returns an error when Game::initialize() fails. On success, this
    /// function does not return.
    [[nodiscard]] virtual std::unique_ptr<Error>
    run(Game& game) = 0;

    /// Creates the host.
    ///
    /// Subsystems report failures that they handle on their own to
    /// `diagnostics`, which must not be null. The host emits them to
    /// `onDiagnostic` once per frame; `onDiagnostic` may be empty, which
    /// discards them.
    [[nodiscard]] static std::tuple<std::shared_ptr<GameHostEmscripten>, std::unique_ptr<Error>>
    create(
        const gpu::PresentationParameters& presentationParameters,
        const GameHostOptions& options,
        const std::string& targetCanvas,
        std::shared_ptr<DiagnosticQueue> diagnostics,
        std::function<void(std::unique_ptr<Error> diagnostic)> onDiagnostic) noexcept;
};

} // namespace pomdog::detail::emscripten
