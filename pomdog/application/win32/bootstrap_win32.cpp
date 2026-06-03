// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/win32/bootstrap_win32.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/application/game.h"
#include "pomdog/application/game_host.h"
#include "pomdog/application/game_host_options.h"
#include "pomdog/application/game_setup.h"
#include "pomdog/application/high_dpi_settings.h"
#include "pomdog/application/win32/game_host_win32.h"
#include "pomdog/application/win32/game_window_win32.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
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
    GameHostOptions options = {};
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
    if (options.maxFramesPerSecond.has_value() && options.maxFramesPerSecond.value() <= 0) {
        if (onError_ != nullptr) {
            onError_(errors::make("maxFramesPerSecond must be > 0"));
        }
        return;
    }
    if (options.clientWidth <= 0 || options.clientHeight <= 0) {
        if (onError_ != nullptr) {
            onError_(errors::make("client area size must be > 0"));
        }
        return;
    }
    if (options.windowMode == WindowMode::BrowserSoftFullscreen) {
        if (onError_ != nullptr) {
            onError_(errors::make("BrowserSoftFullscreen mode is not supported on Windows"));
        }
        return;
    }

    using pomdog::detail::win32::GameHostWin32;
    using pomdog::detail::win32::GameWindowWin32;

    // NOTE: Enable Per-Monitor V2 DPI awareness for HiDPI / Retina / 4K
    // support. When `HighDPIMode::Disabled` is selected, leave the process
    // DPI-Unaware so the OS bitmap-stretches the window (legacy behavior).
    if (options.highDPI.mode != HighDPIMode::Disabled) {
        // NOTE: Pomdog targets Windows 10 1607 or later. `SetProcessDpiAwarenessContext`
        // and `DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2` are available there;
        // a failure here is treated as benign (the process may have already had
        // its DPI awareness set elsewhere).
        [[maybe_unused]] auto ok = ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }

    // NOTE: Estimate the initial pixel ratio from the system DPI so that the
    // initial back buffer is sized in physical pixels. The actual per-monitor
    // value is queried by the GameWindow after creation (GetDpiForWindow) and
    // committed at the first frame boundary; this initial value only needs to
    // be a reasonable starting point.
    //
    // NOTE: Desktop uses the unclamped ratio (`maxPixelRatio` is not applied),
    // so the back buffer is sized at the OS-reported DPI and the logical client
    // size equals the requested size.
    const UINT initialDpi = (options.highDPI.mode == HighDPIMode::Disabled) ? 96u : ::GetDpiForSystem();
    const f32 rawRatio = static_cast<f32>(initialDpi) / 96.0f;
    const f32 unclampedRatio = computeUnclampedPixelRatio(options.highDPI, rawRatio);
    const i32 initialPhysWidth = static_cast<i32>(std::lround(static_cast<f32>(options.clientWidth) * unclampedRatio));
    const i32 initialPhysHeight = static_cast<i32>(std::lround(static_cast<f32>(options.clientHeight) * unclampedRatio));

    gpu::PresentationParameters presentationParameters = {};
    presentationParameters.backBufferHeight = initialPhysHeight;
    presentationParameters.backBufferWidth = initialPhysWidth;
    presentationParameters.backBufferFormat = options.surfaceFormat;
    presentationParameters.depthStencilFormat = options.depthFormat;
    presentationParameters.multiSampleCount = options.multiSampleCount;
    presentationParameters.windowMode = options.windowMode;

    auto eventQueue = std::make_shared<SystemEventQueue>();

    auto [gameWindow, windowErr] = GameWindowWin32::create(
        hInstance_,
        cmdShow_,
        icon_,
        iconSmall_,
        (options.graphicsBackend == gpu::GraphicsBackend::OpenGL4),
        eventQueue,
        presentationParameters,
        options.highDPI);
    if (windowErr != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(windowErr), "GameWindowWin32::create() failed"));
        }
        return;
    }

    // NOTE: The window has been created and queried its actual per-monitor
    // DPI. Update the presentation parameters with the precise physical back
    // buffer size before the GraphicsDevice and swap chain are created.
    {
        const auto metrics = gameWindow->getDisplayMetrics();
        presentationParameters.backBufferWidth = metrics.backBufferWidth;
        presentationParameters.backBufferHeight = metrics.backBufferHeight;
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
