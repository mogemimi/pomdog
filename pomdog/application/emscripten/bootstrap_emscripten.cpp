// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/emscripten/bootstrap_emscripten.h"
#include "pomdog/application/emscripten/game_host_emscripten.h"
#include "pomdog/application/game.h"
#include "pomdog/application/game_host.h"
#include "pomdog/application/game_host_options.h"
#include "pomdog/application/game_setup.h"
#include "pomdog/application/game_window.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::emscripten {

void Bootstrap::setTargetCanvas(const std::string& targetCanvas) noexcept
{
    targetCanvas_ = targetCanvas;
}

void Bootstrap::onError(std::function<void(std::unique_ptr<Error>&& err)> onErrorIn)
{
    onError_ = std::move(onErrorIn);
}

void Bootstrap::onWindowCreated(std::function<void(unsafe_ptr<GameWindow> window)> callback)
{
    onWindowCreated_ = std::move(callback);
}

void Bootstrap::run(std::unique_ptr<GameSetup>&& gameSetup)
{
    if (gameSetup == nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::make("GameSetup must be != nullptr"));
        }
        return;
    }

    // NOTE: Set up default options with Emscripten defaults.
    GameHostOptions options = {};
    options.graphicsBackend = gpu::GraphicsBackend::OpenGL4;
    options.surfaceFormat = gpu::PixelFormat::R8G8B8A8_UNorm;
    options.depthFormat = gpu::PixelFormat::Depth24Stencil8;

    // NOTE: Emscripten has no command-line arguments; pass an empty span.
    if (auto err = gameSetup->configure(options, std::span<const char* const>{}); err != nullptr) {
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
    if (options.windowMode == WindowMode::Maximized) {
        if (onError_ != nullptr) {
            onError_(errors::make("Maximized mode is not supported on Emscripten"));
        }
        return;
    }
    if (options.windowMode == WindowMode::BorderlessWindowed) {
        if (onError_ != nullptr) {
            onError_(errors::make("BorderlessWindowed mode is not supported on Emscripten"));
        }
        return;
    }

    gpu::PresentationParameters presentationParameters = {};
    presentationParameters.backBufferHeight = options.backBufferHeight;
    presentationParameters.backBufferWidth = options.backBufferWidth;
    presentationParameters.presentationInterval = options.presentationInterval;
    presentationParameters.backBufferFormat = options.surfaceFormat;
    presentationParameters.depthStencilFormat = options.depthFormat;
    presentationParameters.multiSampleCount = options.multiSampleCount;
    presentationParameters.windowMode = options.windowMode;

    auto [gameHost, hostErr] = pomdog::detail::emscripten::GameHostEmscripten::create(
        presentationParameters, targetCanvas_);
    if (hostErr != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(hostErr), "GameHostEmscripten::create() failed"));
        }
        return;
    }

    if (onWindowCreated_) {
        onWindowCreated_(gameHost->getWindow().get());
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

    // NOTE: run() also calls game->initialize() internally with GL context current.
    gameHost->run(*game);

    game = nullptr;
    gameHost = nullptr;
}

} // namespace pomdog::emscripten
