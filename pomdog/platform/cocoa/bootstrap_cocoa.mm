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
#include "pomdog/application/game_host_options.h"
#include "pomdog/application/game_setup.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <MetalKit/MTKView.h>
#include <crt_externs.h>
#include <span>
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

void Bootstrap::onError(std::function<void(std::unique_ptr<Error>&& err)>&& onErrorIn)
{
    POMDOG_ASSERT(onErrorIn);
    onError_ = std::move(onErrorIn);
}

void Bootstrap::onCompleted(std::function<void()>&& onCompletedIn)
{
    POMDOG_ASSERT(onCompletedIn);
    onCompleted_ = [this, onCompletedIn = std::move(onCompletedIn)] {
        game_ = nullptr;
        gameHostCocoa_ = nullptr;
        gameHostMetal_ = nullptr;
        onCompletedIn();
    };
}

std::unique_ptr<Error>
Bootstrap::run(std::unique_ptr<GameSetup>&& gameSetup)
{
    POMDOG_ASSERT(nativeWindow_ != nullptr);

    if (gameSetup == nullptr) {
        return errors::make("GameSetup must be != nullptr");
    }

    gameSetup_ = std::move(gameSetup);

    // NOTE: Set up default options with macOS defaults.
    GameHostOptions options;
    options.graphicsBackend = gpu::GraphicsBackend::Metal;
    options.surfaceFormat = gpu::PixelFormat::B8G8R8A8_UNorm;
    options.depthFormat = gpu::PixelFormat::Depth32_Float_Stencil8_Uint;

    // NOTE: Get argc/argv from NSProcessInfo via _NSGetArgc/_NSGetArgv.
    int argc = *_NSGetArgc();
    const char* const* argv = const_cast<const char* const*>(*_NSGetArgv());

    // NOTE: Let the GameSetup configure options.
    if (auto err = gameSetup_->configure(options, std::span<const char* const>(argv, static_cast<std::size_t>(argc))); err != nullptr) {
        return errors::wrap(std::move(err), "failed to configure GameSetup");
    }

    // NOTE: Validate the configured options.
    if (options.presentationInterval <= 0) {
        return errors::make("presentation interval must be > 0");
    }
    if (options.backBufferWidth <= 0 || options.backBufferHeight <= 0) {
        return errors::make("back buffer size must be > 0");
    }

    if (!onCompleted_) {
        onCompleted_ = [window = nativeWindow_] {
            [window close];
            [NSApp terminate:nullptr];
        };
    }

    // NOTE: Build PresentationParameters from the configured options.
    gpu::PresentationParameters presentationParameters;
    presentationParameters.backBufferFormat = options.surfaceFormat;
    presentationParameters.depthStencilFormat = options.depthFormat;
    presentationParameters.presentationInterval = options.presentationInterval;
    presentationParameters.multiSampleCount = options.multiSampleCount;
    presentationParameters.backBufferWidth = options.backBufferWidth;
    presentationParameters.backBufferHeight = options.backBufferHeight;
    presentationParameters.isFullScreen = options.isFullScreen;

    auto createGame = [this]() -> std::shared_ptr<Game> {
        game_ = gameSetup_->createGame();
        // NOTE: GameSetup is no longer needed after createGame(); destroy it now.
        gameSetup_ = nullptr;
        return game_;
    };

    if (options.graphicsBackend == gpu::GraphicsBackend::OpenGL4) {
        NSRect bounds = nativeWindow_.frame;

        PomdogOpenGLView* view = [[PomdogOpenGLView alloc] initWithFrame:bounds];
        view.hidden = NO;
        view.needsDisplay = YES;
        view.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

        [nativeWindow_ setContentView:view];
        [nativeWindow_ makeKeyAndOrderFront:nullptr];
        [nativeWindow_ orderFrontRegardless];

        // NOTE: Override back buffer size from actual view bounds.
        presentationParameters.backBufferWidth = bounds.size.width;
        presentationParameters.backBufferHeight = bounds.size.height;

        POMDOG_ASSERT(onCompleted_);
        POMDOG_ASSERT(createGame);

        auto eventQueue = std::make_shared<SystemEventQueue>();

        // NOTE: Create a window.
        auto [gameWindow, windowErr] = GameWindowCocoa::create(nativeWindow_, eventQueue);
        if (windowErr != nullptr) {
            return errors::wrap(std::move(windowErr), "GameWindowCocoa::create() failed");
        }

        // NOTE: Create a game host for Cocoa.
        auto [gameHostCocoa, hostErr] = GameHostCocoa::create(view, gameWindow, eventQueue, presentationParameters, options);
        if (hostErr != nullptr) {
            return errors::wrap(std::move(hostErr), "GameHostCocoa::create() failed");
        }
        gameHostCocoa_ = std::move(gameHostCocoa);

        game_ = createGame();
        if (game_ == nullptr) {
            return errors::make("GameSetup::createGame() returned nullptr");
        }

        if (auto err = gameHostCocoa_->run(game_, std::move(onCompleted_)); err != nullptr) {
            return errors::wrap(std::move(err), "GameHostCocoa::run() failed");
        }
    }
    else {
        viewController_ = [[PomdogMetalViewController alloc] initWithNibName:nullptr bundle:nullptr];
        [viewController_ startGame:std::move(createGame)
                         completed:std::move(onCompleted_)
            presentationParameters:presentationParameters
                           options:options];

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
