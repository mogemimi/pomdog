// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/x11/bootstrap_x11.h"
#include "pomdog/application/game.h"
#include "pomdog/application/linux/game_host_linux.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/errors.h"
#include <utility>

namespace pomdog::x11 {

void Bootstrap::SetSurfaceFormat(PixelFormat surfaceFormatIn) noexcept
{
    surfaceFormat = surfaceFormatIn;
}

void Bootstrap::SetDepthFormat(PixelFormat depthFormatIn) noexcept
{
    depthFormat = depthFormatIn;
}

void Bootstrap::SetPresentationInterval(int presentationIntervalIn) noexcept
{
    presentationInterval = presentationIntervalIn;
}

void Bootstrap::SetBackBufferSize(int width, int height) noexcept
{
    POMDOG_ASSERT(width >= 0);
    POMDOG_ASSERT(height >= 0);
    backBufferWidth = width;
    backBufferHeight = height;
}

void Bootstrap::SetFullScreen(bool isFullScreenIn) noexcept
{
    isFullScreen = isFullScreenIn;
}

void Bootstrap::OnError(std::function<void(std::unique_ptr<Error>&& err)> onErrorIn)
{
    onError = std::move(onErrorIn);
}

void Bootstrap::Run(
    const std::function<std::unique_ptr<Game>(const std::shared_ptr<GameHost>&)>& createApp)
{
    gpu::PresentationParameters presentationParameters;
    presentationParameters.backBufferHeight = backBufferHeight;
    presentationParameters.backBufferWidth = backBufferWidth;
    presentationParameters.presentationInterval = presentationInterval;
    presentationParameters.backBufferFormat = surfaceFormat;
    presentationParameters.depthStencilFormat = depthFormat;
    presentationParameters.multiSampleCount = 1;
    presentationParameters.isFullScreen = isFullScreen;

    auto gameHost = std::make_shared<pomdog::detail::linux::GameHostLinux>();
    if (auto err = gameHost->initialize(presentationParameters); err != nullptr) {
        if (onError != nullptr) {
            onError(errors::wrap(std::move(err), "failed to initialize GameHostLinux"));
        }
        return;
    }

    POMDOG_ASSERT(createApp);
    auto game = createApp(gameHost);
    if (game == nullptr) {
        if (onError != nullptr) {
            onError(errors::make("game must be != nullptr"));
        }
        return;
    }

    POMDOG_ASSERT(game != nullptr);
    if (auto err = game->initialize(); err != nullptr) {
        if (onError != nullptr) {
            onError(errors::wrap(std::move(err), "failed to initialize game"));
        }
        return;
    }

    gameHost->run(*game);

    game.reset();
    gameHost.reset();
}

} // namespace pomdog::x11
