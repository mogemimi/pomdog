// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Platform/Win32/Bootstrap.hpp"
#include "GameHostWin32.hpp"
#include "GameWindowWin32.hpp"
#include "../InputSystem.DirectInput/DeviceContextDirectInput.hpp"
#include "../InputSystem.DirectInput/KeyboardCreatorDirectInput.hpp"
#include "../InputSystem.DirectInput/MouseCreatorDirectInput.hpp"
#include "../InputSystem/InputDeviceFactory.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Logging/Log.hpp"

namespace Pomdog {
namespace Win32 {
//-----------------------------------------------------------------------
void Bootstrap::SetInstance(HINSTANCE hInstanceIn) noexcept
{
    hInstance = hInstanceIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetCommandShow(int cmdShowIn) noexcept
{
    cmdShow = cmdShowIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetIcon(HICON iconIn) noexcept
{
    icon = iconIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetIconSmall(HICON iconSmallIn) noexcept
{
    iconSmall = iconSmallIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetSurfaceFormat(SurfaceFormat surfaceFormatIn) noexcept
{
    surfaceFormat = surfaceFormatIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetDepthFormat(DepthFormat depthFormatIn) noexcept
{
    depthFormat = depthFormatIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetPresentationInterval(int presentationIntervalIn) noexcept
{
    presentationInterval = presentationIntervalIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetBackBufferSize(int width, int height) noexcept
{
    backBufferWidth = width;
    backBufferHeight = height;
}
//-----------------------------------------------------------------------
void Bootstrap::SetFullScreen(bool isFullScreenIn) noexcept
{
    isFullScreen = isFullScreenIn;
}
//-----------------------------------------------------------------------
void Bootstrap::SetOpenGLEnabled(bool openGLEnabledIn) noexcept
{
    openGLEnabled = openGLEnabledIn;
}
//-----------------------------------------------------------------------
void Bootstrap::OnError(std::function<void(std::exception const&)> onErrorIn)
{
    onError = onErrorIn;
}
//-----------------------------------------------------------------------
void Bootstrap::Run(std::function<std::unique_ptr<Game>(
    std::shared_ptr<GameHost> const&)> const& createApp)
{
    using Pomdog::Detail::Win32::GameHostWin32;
    using Pomdog::Detail::Win32::GameWindowWin32;

    PresentationParameters presentationParameters;
    presentationParameters.BackBufferHeight = backBufferHeight;
    presentationParameters.BackBufferWidth = backBufferWidth;
    presentationParameters.PresentationInterval = 60;
    presentationParameters.SurfaceFormat = surfaceFormat;
    presentationParameters.DepthFormat = depthFormat;
    presentationParameters.IsFullScreen = isFullScreen;

    auto eventQueue = std::make_shared<EventQueue>();

    const bool useOpenGL = openGLEnabled;

    auto gameWindow = std::make_shared<GameWindowWin32>(
        hInstance, cmdShow, icon, iconSmall,
        useOpenGL, eventQueue, presentationParameters);

    auto inputDeviceFactory = std::make_unique<Detail::InputSystem::InputDeviceFactory>();

    {
        using namespace Detail::InputSystem::DirectInput;
        auto deviceContext = std::make_shared<DeviceContextDirectInput>(
            hInstance, gameWindow->NativeWindowHandle());

        auto keyboardCreator = std::make_unique<KeyboardCreatorDirectInput>(deviceContext);
        inputDeviceFactory->AddCreator(std::move(keyboardCreator));

        auto mouseCreator = std::make_unique<MouseCreatorDirectInput>(deviceContext);
        inputDeviceFactory->AddCreator(std::move(mouseCreator));
    }

    auto gameHost = std::make_shared<GameHostWin32>(gameWindow,
        eventQueue, presentationParameters, std::move(inputDeviceFactory), useOpenGL);

    POMDOG_ASSERT(createApp);
    auto game = createApp(gameHost);
    
    POMDOG_ASSERT(game);
    if (game) {
        gameHost->Run(*game);
    }

    gameHost.reset();
    gameWindow.reset();
}
//-----------------------------------------------------------------------
} // namespace Win32
} // namespace Pomdog
