// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/win32/bootstrap_win32.h"
#include "pomdog/application/game.h"
#include "pomdog/application/game_host.h"
#include "pomdog/application/system_events.h"
#include "pomdog/application/win32/game_host_win32.h"
#include "pomdog/application/win32/game_window_win32.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::SystemEvent;

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

void Bootstrap::setSurfaceFormat(gpu::PixelFormat surfaceFormatIn) noexcept
{
    surfaceFormat_ = surfaceFormatIn;
}

void Bootstrap::setDepthFormat(gpu::PixelFormat depthFormatIn) noexcept
{
    depthFormat_ = depthFormatIn;
}

void Bootstrap::setPresentationInterval(i32 presentationIntervalIn) noexcept
{
    presentationInterval_ = presentationIntervalIn;
}

void Bootstrap::setBackBufferSize(i32 width, i32 height) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    backBufferWidth_ = width;
    backBufferHeight_ = height;
}

void Bootstrap::setFullScreen(bool isFullScreenIn) noexcept
{
    isFullScreen_ = isFullScreenIn;
}

void Bootstrap::setGraphicsBackend(gpu::GraphicsBackend backendIn) noexcept
{
    graphicsBackend_ = backendIn;
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

[[nodiscard]] std::unique_ptr<Error>
Bootstrap::validate() noexcept
{
    switch (graphicsBackend_) {
    case gpu::GraphicsBackend::OpenGL4:
        break;
    case gpu::GraphicsBackend::Direct3D11:
        break;
    case gpu::GraphicsBackend::Vulkan:
        break;
    default:
        return errors::make("unsupported graphics backend");
    }
    if (presentationInterval_ <= 0) {
        return errors::make("presentation interval must be > 0");
    }
    if (backBufferWidth_ <= 0) {
        return errors::make("back buffer width must be > 0");
    }
    if (backBufferHeight_ <= 0) {
        return errors::make("back buffer height must be > 0");
    }
    return nullptr;
}

void Bootstrap::run(
    const std::function<std::unique_ptr<Game>()>& createApp)
{
    using pomdog::detail::win32::GameHostWin32;
    using pomdog::detail::win32::GameWindowWin32;

    gpu::PresentationParameters presentationParameters;
    presentationParameters.backBufferHeight = backBufferHeight_;
    presentationParameters.backBufferWidth = backBufferWidth_;
    presentationParameters.presentationInterval = presentationInterval_;
    presentationParameters.backBufferFormat = surfaceFormat_;
    presentationParameters.depthStencilFormat = depthFormat_;
    presentationParameters.multiSampleCount = 1;
    presentationParameters.isFullScreen = isFullScreen_;

    auto eventQueue = std::make_shared<EventQueue<SystemEvent>>();

    auto gameWindow = std::make_shared<GameWindowWin32>();
    if (auto err = gameWindow->initialize(
            hInstance_,
            cmdShow_,
            icon_,
            iconSmall_,
            (graphicsBackend_ == gpu::GraphicsBackend::OpenGL4),
            eventQueue,
            presentationParameters);
        err != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(err), "GameWindowWin32::Initialize() failed"));
        }
        return;
    }

    auto gameHost = std::make_shared<GameHostWin32>();

    if (auto err = gameHost->initialize(
            gameWindow,
            hInstance_,
            eventQueue,
            presentationParameters,
            graphicsBackend_);
        err != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(err), "GameHostWin32::Initialize() failed"));
        }
        return;
    }

    POMDOG_ASSERT(createApp);
    auto game = createApp();
    if (game == nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::make("game must be != nullptr"));
        }
        return;
    }

    POMDOG_ASSERT(game != nullptr);
    if (auto err = game->initialize(gameHost, argc_, argv_); err != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(err), "failed to initialize game"));
        }
        return;
    }

    gameHost->run(*game);

    gameHost.reset();
    gameWindow.reset();
}

} // namespace pomdog::win32
