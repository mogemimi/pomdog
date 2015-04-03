// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Platform/Win32/BootstrapperWin32.hpp"
#include "GameHostWin32.hpp"
#include "GameWindowWin32.hpp"
#include "../InputSystem.DirectInput/DeviceContextDirectInput.hpp"
#include "../InputSystem.DirectInput/KeyboardCreatorDirectInput.hpp"
#include "../InputSystem.DirectInput/MouseCreatorDirectInput.hpp"
#include "../InputSystem/InputDeviceFactory.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include "Pomdog/Logging/Log.hpp"

namespace Pomdog {
namespace Detail {
namespace Win32 {
//-----------------------------------------------------------------------
BootstrapperWin32::BootstrapperWin32(BootstrapSettingsWin32 const& settingsIn)
    : settings(settingsIn)
{}
//-----------------------------------------------------------------------
void BootstrapperWin32::Run(std::function<void(std::shared_ptr<GameHost> const& gameHost)> const& runGame)
{
    PresentationParameters presentationParameters;
    presentationParameters.BackBufferHeight = settings.Height;
    presentationParameters.BackBufferWidth = settings.Width;
    presentationParameters.PresentationInterval = 60;
    presentationParameters.SurfaceFormat = SurfaceFormat::R8G8B8A8_UNorm;
    presentationParameters.DepthFormat = DepthFormat::Depth24Stencil8;
    presentationParameters.IsFullScreen = settings.IsFullscreen;

    auto eventQueue = std::make_shared<EventQueue>();

#if defined(POMDOG_RENDERSYSTEM_GL4)
    bool useOpenGL = true;
#else
    bool useOpenGL = false;
#endif

    auto gameWindow = std::make_shared<GameWindowWin32>(settings.HInstance, settings.CmdShow, settings.Icon, settings.IconSmall,
        useOpenGL, eventQueue, presentationParameters);

    auto inputDeviceFactory = std::make_unique<Detail::InputSystem::InputDeviceFactory>();

    {
        using namespace Detail::InputSystem::DirectInput;
        auto deviceContext = std::make_shared<DeviceContextDirectInput>(
            settings.HInstance, gameWindow->NativeWindowHandle());

        auto keyboardCreator = std::make_unique<KeyboardCreatorDirectInput>(deviceContext);
        inputDeviceFactory->AddCreator(std::move(keyboardCreator));

        auto mouseCreator = std::make_unique<MouseCreatorDirectInput>(deviceContext);
        inputDeviceFactory->AddCreator(std::move(mouseCreator));
    }

    auto gameHost = std::make_shared<GameHostWin32>(gameWindow,
        eventQueue, presentationParameters, std::move(inputDeviceFactory));

    POMDOG_ASSERT(runGame);
    runGame(gameHost);

    gameHost.reset();
    gameWindow.reset();
}
//-----------------------------------------------------------------------
}// namespace Win32
}// namespace Detail
}// namespace Pomdog
