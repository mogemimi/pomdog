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

void Bootstrap::setCommandLineArgs(int argc, const char* const* argv) noexcept
{
    argc_ = argc;
    argv_ = argv;
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

    // NOTE: Browser builds can provide launch arguments through Module.arguments.
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
    if (options.clientWidth <= 0 || options.clientHeight <= 0) {
        if (onError_ != nullptr) {
            onError_(errors::make("client area size must be > 0"));
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
    // NOTE: The GameWindowEmscripten samples `window.devicePixelRatio` after
    // construction and the GameHost commits the precise physical back-buffer
    // size at the first frame boundary. We seed the parameters with the
    // logical client size here; the host updates them to physical pixels
    // before the GraphicsDevice is created below.
    presentationParameters.backBufferHeight = options.clientHeight;
    presentationParameters.backBufferWidth = options.clientWidth;
    presentationParameters.backBufferFormat = options.surfaceFormat;
    presentationParameters.depthStencilFormat = options.depthFormat;
    presentationParameters.multiSampleCount = options.multiSampleCount;
    presentationParameters.windowMode = options.windowMode;

    auto [gameHost, hostErr] = pomdog::detail::emscripten::GameHostEmscripten::create(
        presentationParameters, options, targetCanvas_);
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
