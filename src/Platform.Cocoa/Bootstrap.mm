// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Platform/Cocoa/Bootstrap.hpp"
#include "GameWindowCocoa.hpp"
#include "GameHostCocoa.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Logging/Log.hpp"
#import "PomdogOpenGLView.hpp"

using Pomdog::Detail::Cocoa::GameHostCocoa;
using Pomdog::Detail::Cocoa::GameWindowCocoa;

namespace Pomdog {
namespace Cocoa {
//-----------------------------------------------------------------------
void Bootstrap::SetView(PomdogOpenGLView* openGLViewIn)
{
    POMDOG_ASSERT(openGLViewIn != nil);
    openGLView = openGLViewIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetSurfaceFormat(SurfaceFormat surfaceFormatIn)
{
    surfaceFormat = surfaceFormatIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetDepthFormat(DepthFormat depthFormatIn)
{
    depthFormat = depthFormatIn;
}
//-----------------------------------------------------------------------
void Bootstrap::OnError(std::function<void(std::exception const&)> onErrorIn)
{
    POMDOG_ASSERT(onErrorIn);
    onError = onErrorIn;
}
//-----------------------------------------------------------------------
void Bootstrap::OnCompleted(std::function<void()> onCompletedIn)
{
    POMDOG_ASSERT(onCompletedIn);
    onCompleted = onCompletedIn;
}
//-----------------------------------------------------------------------
void Bootstrap::Run(std::function<std::unique_ptr<Game>(
    std::shared_ptr<GameHost> const&)> const& createApp)
{
    POMDOG_ASSERT(openGLView != nil);
    POMDOG_ASSERT(createApp);

    NSWindow* nativeWindow = [openGLView window];
    POMDOG_ASSERT(nativeWindow != nil);

    NSRect bounds = [openGLView bounds];

    PresentationParameters presentationParameters;
    presentationParameters.SurfaceFormat = surfaceFormat;
    presentationParameters.DepthFormat = depthFormat;
    presentationParameters.PresentationInterval = 60;
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
    catch (std::exception const& e) {
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
//-----------------------------------------------------------------------
} // namespace Cocoa
} // namespace Pomdog
