// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/platform/win32/game_host_win32.hpp"
#include "pomdog/input/backends/native_gamepad.hpp"
#include "pomdog/platform/win32/game_window_win32.hpp"
#include "pomdog/platform/win32/keyboard_win32.hpp"
#include "pomdog/platform/win32/mouse_win32.hpp"
#if !defined(POMDOG_DISABLE_GL4)
#include "pomdog/graphics/backends/graphics_command_queue_immediate.hpp"
#include "pomdog/graphics/gl4/graphics_context_gl4.hpp"
#include "pomdog/graphics/gl4/graphics_device_gl4.hpp"
#include "pomdog/platform/win32/opengl_context_win32.hpp"
#endif
#if !defined(POMDOG_DISABLE_DIRECT3D11)
#include "pomdog/graphics/backends/graphics_command_queue_immediate.hpp"
#include "pomdog/graphics/direct3d11/graphics_context_direct3d11.hpp"
#include "pomdog/graphics/direct3d11/graphics_device_direct3d11.hpp"
#endif
#include "pomdog/application/file_system.hpp"
#include "pomdog/application/game.hpp"
#include "pomdog/application/game_clock.hpp"
#include "pomdog/application/subsystem_scheduler.hpp"
#include "pomdog/application/system_events.hpp"
#include "pomdog/audio/xaudio2/audio_engine_xaudio2.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/content/asset_manager.hpp"
#include "pomdog/graphics/graphics_command_queue.hpp"
#include "pomdog/graphics/graphics_device.hpp"
#include "pomdog/graphics/presentation_parameters.hpp"
#include "pomdog/logging/log.hpp"
#include "pomdog/math/rectangle.hpp"
#include "pomdog/network/http_client.hpp"
#include "pomdog/network/io_service.hpp"
#include "pomdog/signals/event_queue.hpp"
#include "pomdog/signals/scoped_connection.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/path_helper.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using Pomdog::Detail::NativeGamepad;
using Pomdog::Detail::Win32::GameWindowWin32;
using Pomdog::Detail::XAudio2::AudioEngineXAudio2;
#if !defined(POMDOG_DISABLE_GL4)
using Pomdog::Detail::GL4::GraphicsContextGL4;
using Pomdog::Detail::GL4::GraphicsDeviceGL4;
#endif
#if !defined(POMDOG_DISABLE_DIRECT3D11)
using Pomdog::Detail::Direct3D11::GraphicsContextDirect3D11;
using Pomdog::Detail::Direct3D11::GraphicsDeviceDirect3D11;
#endif

namespace Pomdog::Detail::Win32 {
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
    virtual void Shutdown() = 0;
};

using CreateGraphicsDeviceResult = std::tuple<
    std::shared_ptr<GraphicsDevice>,
    std::shared_ptr<GraphicsCommandQueue>,
    std::unique_ptr<GraphicsBridgeWin32>,
    std::unique_ptr<Error>>;

#if !defined(POMDOG_DISABLE_GL4)

class GraphicsBridgeWin32GL4 final : public GraphicsBridgeWin32 {
private:
    std::shared_ptr<GraphicsDeviceGL4> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueueImmediate> commandQueue;

public:
    GraphicsBridgeWin32GL4(
        const std::shared_ptr<GraphicsDeviceGL4>& graphicsDeviceIn,
        const std::shared_ptr<GraphicsCommandQueueImmediate>& commandQueueIn)
        : graphicsDevice(graphicsDeviceIn)
        , commandQueue(commandQueueIn)
    {
    }

    void OnClientSizeChanged(int width, int height) override
    {
        POMDOG_ASSERT(graphicsDevice);
        graphicsDevice->ClientSizeChanged(width, height);
    }

    void Shutdown() override
    {
        if (commandQueue != nullptr) {
            commandQueue->Reset();
        }

        commandQueue.reset();
        graphicsDevice.reset();
    }
};

[[nodiscard]] CreateGraphicsDeviceResult
CreateGraphicsDeviceGL4(
    const std::shared_ptr<GameWindowWin32>& window,
    const PresentationParameters& presentationParameters)
{
    // NOTE: Create an OpenGL context.
    auto openGLContext = std::make_shared<Win32::OpenGLContextWin32>();
    if (auto err = openGLContext->Initialize(
            window->GetNativeWindowHandle(),
            presentationParameters);
        err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            Errors::Wrap(std::move(err), "OpenGLContextWin32::Initialize() failed."));
    }

    if (::glewInit() != GLEW_OK) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            Errors::New("glewInit() failed."));
    }

    openGLContext->MakeCurrent();

    // NOTE: Create a graphics device.
    auto graphicsDevice = std::make_shared<GraphicsDeviceGL4>();
    if (auto err = graphicsDevice->Initialize(presentationParameters); err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            Errors::Wrap(std::move(err), "GraphicsDeviceGL4::Initialize() failed."));
    }

    // NOTE: Create a graphics context.
    auto graphicsContext = std::make_shared<GraphicsContextGL4>();
    if (auto err = graphicsContext->Initialize(openGLContext, graphicsDevice); err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            Errors::Wrap(std::move(err), "GraphicsContextGL4::Initialize() failed."));
    }

    auto graphicsCommandQueue = std::make_shared<GraphicsCommandQueueImmediate>(graphicsContext);

    POMDOG_ASSERT(graphicsDevice);
    POMDOG_ASSERT(graphicsContext);
    POMDOG_ASSERT(graphicsCommandQueue);

    auto bridge = std::make_unique<GraphicsBridgeWin32GL4>(
        graphicsDevice,
        graphicsCommandQueue);

    return std::make_tuple(
        std::move(graphicsDevice),
        std::move(graphicsCommandQueue),
        std::move(bridge),
        nullptr);
}
#endif

#if !defined(POMDOG_DISABLE_DIRECT3D11)

class GraphicsBridgeWin32Direct3D11 final : public GraphicsBridgeWin32 {
private:
    std::shared_ptr<GraphicsDeviceDirect3D11> graphicsDevice;
    std::shared_ptr<GraphicsContextDirect3D11> graphicsContext;
    std::shared_ptr<GraphicsCommandQueueImmediate> commandQueue;

public:
    GraphicsBridgeWin32Direct3D11(
        const std::shared_ptr<GraphicsDeviceDirect3D11>& graphicsDeviceIn,
        const std::shared_ptr<GraphicsContextDirect3D11>& graphicsContextIn,
        const std::shared_ptr<GraphicsCommandQueueImmediate>& commandQueueIn)
        : graphicsDevice(graphicsDeviceIn)
        , graphicsContext(graphicsContextIn)
        , commandQueue(commandQueueIn)
    {
    }

    void OnClientSizeChanged(int width, int height) override
    {
        POMDOG_ASSERT(graphicsDevice);
        POMDOG_ASSERT(graphicsContext);
        auto device = graphicsDevice->GetDevice();
        if (auto err = graphicsContext->ResizeBackBuffers(device.Get(), width, height); err != nullptr) {
            // FIXME: Add error handling
            Log::Critical("Pomdog", "error: ResizeBackBuffers() failed: " + err->ToString());
        }
        graphicsDevice->ClientSizeChanged(width, height);
    }

    void Shutdown() override
    {
        if (commandQueue != nullptr) {
            commandQueue->Reset();
        }

        commandQueue.reset();
        graphicsContext.reset();
        graphicsDevice.reset();
    }
};

[[nodiscard]] CreateGraphicsDeviceResult
CreateGraphicsDeviceDirect3D11(
    const std::shared_ptr<GameWindowWin32>& window,
    const PresentationParameters& presentationParameters)
{
    auto graphicsDevice = std::make_shared<GraphicsDeviceDirect3D11>();
    if (auto err = graphicsDevice->Initialize(presentationParameters); err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            Errors::Wrap(std::move(err), "failed to initialize GraphicsDeviceDirect3D11"));
    }

    auto device = graphicsDevice->GetDevice();
    auto dxgiFactory = std::get<0>(graphicsDevice->GetDXGIFactory());

    auto graphicsContext = std::make_shared<GraphicsContextDirect3D11>();
    if (auto err = graphicsContext->Initialize(window->GetNativeWindowHandle(),
            dxgiFactory,
            device,
            presentationParameters);
        err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            Errors::Wrap(std::move(err), "failed to initialize GraphicsContextDirect3D11"));
    }

    auto graphicsCommandQueue = std::make_shared<GraphicsCommandQueueImmediate>(graphicsContext);

    POMDOG_ASSERT(graphicsDevice);
    POMDOG_ASSERT(graphicsContext);
    POMDOG_ASSERT(graphicsCommandQueue);

    auto bridge = std::make_unique<GraphicsBridgeWin32Direct3D11>(
        graphicsDevice,
        std::move(graphicsContext),
        graphicsCommandQueue);

    return std::make_tuple(
        std::move(graphicsDevice),
        std::move(graphicsCommandQueue),
        std::move(bridge),
        nullptr);
}
#endif

} // namespace

class GameHostWin32::Impl final {
public:
    ~Impl();

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        const std::shared_ptr<GameWindowWin32>& window,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const std::shared_ptr<NativeGamepad>& gamepad,
        const PresentationParameters& presentationParameters,
        bool useOpenGL) noexcept;

    void Run(Game& game);

    void Exit();

    [[nodiscard]] std::shared_ptr<GameWindow>
    GetWindow() noexcept;

    [[nodiscard]] std::shared_ptr<GameClock>
    GetClock(std::shared_ptr<GameHost>&& gameHost) noexcept;

    [[nodiscard]] std::shared_ptr<GraphicsCommandQueue>
    GetGraphicsCommandQueue() noexcept;

    [[nodiscard]] std::shared_ptr<GraphicsDevice>
    GetGraphicsDevice() noexcept;

    [[nodiscard]] std::shared_ptr<AudioEngine>
    GetAudioEngine() noexcept;

    [[nodiscard]] std::shared_ptr<AssetManager>
    GetAssetManager(std::shared_ptr<GameHost>&& gameHost) noexcept;

    [[nodiscard]] std::shared_ptr<Keyboard>
    GetKeyboard() noexcept;

    [[nodiscard]] std::shared_ptr<Mouse>
    GetMouse() noexcept;

    [[nodiscard]] std::shared_ptr<Gamepad>
    GetGamepad() noexcept;

    [[nodiscard]] std::shared_ptr<IOService>
    GetIOService(std::shared_ptr<GameHost>&& gameHost) noexcept;

    [[nodiscard]] std::shared_ptr<HTTPClient>
    GetHTTPClient(std::shared_ptr<GameHost>&& gameHost) noexcept;

private:
    void RenderFrame(Game& game);

    void DoEvents();

    void ProcessSystemEvents(const SystemEvent& event);

    void ClientSizeChanged();

private:
    GameClock clock;
    Detail::SubsystemScheduler subsystemScheduler;
    ScopedConnection systemEventConnection;

    std::shared_ptr<EventQueue<SystemEvent>> eventQueue;
    std::shared_ptr<GameWindowWin32> window;

    std::unique_ptr<GraphicsBridgeWin32> graphicsBridge;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsCommandQueue> graphicsCommandQueue;
    std::unique_ptr<AssetManager> assetManager;
    std::shared_ptr<AudioEngineXAudio2> audioEngine;

    std::shared_ptr<KeyboardWin32> keyboard;
    std::shared_ptr<MouseWin32> mouse;
    std::shared_ptr<NativeGamepad> gamepad;

    std::unique_ptr<IOService> ioService;
    std::unique_ptr<HTTPClient> httpClient;

    std::thread gamepadThread;

    Duration presentationInterval;
    SurfaceFormat backBufferSurfaceFormat;
    SurfaceFormat backBufferDepthStencilFormat;
    std::atomic<bool> exitRequest = false;
    bool surfaceResizeRequest = false;
};

std::unique_ptr<Error>
GameHostWin32::Impl::Initialize(
    const std::shared_ptr<GameWindowWin32>& windowIn,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueueIn,
    const std::shared_ptr<NativeGamepad>& gamepadIn,
    const PresentationParameters& presentationParameters,
    bool useOpenGL) noexcept
{
    eventQueue = eventQueueIn;
    window = windowIn;
    backBufferSurfaceFormat = presentationParameters.BackBufferFormat;
    backBufferDepthStencilFormat = presentationParameters.DepthStencilFormat;
    exitRequest = false;
    surfaceResizeRequest = false;

    POMDOG_ASSERT(presentationParameters.PresentationInterval > 0);
    presentationInterval = Duration(1.0) / presentationParameters.PresentationInterval;

#if !defined(POMDOG_DISABLE_GL4)
    if (useOpenGL) {
        auto result = CreateGraphicsDeviceGL4(window, presentationParameters);
        graphicsDevice = std::move(std::get<0>(result));
        graphicsCommandQueue = std::move(std::get<1>(result));
        graphicsBridge = std::move(std::get<2>(result));

        if (auto deviceErr = std::move(std::get<3>(result)); deviceErr != nullptr) {
            return Errors::Wrap(std::move(deviceErr), "CreateGraphicsDeviceGL4() failed");
        }
    }
#endif
#if !defined(POMDOG_DISABLE_DIRECT3D11)
    if (!useOpenGL) {
        auto result = CreateGraphicsDeviceDirect3D11(window, presentationParameters);
        graphicsDevice = std::move(std::get<0>(result));
        graphicsCommandQueue = std::move(std::get<1>(result));
        graphicsBridge = std::move(std::get<2>(result));

        if (auto deviceErr = std::move(std::get<3>(result)); deviceErr != nullptr) {
            return Errors::Wrap(std::move(deviceErr), "CreateGraphicsDeviceDirect3D11() failed");
        }
    }
#endif

    POMDOG_ASSERT(eventQueue);
    systemEventConnection = eventQueue->Connect([this](const SystemEvent& event) {
        ProcessSystemEvents(event);
    });

    // NOTE: Create audio engine.
    audioEngine = std::make_shared<AudioEngineXAudio2>();
    if (auto err = audioEngine->Initialize(); err != nullptr) {
        return Errors::Wrap(std::move(err), "AudioEngineXAudio2::Initialize() failed");
    }

    keyboard = std::make_shared<KeyboardWin32>();
    mouse = std::make_shared<MouseWin32>(window->GetNativeWindowHandle());
    gamepad = gamepadIn;

    auto [resourceDir, resourceDirErr] = FileSystem::GetResourceDirectoryPath();
    if (resourceDirErr != nullptr) {
        return Errors::Wrap(std::move(resourceDirErr), "FileSystem::GetResourceDirectoryPath() failed");
    }
    auto contentDirectory = PathHelper::Join(resourceDir, "Content");

    // NOTE: Create asset manager.
    assetManager = std::make_unique<AssetManager>(
        std::move(contentDirectory),
        audioEngine,
        graphicsDevice);

    // NOTE: Create IO service.
    ioService = std::make_unique<IOService>(&clock);
    if (auto err = ioService->Initialize(); err != nullptr) {
        return Errors::Wrap(std::move(err), "IOService::Initialize() failed");
    }
    httpClient = std::make_unique<HTTPClient>(ioService.get());

    gamepadThread = std::thread([this] {
        while (!exitRequest) {
            gamepad->EnumerateDevices();
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
        }
    });

    return nullptr;
}

GameHostWin32::Impl::~Impl()
{
    if (gamepadThread.joinable()) {
        gamepadThread.join();
    }

    eventQueue.reset();
    httpClient.reset();
    if (auto err = ioService->Shutdown(); err != nullptr) {
        Log::Warning("Pomdog", err->ToString());
    }
    ioService.reset();
    assetManager.reset();
    gamepad.reset();
    keyboard.reset();
    mouse.reset();
    audioEngine.reset();

    if (graphicsBridge != nullptr) {
        graphicsBridge->Shutdown();
    }
    graphicsBridge.reset();
    graphicsCommandQueue.reset();
    graphicsDevice.reset();

    window.reset();
}

void GameHostWin32::Impl::Run(Game& game)
{
    while (!exitRequest) {
        clock.Tick();
        MessagePump();
        DoEvents();
        gamepad->PollEvents();
        ioService->Step();
        subsystemScheduler.OnUpdate();
        game.Update();
        RenderFrame(game);

        auto elapsedTime = clock.GetElapsedTime();

        if (elapsedTime < presentationInterval) {
            ::Sleep(1);
        }
    }

    window->Close();
    MessagePump();
}

void GameHostWin32::Impl::Exit()
{
    exitRequest = true;
}

void GameHostWin32::Impl::RenderFrame(Game& game)
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

void GameHostWin32::Impl::ProcessSystemEvents(const SystemEvent& event)
{
    switch (event.Kind) {
    case SystemEventKind::WindowShouldCloseEvent: {
        Log::Internal("WindowShouldCloseEvent");
        this->Exit();
        break;
    }
    case SystemEventKind::ViewDidEndLiveResizeEvent: {
        surfaceResizeRequest = true;
        break;
    }
    default:
        mouse->HandleMessage(event);
        keyboard->HandleMessage(event);
        break;
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

std::shared_ptr<GameWindow>
GameHostWin32::Impl::GetWindow() noexcept
{
    return window;
}

std::shared_ptr<GameClock>
GameHostWin32::Impl::GetClock(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    std::shared_ptr<GameClock> shared{std::move(gameHost), &clock};
    return shared;
}

std::shared_ptr<GraphicsCommandQueue>
GameHostWin32::Impl::GetGraphicsCommandQueue() noexcept
{
    POMDOG_ASSERT(graphicsCommandQueue != nullptr);
    return graphicsCommandQueue;
}

std::shared_ptr<GraphicsDevice>
GameHostWin32::Impl::GetGraphicsDevice() noexcept
{
    POMDOG_ASSERT(graphicsDevice != nullptr);
    return graphicsDevice;
}

std::shared_ptr<AudioEngine>
GameHostWin32::Impl::GetAudioEngine() noexcept
{
    POMDOG_ASSERT(audioEngine != nullptr);
    return audioEngine;
}

std::shared_ptr<AssetManager>
GameHostWin32::Impl::GetAssetManager(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(assetManager != nullptr);
    std::shared_ptr<AssetManager> shared{std::move(gameHost), assetManager.get()};
    return shared;
}

std::shared_ptr<Keyboard>
GameHostWin32::Impl::GetKeyboard() noexcept
{
    POMDOG_ASSERT(keyboard != nullptr);
    return keyboard;
}

std::shared_ptr<Mouse>
GameHostWin32::Impl::GetMouse() noexcept
{
    POMDOG_ASSERT(mouse != nullptr);
    return mouse;
}

std::shared_ptr<Gamepad>
GameHostWin32::Impl::GetGamepad() noexcept
{
    POMDOG_ASSERT(gamepad != nullptr);
    return gamepad;
}

std::shared_ptr<IOService>
GameHostWin32::Impl::GetIOService(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(ioService != nullptr);
    std::shared_ptr<IOService> shared{std::move(gameHost), ioService.get()};
    return shared;
}

std::shared_ptr<HTTPClient>
GameHostWin32::Impl::GetHTTPClient(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(httpClient != nullptr);
    std::shared_ptr<HTTPClient> shared(std::move(gameHost), httpClient.get());
    return shared;
}

GameHostWin32::GameHostWin32()
    : impl(std::make_unique<Impl>())
{
}

GameHostWin32::~GameHostWin32() = default;

std::unique_ptr<Error>
GameHostWin32::Initialize(
    const std::shared_ptr<GameWindowWin32>& window,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
    const std::shared_ptr<NativeGamepad>& gamepad,
    const PresentationParameters& presentationParameters,
    bool useOpenGL) noexcept
{
    POMDOG_ASSERT(impl != nullptr);
    return impl->Initialize(
        window,
        eventQueue,
        gamepad,
        presentationParameters,
        useOpenGL);
}

void GameHostWin32::Run(Game& game)
{
    POMDOG_ASSERT(impl);
    impl->Run(game);
}

void GameHostWin32::Exit()
{
    POMDOG_ASSERT(impl);
    impl->Exit();
}

std::shared_ptr<GameWindow> GameHostWin32::GetWindow() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetWindow();
}

std::shared_ptr<GameClock> GameHostWin32::GetClock() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetClock(shared_from_this());
}

std::shared_ptr<GraphicsCommandQueue> GameHostWin32::GetGraphicsCommandQueue() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsCommandQueue();
}

std::shared_ptr<GraphicsDevice> GameHostWin32::GetGraphicsDevice() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsDevice();
}

std::shared_ptr<AudioEngine> GameHostWin32::GetAudioEngine() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetAudioEngine();
}

std::shared_ptr<AssetManager> GameHostWin32::GetAssetManager() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetAssetManager(shared_from_this());
}

std::shared_ptr<Keyboard> GameHostWin32::GetKeyboard() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetKeyboard();
}

std::shared_ptr<Mouse> GameHostWin32::GetMouse() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetMouse();
}

std::shared_ptr<Gamepad> GameHostWin32::GetGamepad() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetGamepad();
}

std::shared_ptr<IOService> GameHostWin32::GetIOService() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetIOService(shared_from_this());
}

std::shared_ptr<HTTPClient> GameHostWin32::GetHTTPClient() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetHTTPClient(shared_from_this());
}

} // namespace Pomdog::Detail::Win32
