// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/x11/bootstrap_x11.h"
#include "pomdog/application/game.h"
#include "pomdog/application/game_host_options.h"
#include "pomdog/application/game_setup.h"
#include "pomdog/application/linux/game_host_linux.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/assert.h"
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

void Bootstrap::onError(std::function<void(std::unique_ptr<Error>&& err)> onErrorIn)
{
    onError_ = std::move(onErrorIn);
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
    GameHostOptions options;
    options.graphicsBackend = gpu::GraphicsBackend::OpenGL4;

    // NOTE: Let the GameSetup configure options.
    if (auto err = gameSetup->configure(options, std::span<const char* const>(argv_, static_cast<std::size_t>(argc_))); err != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(err), "failed to configure GameSetup"));
        }
        return;
    }

    // NOTE: Validate the configured options.
    if (options.presentationInterval <= 0) {
        if (onError_ != nullptr) {
            onError_(errors::make("presentation interval must be > 0"));
        }
        return;
    }
    if (options.backBufferWidth <= 0 || options.backBufferHeight <= 0) {
        if (onError_ != nullptr) {
            onError_(errors::make("back buffer size must be > 0"));
        }
        return;
    }

    gpu::PresentationParameters presentationParameters;
    presentationParameters.backBufferHeight = options.backBufferHeight;
    presentationParameters.backBufferWidth = options.backBufferWidth;
    presentationParameters.presentationInterval = options.presentationInterval;
    presentationParameters.backBufferFormat = options.surfaceFormat;
    presentationParameters.depthStencilFormat = options.depthFormat;
    presentationParameters.multiSampleCount = options.multiSampleCount;
    presentationParameters.isFullScreen = options.isFullScreen;

    auto [gameHost, hostErr] = pomdog::detail::linux::GameHostLinux::create(presentationParameters, options);
    if (hostErr != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(hostErr), "GameHostLinux::create() failed"));
        }
        return;
    }

    auto game = gameSetup->createGame();
    if (game == nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::make("GameSetup::createGame() returned nullptr"));
        }
        return;
    }

    // NOTE: GameSetup is no longer needed after createGame(); destroy it now.
    gameSetup = nullptr;

    if (auto err = game->initialize(gameHost); err != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(err), "failed to initialize game"));
        }
        return;
    }

    gameHost->run(*game);

    game = nullptr;
    gameHost = nullptr;
}

} // namespace pomdog::x11
