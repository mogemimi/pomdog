// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/cocoa/bootstrap_cocoa.h"
#include "pomdog/application/cocoa/game_host_cocoa.h"
#include "pomdog/application/cocoa/game_window_cocoa.h"
#include "pomdog/application/cocoa/pomdog_metal_view_controller.h"
#include "pomdog/application/cocoa/pomdog_opengl_view.h"
#include "pomdog/application/game.h"
#include "pomdog/application/game_host.h"
#include "pomdog/application/system_events.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <MetalKit/MTKView.h>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::SystemEvent;
using pomdog::detail::cocoa::GameHostCocoa;
using pomdog::detail::cocoa::GameWindowCocoa;

namespace pomdog::cocoa {

void Bootstrap::setWindow(NSWindow* window)
{
    POMDOG_ASSERT(window != nullptr);
    nativeWindow_ = window;
}

void Bootstrap::setOpenGLEnabled(bool enabled)
{
    openGLEnabled_ = enabled;
}

void Bootstrap::setOpenGLSurfaceFormat(PixelFormat surfaceFormatIn)
{
    surfaceFormat_ = surfaceFormatIn;
}

void Bootstrap::setOpenGLDepthFormat(PixelFormat depthFormatIn)
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
Bootstrap::run(std::function<std::shared_ptr<Game>(const std::shared_ptr<GameHost>&)>&& createGame)
{
    POMDOG_ASSERT(nativeWindow_ != nullptr);
    POMDOG_ASSERT(createGame);

    if (!onCompleted_) {
        onCompleted_ = [window = nativeWindow_] {
            [window close];
            [NSApp terminate:nullptr];
        };
    }

    if (openGLEnabled_) {
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

        auto eventQueue = std::make_shared<EventQueue<SystemEvent>>();

        // NOTE: Create a window.
        auto gameWindow = std::make_shared<GameWindowCocoa>();
        if (auto err = gameWindow->initialize(nativeWindow_, eventQueue); err != nullptr) {
            return errors::wrap(std::move(err), "GameWindowCocoa::Initialize() failed.");
        }

        // NOTE: Create a game host for Cocoa.
        gameHostCocoa_ = std::make_shared<GameHostCocoa>();
        if (auto err = gameHostCocoa_->initialize(view, gameWindow, eventQueue, presentationParameters); err != nullptr) {
            return errors::wrap(std::move(err), "GameHostCocoa::Initialize() failed.");
        }

        game_ = createGame(gameHostCocoa_);
        if (game_ == nullptr) {
            return errors::make("game must be != nullptr");
        }

        if (auto err = gameHostCocoa_->run(game_, std::move(onCompleted_)); err != nullptr) {
            return errors::wrap(std::move(err), "GameHostCocoa::Run() failed.");
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
