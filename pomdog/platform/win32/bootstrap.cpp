// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/win32/bootstrap.hpp"
#include "pomdog/application/game.hpp"
#include "pomdog/application/game_host.hpp"
#include "pomdog/application/system_events.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/presentation_parameters.hpp"
#include "pomdog/input/directinput/gamepad_directinput.hpp"
#include "pomdog/logging/log.hpp"
#include "pomdog/platform/win32/game_host_win32.hpp"
#include "pomdog/platform/win32/game_window_win32.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::SystemEvent;
using pomdog::detail::DirectInput::GamepadDirectInput;

namespace pomdog::win32 {

void Bootstrap::SetInstance(HINSTANCE hInstanceIn) noexcept
{
    hInstance = hInstanceIn;
}

void Bootstrap::SetCommandShow(int cmdShowIn) noexcept
{
    cmdShow = cmdShowIn;
}

void Bootstrap::SetIcon(HICON iconIn) noexcept
{
    icon = iconIn;
}

void Bootstrap::SetIconSmall(HICON iconSmallIn) noexcept
{
    iconSmall = iconSmallIn;
}

void Bootstrap::SetSurfaceFormat(SurfaceFormat surfaceFormatIn) noexcept
{
    surfaceFormat = surfaceFormatIn;
}

void Bootstrap::SetDepthFormat(SurfaceFormat depthFormatIn) noexcept
{
    depthFormat = depthFormatIn;
}

void Bootstrap::SetPresentationInterval(int presentationIntervalIn) noexcept
{
    presentationInterval = presentationIntervalIn;
}

void Bootstrap::SetBackBufferSize(int width, int height) noexcept
{
    POMDOG_ASSERT(width >= 0);
    POMDOG_ASSERT(height >= 0);
    backBufferWidth = width;
    backBufferHeight = height;
}

void Bootstrap::SetFullScreen(bool isFullScreenIn) noexcept
{
    isFullScreen = isFullScreenIn;
}

void Bootstrap::SetOpenGLEnabled(bool openGLEnabledIn) noexcept
{
    openGLEnabled = openGLEnabledIn;
}

void Bootstrap::OnError(std::function<void(std::unique_ptr<Error>&& err)> onErrorIn)
{
    onError = std::move(onErrorIn);
}

void Bootstrap::Run(
    const std::function<std::unique_ptr<Game>(const std::shared_ptr<GameHost>&)>& createApp)
{
    using pomdog::detail::win32::GameHostWin32;
    using pomdog::detail::win32::GameWindowWin32;

    PresentationParameters presentationParameters;
    presentationParameters.BackBufferHeight = backBufferHeight;
    presentationParameters.BackBufferWidth = backBufferWidth;
    presentationParameters.PresentationInterval = presentationInterval;
    presentationParameters.BackBufferFormat = surfaceFormat;
    presentationParameters.DepthStencilFormat = depthFormat;
    presentationParameters.MultiSampleCount = 1;
    presentationParameters.IsFullScreen = isFullScreen;

    auto eventQueue = std::make_shared<EventQueue<SystemEvent>>();

    const bool useOpenGL = openGLEnabled;

    auto gameWindow = std::make_shared<GameWindowWin32>();
    if (auto err = gameWindow->Initialize(
            hInstance,
            cmdShow,
            icon,
            iconSmall,
            useOpenGL,
            eventQueue,
            presentationParameters);
        err != nullptr) {
        if (onError != nullptr) {
            onError(errors::Wrap(std::move(err), "GameWindowWin32::Initialize() failed"));
        }
        return;
    }

    auto gamepad = std::make_shared<GamepadDirectInput>();
    if (auto err = gamepad->Initialize(hInstance, gameWindow->GetNativeWindowHandle()); err != nullptr) {
        if (onError != nullptr) {
            onError(errors::Wrap(std::move(err), "GamepadDirectInput::Initialize() failed"));
        }
        return;
    }

    auto gameHost = std::make_shared<GameHostWin32>();

    if (auto err = gameHost->Initialize(
            gameWindow,
            eventQueue,
            gamepad,
            presentationParameters,
            useOpenGL);
        err != nullptr) {
        if (onError != nullptr) {
            onError(errors::Wrap(std::move(err), "GameHostWin32::Initialize() failed"));
        }
        return;
    }

    POMDOG_ASSERT(createApp);
    auto game = createApp(gameHost);
    if (game == nullptr) {
        if (onError != nullptr) {
            onError(errors::New("game must be != nullptr"));
        }
        return;
    }

    POMDOG_ASSERT(game != nullptr);
    if (auto err = game->Initialize(); err != nullptr) {
        if (onError != nullptr) {
            onError(errors::Wrap(std::move(err), "failed to initialize game"));
        }
        return;
    }

    gameHost->Run(*game);

    gameHost.reset();
    gameWindow.reset();
}

} // namespace pomdog::win32
