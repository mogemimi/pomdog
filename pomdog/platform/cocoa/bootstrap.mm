// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/cocoa/bootstrap.hpp"
#include "pomdog/application/game.hpp"
#include "pomdog/application/game_host.hpp"
#include "pomdog/application/system_events.hpp"
#include "pomdog/graphics/presentation_parameters.hpp"
#include "pomdog/platform/cocoa/game_host_cocoa.hpp"
#include "pomdog/platform/cocoa/game_window_cocoa.hpp"
#include "pomdog/platform/cocoa/pomdog_metal_view_controller.hpp"
#include "pomdog/platform/cocoa/pomdog_opengl_view.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/errors.hpp"
#import <MetalKit/MTKView.h>
#include <utility>

using pomdog::detail::SystemEvent;
using pomdog::detail::cocoa::GameHostCocoa;
using pomdog::detail::cocoa::GameWindowCocoa;

namespace pomdog::cocoa {

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

void Bootstrap::OnError(std::function<void(std::unique_ptr<Error>&& err)>&& onErrorIn)
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

std::unique_ptr<Error>
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
            return errors::Wrap(std::move(err), "GameWindowCocoa::Initialize() failed.");
        }

        // NOTE: Create a game host for Cocoa.
        gameHostCocoa = std::make_shared<GameHostCocoa>();
        if (auto err = gameHostCocoa->Initialize(view, gameWindow, eventQueue, presentationParameters); err != nullptr) {
            return errors::Wrap(std::move(err), "GameHostCocoa::Initialize() failed.");
        }

        game = createGame(gameHostCocoa);
        if (game == nullptr) {
            return errors::New("game must be != nullptr");
        }

        if (auto err = gameHostCocoa->Run(game, std::move(onCompleted)); err != nullptr) {
            return errors::Wrap(std::move(err), "GameHostCocoa::Run() failed.");
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

} // namespace pomdog::cocoa
