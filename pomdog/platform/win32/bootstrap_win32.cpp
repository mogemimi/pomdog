// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/win32/bootstrap_win32.h"
#include "pomdog/application/game.h"
#include "pomdog/application/game_host.h"
#include "pomdog/application/system_events.h"
#include "pomdog/application/win32/game_host_win32.h"
#include "pomdog/application/win32/game_window_win32.h"
#include "pomdog/basic/conditional_compilation.h"
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

void Bootstrap::setCommandShow(int cmdShowIn) noexcept
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

void Bootstrap::setSurfaceFormat(PixelFormat surfaceFormatIn) noexcept
{
    surfaceFormat_ = surfaceFormatIn;
}

void Bootstrap::setDepthFormat(PixelFormat depthFormatIn) noexcept
{
    depthFormat_ = depthFormatIn;
}

void Bootstrap::setPresentationInterval(int presentationIntervalIn) noexcept
{
    presentationInterval_ = presentationIntervalIn;
}

void Bootstrap::setBackBufferSize(int width, int height) noexcept
{
    POMDOG_ASSERT(width >= 0);
    POMDOG_ASSERT(height >= 0);
    backBufferWidth_ = width;
    backBufferHeight_ = height;
}

void Bootstrap::setFullScreen(bool isFullScreenIn) noexcept
{
    isFullScreen_ = isFullScreenIn;
}

void Bootstrap::setOpenGLEnabled(bool openGLEnabledIn) noexcept
{
    openGLEnabled_ = openGLEnabledIn;
}

void Bootstrap::onError(std::function<void(std::unique_ptr<Error>&& err)> onErrorIn)
{
    onError_ = std::move(onErrorIn);
}

void Bootstrap::run(
    const std::function<std::unique_ptr<Game>(const std::shared_ptr<GameHost>&)>& createApp)
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

    const bool useOpenGL = openGLEnabled_;

    auto gameWindow = std::make_shared<GameWindowWin32>();
    if (auto err = gameWindow->initialize(
            hInstance_,
            cmdShow_,
            icon_,
            iconSmall_,
            useOpenGL,
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
            useOpenGL);
        err != nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::wrap(std::move(err), "GameHostWin32::Initialize() failed"));
        }
        return;
    }

    POMDOG_ASSERT(createApp);
    auto game = createApp(gameHost);
    if (game == nullptr) {
        if (onError_ != nullptr) {
            onError_(errors::make("game must be != nullptr"));
        }
        return;
    }

    POMDOG_ASSERT(game != nullptr);
    if (auto err = game->initialize(); err != nullptr) {
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
