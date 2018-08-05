// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Platform/Cocoa/Bootstrap.hpp"
#include "GameHostCocoa.hpp"
#include "GameWindowCocoa.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#import "Pomdog/Platform/Cocoa/PomdogOpenGLView.hpp"

using Pomdog::Detail::Cocoa::GameHostCocoa;
using Pomdog::Detail::Cocoa::GameWindowCocoa;

namespace Pomdog {
namespace Cocoa {

void Bootstrap::SetView(PomdogOpenGLView* openGLViewIn)
{
    POMDOG_ASSERT(openGLViewIn != nil);
    openGLView = openGLViewIn;
}

void Bootstrap::SetSurfaceFormat(SurfaceFormat surfaceFormatIn)
{
    surfaceFormat = surfaceFormatIn;
}

void Bootstrap::SetDepthFormat(DepthFormat depthFormatIn)
{
    depthFormat = depthFormatIn;
}

void Bootstrap::OnError(std::function<void(const std::exception&)> onErrorIn)
{
    POMDOG_ASSERT(onErrorIn);
    onError = onErrorIn;
}

void Bootstrap::OnCompleted(std::function<void()> onCompletedIn)
{
    POMDOG_ASSERT(onCompletedIn);
    onCompleted = [=] {
        this->game.reset();
        this->gameHost.reset();
        onCompletedIn();
    };
}

void Bootstrap::Run(
    const std::function<std::unique_ptr<Game>(const std::shared_ptr<GameHost>&)>& createApp)
{
    POMDOG_ASSERT(openGLView != nil);
    POMDOG_ASSERT(createApp);

    NSWindow* nativeWindow = [openGLView window];
    POMDOG_ASSERT(nativeWindow != nil);

    NSRect bounds = [openGLView bounds];

    PresentationParameters presentationParameters;
    presentationParameters.BackBufferFormat = surfaceFormat;
    presentationParameters.DepthStencilFormat = depthFormat;
    presentationParameters.PresentationInterval = 60;
    presentationParameters.MultiSampleCount = 1;
    presentationParameters.BackBufferWidth = bounds.size.width;
    presentationParameters.BackBufferHeight = bounds.size.height;
    presentationParameters.IsFullScreen = false;

    try {
        auto eventQueue = std::make_shared<EventQueue>();

        auto gameWindow = std::make_shared<GameWindowCocoa>(
            nativeWindow, eventQueue);

        gameHost = std::make_shared<GameHostCocoa>(
            openGLView, gameWindow, eventQueue, presentationParameters);

        game = createApp(gameHost);
        gameHost->Run(game, onCompleted);
    }
    catch (const std::exception& e) {
        if (onError) {
            onError(e);
        }
        else {
#if defined(DEBUG) && !defined(NDEBUG)
            Log::Critical("Pomdog", e.what());
#endif
            throw e;
        }
    }
}

} // namespace Cocoa
} // namespace Pomdog
