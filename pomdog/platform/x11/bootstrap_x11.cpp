// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/x11/bootstrap_x11.h"
#include "pomdog/application/game.h"
#include "pomdog/application/linux/game_host_linux.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::x11 {

void Bootstrap::setSurfaceFormat(PixelFormat surfaceFormatIn) noexcept
{
    surfaceFormat_ = surfaceFormatIn;
}

void Bootstrap::setDepthFormat(PixelFormat depthFormatIn) noexcept
{
    depthFormat_ = depthFormatIn;
}

void Bootstrap::setPresentationInterval(int presentationIntervalIn) noexcept
{
    presentationInterval_ = presentationIntervalIn;
}

void Bootstrap::setBackBufferSize(int width, int height) noexcept
{
    POMDOG_ASSERT(width >= 0);
    POMDOG_ASSERT(height >= 0);
    backBufferWidth_ = width;
    backBufferHeight_ = height;
}

void Bootstrap::setFullScreen(bool isFullScreenIn) noexcept
{
    isFullScreen_ = isFullScreenIn;
}

void Bootstrap::onError(std::function<void(std::unique_ptr<Error>&& err)> onErrorIn)
{
    onError_ = std::move(onErrorIn);
}

void Bootstrap::run(
    const std::function<std::unique_ptr<Game>(const std::shared_ptr<GameHost>&)>& createApp)
{
    gpu::PresentationParameters presentationParameters;
    presentationParameters.backBufferHeight = backBufferHeight_;
    presentationParameters.backBufferWidth = backBufferWidth_;
    presentationParameters.presentationInterval = presentationInterval_;
    presentationParameters.backBufferFormat = surfaceFormat_;
    presentationParameters.depthStencilFormat = depthFormat_;
    presentationParameters.multiSampleCount = 1;
    presentationParameters.isFullScreen = isFullScreen_;

    auto gameHost = std::make_shared<pomdog::detail::linux::GameHostLinux>();
    if (auto err = gameHost->initialize(presentationParameters); err != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(err), "failed to initialize GameHostLinux"));
        }
        return;
    }

    POMDOG_ASSERT(createApp);
    auto game = createApp(gameHost);
    if (game == nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::make("game must be != nullptr"));
        }
        return;
    }

    POMDOG_ASSERT(game != nullptr);
    if (auto err = game->initialize(); err != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(err), "failed to initialize game"));
        }
        return;
    }

    gameHost->run(*game);

    game.reset();
    gameHost.reset();
}

} // namespace pomdog::x11
