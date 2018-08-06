// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GameHostWin32.hpp"
#include "GameWindowWin32.hpp"
#include "KeyboardWin32.hpp"
#include "MouseWin32.hpp"
#include "../InputSystem/NativeGamepad.hpp"
#if !defined(POMDOG_DISABLE_GL4)
#include "../Platform.Win32/OpenGLContextWin32.hpp"
#include "../RenderSystem.GL4/GraphicsContextGL4.hpp"
#include "../RenderSystem.GL4/GraphicsDeviceGL4.hpp"
#include "../RenderSystem/GraphicsCommandQueueImmediate.hpp"
#endif
#if !defined(POMDOG_DISABLE_DIRECT3D11)
#include "../RenderSystem.Direct3D11/GraphicsContextDirect3D11.hpp"
#include "../RenderSystem.Direct3D11/GraphicsDeviceDirect3D11.hpp"
#include "../RenderSystem/GraphicsCommandQueueImmediate.hpp"
#endif
#include "../Application/SubsystemScheduler.hpp"
#include "../Application/SystemEvents.hpp"
#include "../SoundSystem.XAudio2/AudioEngineXAudio2.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Audio/AudioEngine.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/GraphicsCommandQueue.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Signals/EventQueue.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include <chrono>
#include <thread>

using Pomdog::Detail::InputSystem::NativeGamepad;
using Pomdog::Detail::Win32::GameWindowWin32;
#if !defined(POMDOG_DISABLE_GL4)
using Pomdog::Detail::GL4::GraphicsContextGL4;
using Pomdog::Detail::GL4::GraphicsDeviceGL4;
#endif
#if !defined(POMDOG_DISABLE_DIRECT3D11)
using Pomdog::Detail::Direct3D11::GraphicsContextDirect3D11;
using Pomdog::Detail::Direct3D11::GraphicsDeviceDirect3D11;
#endif

namespace Pomdog {
namespace Detail {
namespace Win32 {
namespace {

void MessagePump()
{
    MSG msg;
    while (0 != ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

struct GraphicsBridgeWin32 {
    virtual ~GraphicsBridgeWin32() = default;
    virtual void OnClientSizeChanged(int width, int height) = 0;
};

using CreateGraphicsDeviceResult = std::tuple<
    std::shared_ptr<GraphicsDevice>,
    std::shared_ptr<GraphicsCommandQueue>,
    std::unique_ptr<GraphicsBridgeWin32>>;

#if !defined(POMDOG_DISABLE_GL4)

class GraphicsBridgeWin32GL4 final : public GraphicsBridgeWin32 {
private:
    std::shared_ptr<GraphicsDeviceGL4> graphicsDevice;

public:
    explicit GraphicsBridgeWin32GL4(const std::shared_ptr<GraphicsDeviceGL4>& graphicsDeviceIn)
        : graphicsDevice(graphicsDeviceIn)
    {
    }

    void OnClientSizeChanged(int width, int height)
    {
        POMDOG_ASSERT(graphicsDevice);
        graphicsDevice->ClientSizeChanged(width, height);
    }
};

CreateGraphicsDeviceResult CreateGraphicsDeviceGL4(
    const std::shared_ptr<GameWindowWin32>& window,
    const PresentationParameters& presentationParameters)
{
    auto openGLContext = std::make_shared<Win32::OpenGLContextWin32>(
        window->NativeWindowHandle(), presentationParameters);

    if (glewInit() != GLEW_OK) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to initialize glew.");
    }

    openGLContext->MakeCurrent();

    auto graphicsDevice = std::make_shared<GraphicsDevice>(
        std::make_unique<GraphicsDeviceGL4>(presentationParameters));

    auto graphicsContext = std::make_shared<GraphicsContextGL4>(openGLContext, window);

    auto graphicsCommandQueue = std::make_shared<GraphicsCommandQueue>(
        std::make_unique<GraphicsCommandQueueImmediate>(graphicsContext));

    POMDOG_ASSERT(graphicsDevice);
    POMDOG_ASSERT(graphicsContext);
    POMDOG_ASSERT(graphicsCommandQueue);

    auto sharedNativeDevice = std::shared_ptr<GraphicsDeviceGL4>(
        graphicsDevice,
        static_cast<GraphicsDeviceGL4*>(graphicsDevice->GetNativeGraphicsDevice()));

    POMDOG_ASSERT(sharedNativeDevice);

    return CreateGraphicsDeviceResult{
        std::move(graphicsDevice),
        std::move(graphicsCommandQueue),
        std::make_unique<GraphicsBridgeWin32GL4>(std::move(sharedNativeDevice)),
    };
}
#endif

#if !defined(POMDOG_DISABLE_DIRECT3D11)

class GraphicsBridgeWin32Direct3D11 final : public GraphicsBridgeWin32 {
private:
    std::shared_ptr<GraphicsDeviceDirect3D11> graphicsDevice;
    std::shared_ptr<GraphicsContextDirect3D11> graphicsContext;

public:
    GraphicsBridgeWin32Direct3D11(
        const std::shared_ptr<GraphicsDeviceDirect3D11>& graphicsDeviceIn,
        const std::shared_ptr<GraphicsContextDirect3D11>& graphicsContextIn)
        : graphicsDevice(graphicsDeviceIn)
        , graphicsContext(graphicsContextIn)
    {
    }

    void OnClientSizeChanged(int width, int height)
    {
        POMDOG_ASSERT(graphicsDevice);
        POMDOG_ASSERT(graphicsContext);
        auto device = graphicsDevice->GetDevice();
        graphicsContext->ResizeBackBuffers(device.Get(), width, height);
        graphicsDevice->ClientSizeChanged(width, height);
    }
};

CreateGraphicsDeviceResult CreateGraphicsDeviceDirect3D11(
    const std::shared_ptr<GameWindowWin32>& window,
    const PresentationParameters& presentationParameters)
{
    auto nativeGraphicsDevice = std::make_unique<GraphicsDeviceDirect3D11>(presentationParameters);
    auto device = nativeGraphicsDevice->GetDevice();
    auto deviceContext = nativeGraphicsDevice->GetDeviceContext();
    auto dxgiFactory = nativeGraphicsDevice->GetDXGIFactory();

    auto graphicsDevice = std::make_shared<GraphicsDevice>(
        std::move(nativeGraphicsDevice));

    auto graphicsContext = std::make_shared<GraphicsContextDirect3D11>(
        window->NativeWindowHandle(),
        dxgiFactory,
        device,
        deviceContext,
        presentationParameters);

    auto graphicsCommandQueue = std::make_shared<GraphicsCommandQueue>(
        std::make_unique<GraphicsCommandQueueImmediate>(graphicsContext));

    POMDOG_ASSERT(graphicsDevice);
    POMDOG_ASSERT(graphicsContext);
    POMDOG_ASSERT(graphicsCommandQueue);

    auto sharedNativeDevice = std::shared_ptr<GraphicsDeviceDirect3D11>(
        graphicsDevice,
        static_cast<GraphicsDeviceDirect3D11*>(graphicsDevice->GetNativeGraphicsDevice()));

    POMDOG_ASSERT(sharedNativeDevice);

    return CreateGraphicsDeviceResult{
        std::move(graphicsDevice),
        std::move(graphicsCommandQueue),
        std::make_unique<GraphicsBridgeWin32Direct3D11>(
            std::move(sharedNativeDevice),
            std::move(graphicsContext)),
    };
}
#endif

} // unnamed namespace

class GameHostWin32::Impl final {
public:
    Impl(
        const std::shared_ptr<GameWindowWin32>& window,
        const std::shared_ptr<EventQueue>& eventQueue,
        const std::shared_ptr<InputSystem::NativeGamepad>& gamepad,
        const PresentationParameters& presentationParameters,
        bool useOpenGL);

    ~Impl();

    void Run(Game & game);

    void Exit();

    std::shared_ptr<GameWindow> GetWindow();

    std::shared_ptr<GameClock> GetClock(std::shared_ptr<GameHost> && gameHost);

    std::shared_ptr<GraphicsCommandQueue> GetGraphicsCommandQueue();

    std::shared_ptr<GraphicsDevice> GetGraphicsDevice();

    std::shared_ptr<AudioEngine> GetAudioEngine();

    std::shared_ptr<AssetManager> GetAssetManager(std::shared_ptr<GameHost> && gameHost);

    std::shared_ptr<Keyboard> GetKeyboard();

    std::shared_ptr<Mouse> GetMouse();

    std::shared_ptr<Gamepad> GetGamepad();

private:
    void RenderFrame(Game & game);

    void DoEvents();

    void ProcessSystemEvents(const Event& event);

    void ClientSizeChanged();

private:
    GameClock clock;
    Detail::SubsystemScheduler subsystemScheduler;
    ScopedConnection systemEventConnection;

    std::shared_ptr<EventQueue> eventQueue;
    std::shared_ptr<GameWindowWin32> window;

    std::unique_ptr<GraphicsBridgeWin32> graphicsBridge;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> graphicsCommandQueue;
    std::unique_ptr<Pomdog::AssetManager> assetManager;
    std::shared_ptr<AudioEngine> audioEngine;

    std::shared_ptr<KeyboardWin32> keyboard;
    std::shared_ptr<MouseWin32> mouse;
    std::shared_ptr<NativeGamepad> gamepad;

    Duration presentationInterval;
    SurfaceFormat backBufferSurfaceFormat;
    DepthFormat backBufferDepthStencilFormat;
    bool exitRequest;
    bool surfaceResizeRequest;
};

GameHostWin32::Impl::Impl(
    const std::shared_ptr<GameWindowWin32>& windowIn,
    const std::shared_ptr<EventQueue>& eventQueueIn,
    const std::shared_ptr<InputSystem::NativeGamepad>& gamepadIn,
    const PresentationParameters& presentationParameters,
    bool useOpenGL)
    : eventQueue(eventQueueIn)
    , window(windowIn)
    , backBufferSurfaceFormat(presentationParameters.BackBufferFormat)
    , backBufferDepthStencilFormat(presentationParameters.DepthStencilFormat)
    , exitRequest(false)
    , surfaceResizeRequest(false)
{
    POMDOG_ASSERT(presentationParameters.PresentationInterval > 0);
    presentationInterval = Duration(1.0) / presentationParameters.PresentationInterval;

#if !defined(POMDOG_DISABLE_GL4)
    if (useOpenGL) {
        auto result = CreateGraphicsDeviceGL4(window, presentationParameters);
        graphicsDevice = std::move(std::get<0>(result));
        graphicsCommandQueue = std::move(std::get<1>(result));
        graphicsBridge = std::move(std::get<2>(result));
    }
#endif
#if !defined(POMDOG_DISABLE_DIRECT3D11)
    if (!useOpenGL) {
        auto result = CreateGraphicsDeviceDirect3D11(window, presentationParameters);
        graphicsDevice = std::move(std::get<0>(result));
        graphicsCommandQueue = std::move(std::get<1>(result));
        graphicsBridge = std::move(std::get<2>(result));
    }
#endif

    POMDOG_ASSERT(eventQueue);
    systemEventConnection = eventQueue->Connect([this](const Event& event) {
        ProcessSystemEvents(event);
    });

    audioEngine = std::make_shared<Pomdog::AudioEngine>();

    keyboard = std::make_shared<KeyboardWin32>();
    mouse = std::make_shared<MouseWin32>(window->NativeWindowHandle());
    gamepad = gamepadIn;

    Detail::AssetLoaderContext loaderContext;
    loaderContext.RootDirectory = PathHelper::Join(
        FileSystem::GetResourceDirectoryPath(), "Content");
    loaderContext.GraphicsDevice = graphicsDevice;
    assetManager = std::make_unique<Pomdog::AssetManager>(std::move(loaderContext));
}

GameHostWin32::Impl::~Impl()
{
    eventQueue.reset();
    assetManager.reset();
    gamepad.reset();
    keyboard.reset();
    mouse.reset();
    audioEngine.reset();
    graphicsBridge.reset();
    graphicsCommandQueue.reset();
    graphicsDevice.reset();
    window.reset();
}

void GameHostWin32::Impl::Run(Game & game)
{
    game.Initialize();

    while (!exitRequest) {
        clock.Tick();
        MessagePump();
        DoEvents();
        constexpr int64_t gamepadDetectionInterval = 240;
        if (((clock.GetFrameNumber() % gamepadDetectionInterval) == 16) && (clock.GetFrameRate() >= 30.0f)) {
            gamepad->EnumerateDevices();
        }
        gamepad->PollEvents();
        subsystemScheduler.OnUpdate();
        game.Update();
        RenderFrame(game);

        auto elapsedTime = clock.GetElapsedTime();

        if (elapsedTime < presentationInterval) {
            auto sleepTime = presentationInterval - elapsedTime;
            using namespace std::chrono;
            std::this_thread::sleep_for(duration_cast<milliseconds>(sleepTime));
        }
    }

    window->Close();
    //DoEvents();
    MessagePump();
}

void GameHostWin32::Impl::Exit()
{
    exitRequest = true;
}

void GameHostWin32::Impl::RenderFrame(Game & game)
{
    POMDOG_ASSERT(window);

    if (!window || window->IsMinimized()) {
        // skip rendering
        return;
    }

    game.Draw();
}

void GameHostWin32::Impl::DoEvents()
{
    eventQueue->Emit();

    if (surfaceResizeRequest) {
        ClientSizeChanged();
        surfaceResizeRequest = false;
    }
}

void GameHostWin32::Impl::ProcessSystemEvents(const Event& event)
{
    if (event.Is<WindowShouldCloseEvent>()) {
        Log::Internal("WindowShouldCloseEvent");
        this->Exit();
    }
    else if (event.Is<ViewDidEndLiveResizeEvent>()) {
        surfaceResizeRequest = true;
    }
    else if (auto keyboardEvent = event.As<RAWKEYBOARD>()) {
        keyboard->HandleMessage(*keyboardEvent);
    }
    else if (auto mouseEvent = event.As<RAWMOUSE>()) {
        mouse->HandleMessage(*mouseEvent);
    }
}

void GameHostWin32::Impl::ClientSizeChanged()
{
    POMDOG_ASSERT(window);
    auto bounds = window->GetClientBounds();

    POMDOG_ASSERT(graphicsBridge);
    graphicsBridge->OnClientSizeChanged(bounds.Width, bounds.Height);
    window->ClientSizeChanged(bounds.Width, bounds.Height);
}

std::shared_ptr<GameWindow> GameHostWin32::Impl::GetWindow()
{
    return window;
}

std::shared_ptr<GameClock> GameHostWin32::Impl::GetClock(
    std::shared_ptr<GameHost> && gameHost)
{
    std::shared_ptr<GameClock> sharedClock(std::move(gameHost), &clock);
    return std::move(sharedClock);
}

std::shared_ptr<GraphicsCommandQueue> GameHostWin32::Impl::GetGraphicsCommandQueue()
{
    POMDOG_ASSERT(graphicsCommandQueue);
    return graphicsCommandQueue;
}

std::shared_ptr<GraphicsDevice> GameHostWin32::Impl::GetGraphicsDevice()
{
    POMDOG_ASSERT(graphicsDevice);
    return graphicsDevice;
}

std::shared_ptr<AudioEngine> GameHostWin32::Impl::GetAudioEngine()
{
    POMDOG_ASSERT(audioEngine);
    return audioEngine;
}

std::shared_ptr<AssetManager> GameHostWin32::Impl::GetAssetManager(
    std::shared_ptr<GameHost> && gameHost)
{
    POMDOG_ASSERT(assetManager);
    std::shared_ptr<AssetManager> sharedAssetManager(std::move(gameHost), assetManager.get());
    return std::move(sharedAssetManager);
}

std::shared_ptr<Keyboard> GameHostWin32::Impl::GetKeyboard()
{
    POMDOG_ASSERT(keyboard);
    return keyboard;
}

std::shared_ptr<Mouse> GameHostWin32::Impl::GetMouse()
{
    POMDOG_ASSERT(mouse);
    return mouse;
}

std::shared_ptr<Gamepad> GameHostWin32::Impl::GetGamepad()
{
    POMDOG_ASSERT(gamepad);
    return gamepad;
}

GameHostWin32::GameHostWin32(
    const std::shared_ptr<GameWindowWin32>& window,
    const std::shared_ptr<EventQueue>& eventQueue,
    const std::shared_ptr<InputSystem::NativeGamepad>& gamepad,
    const PresentationParameters& presentationParameters,
    bool useOpenGL)
    : impl(std::make_unique<Impl>(
        window,
        eventQueue,
        gamepad,
        presentationParameters,
        useOpenGL))
{
}

GameHostWin32::~GameHostWin32() = default;

void GameHostWin32::Run(Game & game)
{
    POMDOG_ASSERT(impl);
    impl->Run(game);
}

void GameHostWin32::Exit()
{
    POMDOG_ASSERT(impl);
    impl->Exit();
}

std::shared_ptr<GameWindow> GameHostWin32::GetWindow()
{
    POMDOG_ASSERT(impl);
    return impl->GetWindow();
}

std::shared_ptr<GameClock> GameHostWin32::GetClock()
{
    POMDOG_ASSERT(impl);
    return impl->GetClock(shared_from_this());
}

std::shared_ptr<GraphicsCommandQueue> GameHostWin32::GetGraphicsCommandQueue()
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsCommandQueue();
}

std::shared_ptr<GraphicsDevice> GameHostWin32::GetGraphicsDevice()
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsDevice();
}

std::shared_ptr<AudioEngine> GameHostWin32::GetAudioEngine()
{
    POMDOG_ASSERT(impl);
    return impl->GetAudioEngine();
}

std::shared_ptr<AssetManager> GameHostWin32::GetAssetManager()
{
    POMDOG_ASSERT(impl);
    return impl->GetAssetManager(shared_from_this());
}

std::shared_ptr<Keyboard> GameHostWin32::GetKeyboard()
{
    POMDOG_ASSERT(impl);
    return impl->GetKeyboard();
}

std::shared_ptr<Mouse> GameHostWin32::GetMouse()
{
    POMDOG_ASSERT(impl);
    return impl->GetMouse();
}

std::shared_ptr<Gamepad> GameHostWin32::GetGamepad()
{
    POMDOG_ASSERT(impl);
    return impl->GetGamepad();
}

} // namespace Win32
} // namespace Detail
} // namespace Pomdog
