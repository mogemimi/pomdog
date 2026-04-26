// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/win32/bootstrap_win32.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/application/game.h"
#include "pomdog/application/game_host.h"
#include "pomdog/application/game_host_options.h"
#include "pomdog/application/game_setup.h"
#include "pomdog/application/win32/game_host_win32.h"
#include "pomdog/application/win32/game_window_win32.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::SystemEventQueue;

namespace pomdog::win32 {

void Bootstrap::setInstance(HINSTANCE hInstanceIn) noexcept
{
    hInstance_ = hInstanceIn;
}

void Bootstrap::setCommandShow(i32 cmdShowIn) noexcept
{
    cmdShow_ = cmdShowIn;
}

void Bootstrap::setIcon(HICON iconIn) noexcept
{
    icon_ = iconIn;
}

void Bootstrap::setIconSmall(HICON iconSmallIn) noexcept
{
    iconSmall_ = iconSmallIn;
}

void Bootstrap::setCommandLineArgs(int argc, const char* const* argv) noexcept
{
    argc_ = argc;
    argv_ = argv;
}

void Bootstrap::onError(std::function<void(std::unique_ptr<Error>&& err)> onErrorIn)
{
    onError_ = std::move(onErrorIn);
}

void Bootstrap::run(std::unique_ptr<GameSetup>&& gameSetup)
{
    if (gameSetup == nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::make("GameSetup must be != nullptr"));
        }
        return;
    }

    if (hInstance_ == nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::make("hInstance must be != nullptr"));
        }
        return;
    }

    // NOTE: Set up default options with Win32 defaults.
    GameHostOptions options;
    options.graphicsBackend = gpu::GraphicsBackend::Direct3D11;

    // NOTE: Let the GameSetup configure options (parse CLI, set up VFS, etc.).
    if (auto err = gameSetup->configure(options, std::span<const char* const>(argv_, static_cast<std::size_t>(argc_))); err != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(err), "failed to configure GameSetup"));
        }
        return;
    }

    // NOTE: Validate the configured options.
    switch (options.graphicsBackend) {
    case gpu::GraphicsBackend::OpenGL4:
    case gpu::GraphicsBackend::Direct3D11:
    case gpu::GraphicsBackend::Vulkan:
        break;
    default:
        if (onError_ != nullptr) {
            onError_(errors::make("unsupported graphics backend"));
        }
        return;
    }
    if (options.presentationInterval <= 0) {
        if (onError_ != nullptr) {
            onError_(errors::make("presentation interval must be > 0"));
        }
        return;
    }
    if (options.backBufferWidth <= 0 || options.backBufferHeight <= 0) {
        if (onError_ != nullptr) {
            onError_(errors::make("back buffer size must be > 0"));
        }
        return;
    }

    using pomdog::detail::win32::GameHostWin32;
    using pomdog::detail::win32::GameWindowWin32;

    gpu::PresentationParameters presentationParameters;
    presentationParameters.backBufferHeight = options.backBufferHeight;
    presentationParameters.backBufferWidth = options.backBufferWidth;
    presentationParameters.presentationInterval = options.presentationInterval;
    presentationParameters.backBufferFormat = options.surfaceFormat;
    presentationParameters.depthStencilFormat = options.depthFormat;
    presentationParameters.multiSampleCount = options.multiSampleCount;
    presentationParameters.isFullScreen = options.isFullScreen;

    auto eventQueue = std::make_shared<SystemEventQueue>();

    auto [gameWindow, windowErr] = GameWindowWin32::create(
        hInstance_,
        cmdShow_,
        icon_,
        iconSmall_,
        (options.graphicsBackend == gpu::GraphicsBackend::OpenGL4),
        eventQueue,
        presentationParameters);
    if (windowErr != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(windowErr), "GameWindowWin32::create() failed"));
        }
        return;
    }

    if (options.headless) {
        // NOTE: In headless mode, hide the window after creation.
        ::ShowWindow(gameWindow->getNativeWindowHandle(), SW_HIDE);
    }

    auto [gameHost, hostErr] = GameHostWin32::create(
        gameWindow,
        hInstance_,
        eventQueue,
        presentationParameters,
        options);
    if (hostErr != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(hostErr), "GameHostWin32::create() failed"));
        }
        return;
    }

    auto game = gameSetup->createGame();
    if (game == nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::make("GameSetup::createGame() returned nullptr"));
        }
        return;
    }

    // NOTE: GameSetup is no longer needed after createGame(); destroy it now.
    gameSetup = nullptr;

    if (auto err = game->initialize(gameHost); err != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(err), "failed to initialize game"));
        }
        return;
    }

    gameHost->run(*game);

    game = nullptr;
    gameHost = nullptr;
    gameWindow = nullptr;
}

} // namespace pomdog::win32
