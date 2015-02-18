// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Platform/Win32/Bootstrap.hpp"
#include "GameHostWin32.hpp"
#include "GameWindowWin32.hpp"
#include "../InputSystem.DirectInput/GamepadDirectInput.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Logging/Log.hpp"

using Pomdog::Detail::InputSystem::DirectInput::GamepadDirectInput;

namespace Pomdog {
namespace Win32 {

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

void Bootstrap::SetDepthFormat(DepthFormat depthFormatIn) noexcept
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

void Bootstrap::OnError(std::function<void(const std::exception&)> onErrorIn)
{
    onError = onErrorIn;
}

void Bootstrap::Run(
    const std::function<std::unique_ptr<Game>(const std::shared_ptr<GameHost>&)>& createApp)
{
    using Pomdog::Detail::Win32::GameHostWin32;
    using Pomdog::Detail::Win32::GameWindowWin32;

    PresentationParameters presentationParameters;
    presentationParameters.BackBufferHeight = backBufferHeight;
    presentationParameters.BackBufferWidth = backBufferWidth;
    presentationParameters.PresentationInterval = presentationInterval;
    presentationParameters.BackBufferFormat = surfaceFormat;
    presentationParameters.DepthStencilFormat = depthFormat;
    presentationParameters.MultiSampleCount = 1;
    presentationParameters.IsFullScreen = isFullScreen;

    auto eventQueue = std::make_shared<EventQueue>();

    const bool useOpenGL = openGLEnabled;

    auto gameWindow = std::make_shared<GameWindowWin32>(
        hInstance, cmdShow, icon, iconSmall,
        useOpenGL, eventQueue, presentationParameters);

    auto gamepad = std::make_shared<GamepadDirectInput>(
        hInstance, gameWindow->NativeWindowHandle());

    auto gameHost = std::make_shared<GameHostWin32>(
        gameWindow, eventQueue, gamepad, presentationParameters, useOpenGL);

    POMDOG_ASSERT(createApp);
    auto game = createApp(gameHost);

    POMDOG_ASSERT(game);
    if (game) {
        gameHost->Run(*game);
    }

    gameHost.reset();
    gameWindow.reset();
}

} // namespace Win32
} // namespace Pomdog
