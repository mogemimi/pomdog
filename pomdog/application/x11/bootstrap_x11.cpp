// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/x11/bootstrap_x11.h"
#include "pomdog/application/game.h"
#include "pomdog/application/game_host_options.h"
#include "pomdog/application/game_setup.h"
#include "pomdog/application/linux/game_host_linux.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/diagnostic_queue.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::x11 {

void Bootstrap::setCommandLineArgs(int argc, const char* const* argv) noexcept
{
    argc_ = argc;
    argv_ = argv;
}

void Bootstrap::onError(std::function<void(std::unique_ptr<Error> err)> onErrorIn)
{
    onError_ = std::move(onErrorIn);
}

void Bootstrap::onDiagnostic(std::function<void(std::unique_ptr<Error> diagnostic)> onDiagnosticIn)
{
    onDiagnostic_ = std::move(onDiagnosticIn);
}

void Bootstrap::run(std::unique_ptr<GameSetup>&& gameSetup)
{
    if (gameSetup == nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::make("GameSetup must be != nullptr"));
        }
        return;
    }

    // NOTE: Set up default options with Linux defaults.
    GameHostOptions options = {};
    options.graphicsBackend = gpu::GraphicsBackend::OpenGL4;

    // NOTE: Let the GameSetup configure options.
    if (auto err = gameSetup->configure(options, std::span<const char* const>(argv_, static_cast<std::size_t>(argc_))); err != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(err), "failed to configure GameSetup"));
        }
        return;
    }

    // NOTE: Validate the configured options.
    if (options.maxFramesPerSecond.has_value() && options.maxFramesPerSecond.value() <= 0) {
        if (onError_ != nullptr) {
            onError_(errors::make("maxFramesPerSecond must be > 0"));
        }
        return;
    }
    // NOTE: Negated `>` instead of `<= 0` so that NaN is also rejected.
    if (!(options.highDPI.maxPixelRatio > 0.0f)) {
        if (onError_ != nullptr) {
            onError_(errors::make("highDPI.maxPixelRatio must be > 0"));
        }
        return;
    }
    if (options.clientWidth <= 0 || options.clientHeight <= 0) {
        if (onError_ != nullptr) {
            onError_(errors::make("client area size must be > 0"));
        }
        return;
    }
    if (options.windowMode == WindowMode::BrowserSoftFullscreen) {
        if (onError_ != nullptr) {
            onError_(errors::make("BrowserSoftFullscreen mode is not supported on Linux"));
        }
        return;
    }

    gpu::PresentationParameters presentationParameters = {};
    // NOTE: Linux/X11 currently reports `pixelRatio = 1.0` (no HiDPI scaling).
    // The initial back buffer matches the requested logical client size.
    presentationParameters.backBufferHeight = options.clientHeight;
    presentationParameters.backBufferWidth = options.clientWidth;
    presentationParameters.backBufferFormat = options.surfaceFormat;
    presentationParameters.depthStencilFormat = options.depthFormat;
    presentationParameters.multiSampleCount = options.multiSampleCount;

    // NOTE: The queue outlives the host so that diagnostics reported during a
    // failed startup or while the host shuts down are still delivered, before
    // the final error.
    auto diagnostics = std::make_shared<pomdog::detail::DiagnosticQueue>();
    const auto emitDiagnostics = [this, &diagnostics] {
        diagnostics->emit([this](std::unique_ptr<Error> diagnostic) {
            if (onDiagnostic_ != nullptr) {
                onDiagnostic_(std::move(diagnostic));
            }
        });
    };

    auto [gameHost, hostErr] = pomdog::detail::linux::GameHostLinux::create(
        presentationParameters,
        options,
        diagnostics,
        onDiagnostic_);
    if (hostErr != nullptr) {
        emitDiagnostics();
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(hostErr), "GameHostLinux::create() failed"));
        }
        return;
    }

    auto game = gameSetup->createGame();
    if (game == nullptr) {
        emitDiagnostics();
        if (onError_ != nullptr) {
            onError_(errors::make("GameSetup::createGame() returned nullptr"));
        }
        return;
    }

    // NOTE: GameSetup is no longer needed after createGame(); destroy it now.
    gameSetup = nullptr;

    if (auto err = game->initialize(gameHost); err != nullptr) {
        emitDiagnostics();
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(err), "failed to initialize game"));
        }
        return;
    }

    gameHost->run(*game);

    game = nullptr;
    gameHost = nullptr;
    emitDiagnostics();
}

} // namespace pomdog::x11
