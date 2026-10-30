// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <MetalKit/MTKView.h>
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
} // namespace pomdog::gpu

namespace pomdog::detail {
class DiagnosticQueue;
class SystemEventQueue;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {
class GameWindowCocoa;
} // namespace pomdog::detail::cocoa

namespace pomdog::detail::cocoa {

/// GameHostMetal is the macOS (Cocoa + Metal) implementation of GameHost.
/// Drives the game loop via MTKView's per-frame callback, manages the Metal
/// graphics device, audio, input, and the system event queue.
class GameHostMetal : public GameHost {
public:
    GameHostMetal();

    ~GameHostMetal() override;

    /// Creates the host.
    ///
    /// Subsystems report failures that they handle on their own to
    /// `diagnostics`, which must not be null. The host emits them to
    /// `onDiagnostic` once per frame on the game loop thread; `onDiagnostic`
    /// may be empty, which discards them.
    [[nodiscard]] static std::tuple<std::shared_ptr<GameHostMetal>, std::unique_ptr<Error>>
    create(
        MTKView* metalView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<SystemEventQueue>& eventQueue,
        const gpu::PresentationParameters& presentationParameters,
        const GameHostOptions& options,
        std::shared_ptr<DiagnosticQueue> diagnostics,
        std::function<void(std::unique_ptr<Error> diagnostic)> onDiagnostic);

    [[nodiscard]] virtual std::unique_ptr<Error>
    initializeGame(
        const std::weak_ptr<Game>& game,
        const std::function<void()>& onCompleted) = 0;

    virtual void
    gameLoop() = 0;

    [[nodiscard]] virtual bool
    isMetalSupported() const noexcept = 0;
};

} // namespace pomdog::detail::cocoa
