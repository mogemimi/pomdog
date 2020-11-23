// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Platform/X11/Bootstrap.hpp"
#include "GameHostX11.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>

namespace Pomdog::X11 {

void Bootstrap::SetSurfaceFormat(SurfaceFormat surfaceFormatIn) noexcept
{
    surfaceFormat = surfaceFormatIn;
}

void Bootstrap::SetDepthFormat(DepthFormat depthFormatIn) noexcept
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

void Bootstrap::OnError(std::function<void(const std::exception&)> onErrorIn)
{
    onError = onErrorIn;
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

    try {
        gameHost = std::make_shared<GameHostX11>();
        if (auto err = gameHost->Initialize(presentationParameters); err != nullptr) {
            // FIXME: error handling without using exceptions
            POMDOG_THROW_EXCEPTION(std::runtime_error, "failed to initialize GameHostX11: " + err->ToString());
        }

        POMDOG_ASSERT(createApp);
        game = createApp(gameHost);
    }
    catch (const std::exception& e) {
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

} // namespace Pomdog::X11
