// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "Pomdog/Platform/X11/Bootstrap.hpp"
#include "GameHostX11.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include <utility>

namespace Pomdog {
namespace X11 {
//-----------------------------------------------------------------------
void Bootstrap::SetSurfaceFormat(SurfaceFormat surfaceFormatIn) noexcept
{
    surfaceFormat = surfaceFormatIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetDepthFormat(DepthFormat depthFormatIn) noexcept
{
    depthFormat = depthFormatIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetPresentationInterval(int presentationIntervalIn) noexcept
{
    presentationInterval = presentationIntervalIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetBackBufferSize(int width, int height) noexcept
{
    POMDOG_ASSERT(width >= 0);
    POMDOG_ASSERT(height >= 0);
    backBufferWidth = width;
    backBufferHeight = height;
}
//-----------------------------------------------------------------------
void Bootstrap::SetFullScreen(bool isFullScreenIn) noexcept
{
    isFullScreen = isFullScreenIn;
}
//-----------------------------------------------------------------------
void Bootstrap::OnError(std::function<void(std::exception const&)> onErrorIn)
{
    onError = onErrorIn;
}
//-----------------------------------------------------------------------
void Bootstrap::Run(std::function<std::unique_ptr<Game>(
    std::shared_ptr<GameHost> const&)> const& createApp)
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

    try {
        gameHost = std::make_shared<GameHostX11>(presentationParameters);

        POMDOG_ASSERT(createApp);
        game = createApp(gameHost);
    }
    catch (std::exception const& e) {
        if (onError) {
            onError(e);
        }
        return;
    }

    POMDOG_ASSERT(game);
    if (game) {
        gameHost->Run(*game);
    }

    game.reset();
    gameHost.reset();
}
//-----------------------------------------------------------------------
} // namespace X11
} // namespace Pomdog
