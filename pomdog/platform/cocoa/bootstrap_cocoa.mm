// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/cocoa/bootstrap_cocoa.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/application/cocoa/game_host_cocoa.h"
#include "pomdog/application/cocoa/game_window_cocoa.h"
#include "pomdog/application/cocoa/pomdog_metal_view_controller.h"
#include "pomdog/application/cocoa/pomdog_opengl_view.h"
#include "pomdog/application/game.h"
#include "pomdog/application/game_host.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <MetalKit/MTKView.h>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::SystemEventQueue;
using pomdog::detail::cocoa::GameHostCocoa;
using pomdog::detail::cocoa::GameWindowCocoa;

namespace pomdog::cocoa {

void Bootstrap::setWindow(NSWindow* window)
{
    POMDOG_ASSERT(window != nullptr);
    nativeWindow_ = window;
}

void Bootstrap::setGraphicsBackend(gpu::GraphicsBackend backend)
{
    graphicsBackend_ = backend;
}

void Bootstrap::setOpenGLSurfaceFormat(gpu::PixelFormat surfaceFormatIn)
{
    surfaceFormat_ = surfaceFormatIn;
}

void Bootstrap::setOpenGLDepthFormat(gpu::PixelFormat depthFormatIn)
{
    depthFormat_ = depthFormatIn;
}

void Bootstrap::onError(std::function<void(std::unique_ptr<Error>&& err)>&& onErrorIn)
{
    POMDOG_ASSERT(onErrorIn);
    onError_ = std::move(onErrorIn);
}

void Bootstrap::onCompleted(std::function<void()>&& onCompletedIn)
{
    POMDOG_ASSERT(onCompletedIn);
    onCompleted_ = [this, onCompletedIn = std::move(onCompletedIn)] {
        game_.reset();
        gameHostCocoa_.reset();
        gameHostMetal_.reset();
        onCompletedIn();
    };
}

std::unique_ptr<Error>
Bootstrap::run(std::function<std::shared_ptr<Game>()>&& createGame)
{
    POMDOG_ASSERT(nativeWindow_ != nullptr);
    POMDOG_ASSERT(createGame);

    if (!onCompleted_) {
        onCompleted_ = [window = nativeWindow_] {
            [window close];
            [NSApp terminate:nullptr];
        };
    }

    if (graphicsBackend_ == gpu::GraphicsBackend::OpenGL4) {
        NSRect bounds = nativeWindow_.frame;

        PomdogOpenGLView* view = [[PomdogOpenGLView alloc] initWithFrame:bounds];
        view.hidden = NO;
        view.needsDisplay = YES;
        view.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

        [nativeWindow_ setContentView:view];
        [nativeWindow_ makeKeyAndOrderFront:nullptr];
        [nativeWindow_ orderFrontRegardless];

        gpu::PresentationParameters presentationParameters;
        presentationParameters.backBufferFormat = surfaceFormat_;
        presentationParameters.depthStencilFormat = depthFormat_;
        presentationParameters.presentationInterval = 60;
        presentationParameters.multiSampleCount = 1;
        presentationParameters.backBufferWidth = bounds.size.width;
        presentationParameters.backBufferHeight = bounds.size.height;
        presentationParameters.isFullScreen = false;

        POMDOG_ASSERT(onCompleted_);
        POMDOG_ASSERT(createGame);

        auto eventQueue = std::make_shared<SystemEventQueue>();

        // NOTE: Create a window.
        auto [gameWindow, windowErr] = GameWindowCocoa::create(nativeWindow_, eventQueue);
        if (windowErr != nullptr) {
            return errors::wrap(std::move(windowErr), "GameWindowCocoa::create() failed");
        }

        // NOTE: Create a game host for Cocoa.
        auto [gameHostCocoa, hostErr] = GameHostCocoa::create(view, gameWindow, eventQueue, presentationParameters);
        if (hostErr != nullptr) {
            return errors::wrap(std::move(hostErr), "GameHostCocoa::create() failed");
        }
        gameHostCocoa_ = std::move(gameHostCocoa);

        game_ = createGame();
        if (game_ == nullptr) {
            return errors::make("game must be != nullptr");
        }

        if (auto err = gameHostCocoa_->run(game_, std::move(onCompleted_)); err != nullptr) {
            return errors::wrap(std::move(err), "GameHostCocoa::run() failed");
        }
    }
    else {
        viewController_ = [[PomdogMetalViewController alloc] initWithNibName:nullptr bundle:nullptr];
        [viewController_ startGame:std::move(createGame) completed:std::move(onCompleted_)];

        NSRect bounds = nativeWindow_.frame;

        // NOTE: To avoid linking error, use NSClassFromString indirectly instead of MTKView.
        MTKView* view = [[NSClassFromString(@"MTKView") alloc] initWithFrame:bounds];

        view.delegate = viewController_;
        view.hidden = NO;
        view.needsDisplay = YES;
        view.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

        [nativeWindow_ setContentView:view];
        [nativeWindow_ makeKeyAndOrderFront:nullptr];
        [nativeWindow_ orderFrontRegardless];

        [viewController_ setView:view];
        [viewController_ viewDidLoad];
    }

    return nullptr;
}

} // namespace pomdog::cocoa
