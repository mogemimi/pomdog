// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/platform/x11/bootstrap.hpp"
#include "pomdog/application/game.hpp"
#include "pomdog/graphics/presentation_parameters.hpp"
#include "pomdog/platform/x11/game_host_x11.hpp"
#include "pomdog/utility/errors.hpp"
#include <utility>

namespace Pomdog::X11 {

void Bootstrap::SetSurfaceFormat(SurfaceFormat surfaceFormatIn) noexcept
{
    surfaceFormat = surfaceFormatIn;
}

void Bootstrap::SetDepthFormat(SurfaceFormat depthFormatIn) noexcept
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
    using Pomdog::Detail::X11::GameHostX11;

    PresentationParameters presentationParameters;
    presentationParameters.BackBufferHeight = backBufferHeight;
    presentationParameters.BackBufferWidth = backBufferWidth;
    presentationParameters.PresentationInterval = presentationInterval;
    presentationParameters.BackBufferFormat = surfaceFormat;
    presentationParameters.DepthStencilFormat = depthFormat;
    presentationParameters.MultiSampleCount = 1;
    presentationParameters.IsFullScreen = isFullScreen;

    std::shared_ptr<GameHostX11> gameHost;
    std::unique_ptr<Game> game;

    gameHost = std::make_shared<GameHostX11>();
    if (auto err = gameHost->Initialize(presentationParameters); err != nullptr) {
        if (onError != nullptr) {
            onError(Errors::Wrap(std::move(err), "failed to initialize GameHostX11"));
        }
        return;
    }

    POMDOG_ASSERT(createApp);
    game = createApp(gameHost);
    if (game == nullptr) {
        if (onError != nullptr) {
            onError(Errors::New("game must be != nullptr"));
        }
        return;
    }

    POMDOG_ASSERT(game != nullptr);
    if (auto err = game->Initialize(); err != nullptr) {
        if (onError != nullptr) {
            onError(Errors::Wrap(std::move(err), "failed to initialize game"));
        }
        return;
    }

    gameHost->Run(*game);

    game.reset();
    gameHost.reset();
}

} // namespace Pomdog::X11
