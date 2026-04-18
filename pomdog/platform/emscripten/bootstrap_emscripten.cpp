// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/emscripten/bootstrap_emscripten.h"
#include "pomdog/application/emscripten/game_host_emscripten.h"
#include "pomdog/application/game.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::emscripten {

void Bootstrap::setSurfaceFormat(gpu::PixelFormat surfaceFormatIn) noexcept
{
    surfaceFormat_ = surfaceFormatIn;
}

void Bootstrap::setDepthFormat(gpu::PixelFormat depthFormatIn) noexcept
{
    depthFormat_ = depthFormatIn;
}

void Bootstrap::setPresentationInterval(int presentationIntervalIn) noexcept
{
    presentationInterval_ = presentationIntervalIn;
}

void Bootstrap::setBackBufferSize(int width, int height) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    backBufferWidth_ = width;
    backBufferHeight_ = height;
}

void Bootstrap::setFullScreen(bool isFullScreenIn) noexcept
{
    isFullScreen_ = isFullScreenIn;
}

void Bootstrap::setCommandLineArgs(int argc, const char* const* argv) noexcept
{
    argc_ = argc;
    argv_ = argv;
}

void Bootstrap::setTargetCanvas(const std::string& targetCanvas) noexcept
{
    targetCanvas_ = targetCanvas;
}

void Bootstrap::onError(std::function<void(std::unique_ptr<Error>&& err)> onErrorIn)
{
    onError_ = std::move(onErrorIn);
}

void Bootstrap::run(
    const std::function<std::unique_ptr<Game>()>& createApp)
{
    gpu::PresentationParameters presentationParameters;
    presentationParameters.backBufferHeight = backBufferHeight_;
    presentationParameters.backBufferWidth = backBufferWidth_;
    presentationParameters.presentationInterval = presentationInterval_;
    presentationParameters.backBufferFormat = surfaceFormat_;
    presentationParameters.depthStencilFormat = depthFormat_;
    presentationParameters.multiSampleCount = 0;
    presentationParameters.isFullScreen = isFullScreen_;

    auto [gameHost, hostErr] = pomdog::detail::emscripten::GameHostEmscripten::create(
        presentationParameters, targetCanvas_);
    if (hostErr != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(hostErr), "GameHostEmscripten::create() failed"));
        }
        return;
    }

    POMDOG_ASSERT(createApp);
    auto game = createApp();
    if (game == nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::make("game must be != nullptr"));
        }
        return;
    }

    POMDOG_ASSERT(game != nullptr);
    gameHost->run(*game, argc_, argv_);

    game.reset();
    gameHost.reset();
}

} // namespace pomdog::emscripten
