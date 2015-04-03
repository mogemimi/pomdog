// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "GameHostWin32.hpp"
#include "GameWindowWin32.hpp"
#include "../Application/SystemEvents.hpp"
#include "../InputSystem/InputDeviceFactory.hpp"

#if defined(POMDOG_ENABLE_DIRECT3D11)
#elif defined(POMDOG_ENABLE_GL4)
#else
#error "Cannot find render system"
#endif

#if defined(POMDOG_ENABLE_GL4)
#include "../Platform.Win32/OpenGLContextWin32.hpp"
#include "../RenderSystem.GL4/GraphicsContextGL4.hpp"
#include "../RenderSystem.GL4/GraphicsDeviceGL4.hpp"
#endif
#if defined(POMDOG_ENABLE_DIRECT3D11)
#include "../RenderSystem.Direct3D11/GraphicsContextDirect3D11.hpp"
#include "../RenderSystem.Direct3D11/GraphicsDeviceDirect3D11.hpp"
#endif

#include "../SoundSystem.XAudio2/AudioEngineXAudio2.hpp"
#include "../Application/SubsystemScheduler.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Audio/AudioEngine.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Event/ScopedConnection.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Logging/Log.hpp"

#include <thread>
#include <chrono>

namespace Pomdog {
namespace Detail {
namespace Win32 {
namespace {

static void MessagePump()
{
    MSG msg;
    while (0 != ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

}// unnamed namespace
//-----------------------------------------------------------------------
class GameHostWin32::Impl final {
public:
    Impl(std::shared_ptr<GameWindowWin32> const& window,
        std::shared_ptr<EventQueue> const& eventQueue,
        PresentationParameters const& presentationParameters,
        std::unique_ptr<InputSystem::InputDeviceFactory> && inputDeviceFactory,
        bool useOpenGL);

    void Run(Game & game);

    void Exit();

    std::shared_ptr<Pomdog::GameWindow> GetWindow();

    std::shared_ptr<Pomdog::GameClock> GetClock(std::shared_ptr<GameHost> && gameHost);

    std::shared_ptr<Pomdog::GraphicsContext> GetGraphicsContext();

    std::shared_ptr<Pomdog::GraphicsDevice> GetGraphicsDevice();

    std::shared_ptr<Pomdog::AudioEngine> GetAudioEngine();

    std::shared_ptr<Pomdog::AssetManager> GetAssetManager(std::shared_ptr<GameHost> && gameHost);

    std::shared_ptr<Pomdog::Keyboard> GetKeyboard();

    std::shared_ptr<Pomdog::Mouse> GetMouse();

private:
    void RenderFrame(Game & game);

    void DoEvents();

    void ProcessSystemEvents(Event const& event);

    void ClientSizeChanged();

private:
    GameClock clock;
    Detail::SubsystemScheduler subsystemScheduler;
    ScopedConnection systemEventConnection;

    std::shared_ptr<EventQueue> eventQueue;
    std::shared_ptr<GameWindowWin32> window;

    std::shared_ptr<Pomdog::GraphicsContext> graphicsContext;
    std::shared_ptr<Pomdog::GraphicsDevice> graphicsDevice;
    std::unique_ptr<Pomdog::AssetManager> assetManager;
    std::shared_ptr<Pomdog::AudioEngine> audioEngine;

    std::unique_ptr<InputSystem::InputDeviceFactory> inputDeviceFactory;
    std::shared_ptr<Pomdog::Keyboard> keyboard;
    std::shared_ptr<Pomdog::Mouse> mouse;

    Duration presentationInterval;

    bool exitRequest;
    bool surfaceResizeRequest;
    bool useOpenGL;
};
//-----------------------------------------------------------------------
GameHostWin32::Impl::Impl(std::shared_ptr<GameWindowWin32> const& windowIn,
    std::shared_ptr<EventQueue> const& eventQueueIn,
    PresentationParameters const& presentationParameters,
    std::unique_ptr<InputSystem::InputDeviceFactory> && inputDeviceFactoryIn,
    bool useOpenGLIn)
    : eventQueue(eventQueueIn)
    , window(windowIn)
    , inputDeviceFactory(std::move(inputDeviceFactoryIn))
    , exitRequest(false)
    , surfaceResizeRequest(false)
    , useOpenGL(useOpenGLIn)
{
    POMDOG_ASSERT(presentationParameters.PresentationInterval > 0);
    presentationInterval = Duration(1.0) / presentationParameters.PresentationInterval;

#if defined(POMDOG_ENABLE_GL4)
    if (useOpenGL) {
        using Detail::RenderSystem::GL4::GraphicsDeviceGL4;
        using Detail::RenderSystem::GL4::GraphicsContextGL4;

        auto openGLContext = std::make_shared<Win32::OpenGLContextWin32>(
            window->NativeWindowHandle(), presentationParameters);

        if (glewInit() != GLEW_OK)
        {
            //POMDOG_THROW_EXCEPTION(std::runtime_error,
            //    "Failed to initialize glew.");
        }

        openGLContext->MakeCurrent();

        auto nativeGraphicsDevice = std::make_unique<GraphicsDeviceGL4>();
        graphicsDevice = std::make_shared<Pomdog::GraphicsDevice>(std::move(nativeGraphicsDevice));

        graphicsContext = std::make_shared<Pomdog::GraphicsContext>(
            std::make_unique<GraphicsContextGL4>(openGLContext, window),
            presentationParameters, graphicsDevice);
    }
#endif
#if defined(POMDOG_ENABLE_DIRECT3D11)
    if (!useOpenGL) {
        using Detail::RenderSystem::Direct3D11::GraphicsDeviceDirect3D11;
        using Detail::RenderSystem::Direct3D11::GraphicsContextDirect3D11;

        auto nativeGraphicsDevice = std::make_unique<GraphicsDeviceDirect3D11>();
        auto deviceContext = nativeGraphicsDevice->DeviceContext();
        auto nativeDevice = nativeGraphicsDevice->NativeDevice();
        auto dxgiFactory = nativeGraphicsDevice->DXGIFactory();

        graphicsDevice = std::make_shared<Pomdog::GraphicsDevice>(std::move(nativeGraphicsDevice));

        graphicsContext = std::make_shared<Pomdog::GraphicsContext>(
            std::make_unique<GraphicsContextDirect3D11>(window->NativeWindowHandle(), dxgiFactory, nativeDevice, deviceContext),
            presentationParameters, graphicsDevice);
    }
#endif

    POMDOG_ASSERT(eventQueue);
    systemEventConnection = eventQueue->Connect([this](Event const& event) {
        ProcessSystemEvents(event);
    });

    audioEngine = std::make_shared<Pomdog::AudioEngine>();

    POMDOG_ASSERT(inputDeviceFactory);
    keyboard = inputDeviceFactory->CreateKeyboard(subsystemScheduler);
    mouse = inputDeviceFactory->CreateMouse(subsystemScheduler);

    {
        std::string rootDirectory = "Content";
        Detail::AssetLoaderContext loaderContext{ rootDirectory, graphicsDevice };

        assetManager = std::make_unique<Pomdog::AssetManager>(std::move(loaderContext));
    }
}
//-----------------------------------------------------------------------
void GameHostWin32::Impl::Run(Game & game)
{
    game.Initialize();

    if (!game.CompleteInitialize()) {
        return;
    }

    while (!exitRequest)
    {
        clock.Tick();
        MessagePump();
        DoEvents();
        subsystemScheduler.OnUpdate();
        game.Update();
        RenderFrame(game);

        auto elapsedTime = clock.ElapsedTime();

        if (elapsedTime < presentationInterval) {
            auto sleepTime = std::chrono::duration_cast<std::chrono::milliseconds>(presentationInterval - elapsedTime);
            std::this_thread::sleep_for(sleepTime);
        }
    }

    window->Close();
    //DoEvents();
    MessagePump();
}
//-----------------------------------------------------------------------
void GameHostWin32::Impl::Exit()
{
    exitRequest = true;
}
//-----------------------------------------------------------------------
void GameHostWin32::Impl::RenderFrame(Game & game)
{
    POMDOG_ASSERT(window);

    if (!window || window->IsMinimized()) {
        // skip rendering
        return;
    }

    game.Draw();
}
//-----------------------------------------------------------------------
void GameHostWin32::Impl::DoEvents()
{
    eventQueue->Tick();

    if (surfaceResizeRequest) {
        ClientSizeChanged();
        surfaceResizeRequest = false;
    }
}
//-----------------------------------------------------------------------
void GameHostWin32::Impl::ProcessSystemEvents(Event const& event)
{
    if (event.Is<WindowShouldCloseEvent>())
    {
        Log::Internal("WindowShouldCloseEvent");
        this->Exit();
    }
    else if (event.Is<ViewDidEndLiveResizeEvent>())
    {
        surfaceResizeRequest = true;
    }
}
//-----------------------------------------------------------------------
void GameHostWin32::Impl::ClientSizeChanged()
{
    POMDOG_ASSERT(window);
    auto bounds = window->ClientBounds();

#if defined(POMDOG_ENABLE_DIRECT3D11)
    if (!useOpenGL)
    {
        using Detail::RenderSystem::Direct3D11::GraphicsDeviceDirect3D11;
        using Detail::RenderSystem::Direct3D11::GraphicsContextDirect3D11;

        auto nativeGraphicsDevice = dynamic_cast<GraphicsDeviceDirect3D11*>(graphicsDevice->NativeGraphicsDevice());
        auto nativeGraphicsContext = dynamic_cast<GraphicsContextDirect3D11*>(graphicsContext->NativeGraphicsContext());

        POMDOG_ASSERT(nativeGraphicsDevice != nullptr);
        POMDOG_ASSERT(nativeGraphicsContext != nullptr);

        nativeGraphicsContext->ResizeBackBuffers(
            nativeGraphicsDevice->NativeDevice().Get(), bounds.Width, bounds.Height);
    }
#endif

    window->ClientSizeChanged(bounds.Width, bounds.Height);
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameWindow> GameHostWin32::Impl::GetWindow()
{
    return window;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameClock> GameHostWin32::Impl::GetClock(std::shared_ptr<GameHost> && gameHost)
{
    std::shared_ptr<Pomdog::GameClock> sharedClock(gameHost, &clock);
    return std::move(sharedClock);
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsContext> GameHostWin32::Impl::GetGraphicsContext()
{
    POMDOG_ASSERT(graphicsContext);
    return graphicsContext;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsDevice> GameHostWin32::Impl::GetGraphicsDevice()
{
    POMDOG_ASSERT(graphicsDevice);
    return graphicsDevice;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AudioEngine> GameHostWin32::Impl::GetAudioEngine()
{
    POMDOG_ASSERT(audioEngine);
    return audioEngine;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AssetManager> GameHostWin32::Impl::GetAssetManager(std::shared_ptr<GameHost> && gameHost)
{
    POMDOG_ASSERT(assetManager);
    std::shared_ptr<Pomdog::AssetManager> sharedAssetManager(gameHost, assetManager.get());
    return std::move(sharedAssetManager);
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Keyboard> GameHostWin32::Impl::GetKeyboard()
{
    POMDOG_ASSERT(keyboard);
    return keyboard;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Mouse> GameHostWin32::Impl::GetMouse()
{
    POMDOG_ASSERT(mouse);
    return mouse;
}
//-----------------------------------------------------------------------
GameHostWin32::GameHostWin32(std::shared_ptr<GameWindowWin32> const& window,
    std::shared_ptr<EventQueue> const& eventQueue,
    PresentationParameters const& presentationParameters,
    std::unique_ptr<InputSystem::InputDeviceFactory> && inputDeviceFactory,
    bool enableOpenGL)
    : impl(std::make_unique<Impl>(window, eventQueue, presentationParameters, std::move(inputDeviceFactory), enableOpenGL))
{}
//-----------------------------------------------------------------------
GameHostWin32::~GameHostWin32() = default;
//-----------------------------------------------------------------------
void GameHostWin32::Run(Game & game)
{
    POMDOG_ASSERT(impl);
    impl->Run(game);
}
//-----------------------------------------------------------------------
void GameHostWin32::Exit()
{
    POMDOG_ASSERT(impl);
    impl->Exit();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameWindow> GameHostWin32::Window()
{
    POMDOG_ASSERT(impl);
    return impl->GetWindow();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameClock> GameHostWin32::Clock()
{
    POMDOG_ASSERT(impl);
    return impl->GetClock(shared_from_this());
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsContext> GameHostWin32::GraphicsContext()
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsContext();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsDevice> GameHostWin32::GraphicsDevice()
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsDevice();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AudioEngine> GameHostWin32::AudioEngine()
{
    POMDOG_ASSERT(impl);
    return impl->GetAudioEngine();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AssetManager> GameHostWin32::AssetManager()
{
    POMDOG_ASSERT(impl);
    return impl->GetAssetManager(shared_from_this());
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Keyboard> GameHostWin32::Keyboard()
{
    POMDOG_ASSERT(impl);
    return impl->GetKeyboard();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Mouse> GameHostWin32::Mouse()
{
    POMDOG_ASSERT(impl);
    return impl->GetMouse();
}
//-----------------------------------------------------------------------
}// namespace Win32
}// namespace Detail
}// namespace Pomdog
