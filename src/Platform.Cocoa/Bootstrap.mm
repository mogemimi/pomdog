// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Platform/Cocoa/Bootstrap.hpp"
#include "GameHostCocoa.hpp"
#include "GameWindowCocoa.hpp"
#include "PomdogMetalViewController.hpp"
#include "PomdogOpenGLView.hpp"
#include "../Application/SystemEvents.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Errors.hpp"
#import <MetalKit/MTKView.h>
#include <utility>

using Pomdog::Detail::SystemEvent;
using Pomdog::Detail::Cocoa::GameHostCocoa;
using Pomdog::Detail::Cocoa::GameWindowCocoa;

namespace Pomdog::Cocoa {

void Bootstrap::SetWindow(NSWindow* window)
{
    POMDOG_ASSERT(window != nullptr);
    nativeWindow = window;
}

void Bootstrap::SetOpenGLEnabled(bool enabled)
{
    openGLEnabled = enabled;
}

void Bootstrap::SetOpenGLSurfaceFormat(SurfaceFormat surfaceFormatIn)
{
    surfaceFormat = surfaceFormatIn;
}

void Bootstrap::SetOpenGLDepthFormat(SurfaceFormat depthFormatIn)
{
    depthFormat = depthFormatIn;
}

void Bootstrap::OnError(std::function<void(std::shared_ptr<Error>&& err)>&& onErrorIn)
{
    POMDOG_ASSERT(onErrorIn);
    onError = std::move(onErrorIn);
}

void Bootstrap::OnCompleted(std::function<void()>&& onCompletedIn)
{
    POMDOG_ASSERT(onCompletedIn);
    onCompleted = [this, onCompletedIn = std::move(onCompletedIn)] {
        this->game.reset();
        this->gameHostCocoa.reset();
        this->gameHostMetal.reset();
        onCompletedIn();
    };
}

std::shared_ptr<Error>
Bootstrap::Run(std::function<std::shared_ptr<Game>(const std::shared_ptr<GameHost>&)>&& createGame)
{
    POMDOG_ASSERT(nativeWindow != nullptr);
    POMDOG_ASSERT(createGame);

    if (!onCompleted) {
        onCompleted = [window = nativeWindow] {
            [window close];
            [NSApp terminate:nullptr];
        };
    }

    if (openGLEnabled) {
        NSRect bounds = nativeWindow.frame;

        PomdogOpenGLView* view = [[PomdogOpenGLView alloc] initWithFrame:bounds];
        view.hidden = NO;
        view.needsDisplay = YES;
        view.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

        [nativeWindow setContentView:view];
        [nativeWindow makeKeyAndOrderFront:nullptr];
        [nativeWindow orderFrontRegardless];

        PresentationParameters presentationParameters;
        presentationParameters.BackBufferFormat = surfaceFormat;
        presentationParameters.DepthStencilFormat = depthFormat;
        presentationParameters.PresentationInterval = 60;
        presentationParameters.MultiSampleCount = 1;
        presentationParameters.BackBufferWidth = bounds.size.width;
        presentationParameters.BackBufferHeight = bounds.size.height;
        presentationParameters.IsFullScreen = false;

        POMDOG_ASSERT(onCompleted);
        POMDOG_ASSERT(createGame);

        auto eventQueue = std::make_shared<EventQueue<SystemEvent>>();

        // NOTE: Create a window.
        auto gameWindow = std::make_shared<GameWindowCocoa>();
        if (auto err = gameWindow->Initialize(nativeWindow, eventQueue); err != nullptr) {
            return Errors::Wrap(std::move(err), "GameWindowCocoa::Initialize() failed.");
        }

        // NOTE: Create a game host for Cocoa.
        gameHostCocoa = std::make_shared<GameHostCocoa>();
        if (auto err = gameHostCocoa->Initialize(view, gameWindow, eventQueue, presentationParameters); err != nullptr) {
            return Errors::Wrap(std::move(err), "GameHostCocoa::Initialize() failed.");
        }

        game = createGame(gameHostCocoa);
        if (game == nullptr) {
            return Errors::New("game must be != nullptr");
        }

        if (auto err = gameHostCocoa->Run(game, std::move(onCompleted)); err != nullptr) {
            return Errors::Wrap(std::move(err), "GameHostCocoa::Run() failed.");
        }
    }
    else {
        viewController = [[PomdogMetalViewController alloc] initWithNibName:nullptr bundle:nullptr];
        [viewController startGame:std::move(createGame) completed:std::move(onCompleted)];

        NSRect bounds = nativeWindow.frame;

        // NOTE: To avoid linking error, use NSClassFromString indirectly instead of MTKView.
        MTKView* view = [[NSClassFromString(@"MTKView") alloc] initWithFrame:bounds];

        view.delegate = viewController;
        view.hidden = NO;
        view.needsDisplay = YES;
        view.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

        [nativeWindow setContentView:view];
        [nativeWindow makeKeyAndOrderFront:nullptr];
        [nativeWindow orderFrontRegardless];

        [viewController setView:view];
        [viewController viewDidLoad];
    }

    return nullptr;
}

} // namespace Pomdog::Cocoa
