// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/win32/game_host_win32.h"
#include "pomdog/application/win32/game_window_win32.h"
#include "pomdog/input/directinput/gamepad_directinput.h"
#include "pomdog/input/win32/keyboard_win32.h"
#include "pomdog/input/win32/mouse_win32.h"
#if !defined(POMDOG_DISABLE_GL4)
#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/gl4/graphics_context_gl4.h"
#include "pomdog/gpu/gl4/graphics_device_gl4.h"
#include "pomdog/platform/win32/opengl_context_win32.h"
#endif
#if !defined(POMDOG_DISABLE_DIRECT3D11)
#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/direct3d11/graphics_context_direct3d11.h"
#include "pomdog/gpu/direct3d11/graphics_device_direct3d11.h"
#endif
#include "pomdog/application/game.h"
#include "pomdog/application/subsystem_scheduler.h"
#include "pomdog/application/system_events.h"
#include "pomdog/audio/xaudio2/audio_engine_xaudio2.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/chrono/win32/time_source_win32.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/logging/log.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/network/http_client.h"
#include "pomdog/network/io_service.h"
#include "pomdog/signals/event_queue.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::win32::GameWindowWin32;
using pomdog::detail::xaudio2::AudioEngineXAudio2;
#if !defined(POMDOG_DISABLE_GL4)
using pomdog::gpu::detail::gl4::GraphicsContextGL4;
using pomdog::gpu::detail::gl4::GraphicsDeviceGL4;
#endif
#if !defined(POMDOG_DISABLE_DIRECT3D11)
using pomdog::gpu::detail::direct3d11::GraphicsContextDirect3D11;
using pomdog::gpu::detail::direct3d11::GraphicsDeviceDirect3D11;
#endif

namespace pomdog::detail::win32 {
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
    std::shared_ptr<gpu::GraphicsDevice>,
    std::shared_ptr<gpu::CommandQueue>,
    std::unique_ptr<GraphicsBridgeWin32>,
    std::unique_ptr<Error>>;

#if !defined(POMDOG_DISABLE_GL4)

class GraphicsBridgeWin32GL4 final : public GraphicsBridgeWin32 {
private:
    std::shared_ptr<GraphicsDeviceGL4> graphicsDevice;
    std::shared_ptr<gpu::detail::CommandQueueImmediate> commandQueue;

public:
    GraphicsBridgeWin32GL4(
        const std::shared_ptr<GraphicsDeviceGL4>& graphicsDeviceIn,
        const std::shared_ptr<gpu::detail::CommandQueueImmediate>& commandQueueIn)
        : graphicsDevice(graphicsDeviceIn)
        , commandQueue(commandQueueIn)
    {
    }

    void OnClientSizeChanged(int width, int height) override
    {
        POMDOG_ASSERT(graphicsDevice);
        graphicsDevice->clientSizeChanged(width, height);
    }

    void Shutdown() override
    {
        if (commandQueue != nullptr) {
            commandQueue->reset();
        }

        commandQueue.reset();
        graphicsDevice.reset();
    }
};

[[nodiscard]] CreateGraphicsDeviceResult
CreateGraphicsDeviceGL4(
    const std::shared_ptr<GameWindowWin32>& window,
    const gpu::PresentationParameters& presentationParameters)
{
    // NOTE: Create an OpenGL context.
    auto openGLContext = std::make_shared<win32::OpenGLContextWin32>();
    if (auto err = openGLContext->Initialize(
            window->GetNativeWindowHandle(),
            presentationParameters);
        err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            errors::wrap(std::move(err), "OpenGLContextWin32::Initialize() failed."));
    }

    if (::glewInit() != GLEW_OK) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            errors::make("glewInit() failed."));
    }

    openGLContext->makeCurrent();

    // NOTE: Create a graphics device.
    auto graphicsDevice = std::make_shared<GraphicsDeviceGL4>();
    if (auto err = graphicsDevice->Initialize(presentationParameters); err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            errors::wrap(std::move(err), "GraphicsDeviceGL4::Initialize() failed."));
    }

    // NOTE: Create a graphics context.
    auto graphicsContext = std::make_shared<GraphicsContextGL4>();
    if (auto err = graphicsContext->Initialize(openGLContext, graphicsDevice); err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            errors::wrap(std::move(err), "GraphicsContextGL4::Initialize() failed."));
    }

    auto graphicsCommandQueue = std::make_shared<gpu::detail::CommandQueueImmediate>(graphicsContext);

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
    std::shared_ptr<gpu::detail::CommandQueueImmediate> commandQueue;

public:
    GraphicsBridgeWin32Direct3D11(
        const std::shared_ptr<GraphicsDeviceDirect3D11>& graphicsDeviceIn,
        const std::shared_ptr<GraphicsContextDirect3D11>& graphicsContextIn,
        const std::shared_ptr<gpu::detail::CommandQueueImmediate>& commandQueueIn)
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
            Log::Critical("pomdog", "error: ResizeBackBuffers() failed: " + err->toString());
        }
        graphicsDevice->clientSizeChanged(width, height);
    }

    void Shutdown() override
    {
        if (commandQueue != nullptr) {
            commandQueue->reset();
        }

        commandQueue.reset();
        graphicsContext.reset();
        graphicsDevice.reset();
    }
};

[[nodiscard]] CreateGraphicsDeviceResult
CreateGraphicsDeviceDirect3D11(
    const std::shared_ptr<GameWindowWin32>& window,
    const gpu::PresentationParameters& presentationParameters)
{
    auto graphicsDevice = std::make_shared<GraphicsDeviceDirect3D11>();
    if (auto err = graphicsDevice->Initialize(presentationParameters); err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            errors::wrap(std::move(err), "failed to initialize GraphicsDeviceDirect3D11"));
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
            errors::wrap(std::move(err), "failed to initialize GraphicsContextDirect3D11"));
    }

    auto graphicsCommandQueue = std::make_shared<gpu::detail::CommandQueueImmediate>(graphicsContext);

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
    initialize(
        const std::shared_ptr<GameWindowWin32>& window,
        HINSTANCE hInstance,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const gpu::PresentationParameters& presentationParameters,
        bool useOpenGL) noexcept;

    void run(Game& game);

    void exit();

    [[nodiscard]] std::shared_ptr<GameWindow>
    getWindow() noexcept;

    [[nodiscard]] std::shared_ptr<GameClock>
    getClock() noexcept;

    [[nodiscard]] std::shared_ptr<gpu::CommandQueue>
    getCommandQueue() noexcept;

    [[nodiscard]] std::shared_ptr<gpu::GraphicsDevice>
    getGraphicsDevice() noexcept;

    [[nodiscard]] std::shared_ptr<AudioEngine>
    getAudioEngine() noexcept;

    [[nodiscard]] std::shared_ptr<AssetManager>
    GetAssetManager(std::shared_ptr<GameHost>&& gameHost) noexcept;

    [[nodiscard]] std::shared_ptr<Keyboard>
    getKeyboard() noexcept;

    [[nodiscard]] std::shared_ptr<Mouse>
    getMouse() noexcept;

    [[nodiscard]] std::shared_ptr<Gamepad>
    getGamepad() noexcept;

    [[nodiscard]] std::shared_ptr<IOService>
    getIOService(std::shared_ptr<GameHost>&& gameHost) noexcept;

    [[nodiscard]] std::shared_ptr<HTTPClient>
    getHTTPClient(std::shared_ptr<GameHost>&& gameHost) noexcept;

private:
    void renderFrame(Game& game);

    void doEvents();

    void processSystemEvents(const SystemEvent& event);

    void clientSizeChanged();

private:
    std::shared_ptr<detail::win32::TimeSourceWin32> timeSource_;
    std::shared_ptr<GameClockImpl> clock_;
    detail::SubsystemScheduler subsystemScheduler;
    ScopedConnection systemEventConnection;

    std::shared_ptr<EventQueue<SystemEvent>> eventQueue;
    std::shared_ptr<GameWindowWin32> window;

    std::unique_ptr<GraphicsBridgeWin32> graphicsBridge;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<gpu::CommandQueue> graphicsCommandQueue;
    std::unique_ptr<AssetManager> assetManager;
    std::shared_ptr<AudioEngineXAudio2> audioEngine;

    std::shared_ptr<KeyboardWin32> keyboard;
    std::shared_ptr<MouseWin32> mouse;
    std::shared_ptr<directinput::GamepadDirectInput> gamepad;

    std::unique_ptr<IOService> ioService_;
    std::unique_ptr<HTTPClient> httpClient;

    std::thread gamepadThread;

    Duration presentationInterval;
    PixelFormat backBufferSurfaceFormat;
    PixelFormat backBufferDepthStencilFormat;
    std::atomic<bool> exitRequest = false;
    bool surfaceResizeRequest = false;
};

std::unique_ptr<Error>
GameHostWin32::Impl::initialize(
    const std::shared_ptr<GameWindowWin32>& windowIn,
    HINSTANCE hInstance,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueueIn,
    const gpu::PresentationParameters& presentationParameters,
    bool useOpenGL) noexcept
{
    eventQueue = eventQueueIn;
    window = windowIn;
    backBufferSurfaceFormat = presentationParameters.backBufferFormat;
    backBufferDepthStencilFormat = presentationParameters.depthStencilFormat;
    exitRequest = false;
    surfaceResizeRequest = false;

    POMDOG_ASSERT(presentationParameters.presentationInterval > 0);
    presentationInterval = Duration(1.0) / presentationParameters.presentationInterval;

    timeSource_ = std::make_shared<detail::win32::TimeSourceWin32>();
    clock_ = std::make_shared<GameClockImpl>();
    if (auto err = clock_->Initialize(presentationParameters.presentationInterval, timeSource_); err != nullptr) {
        return errors::wrap(std::move(err), "GameClockImpl::Initialize() failed.");
    }

#if !defined(POMDOG_DISABLE_GL4)
    if (useOpenGL) {
        auto result = CreateGraphicsDeviceGL4(window, presentationParameters);
        graphicsDevice = std::move(std::get<0>(result));
        graphicsCommandQueue = std::move(std::get<1>(result));
        graphicsBridge = std::move(std::get<2>(result));

        if (auto deviceErr = std::move(std::get<3>(result)); deviceErr != nullptr) {
            return errors::wrap(std::move(deviceErr), "CreateGraphicsDeviceGL4() failed");
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
            return errors::wrap(std::move(deviceErr), "CreateGraphicsDeviceDirect3D11() failed");
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
        return errors::wrap(std::move(err), "AudioEngineXAudio2::Initialize() failed");
    }

    keyboard = std::make_shared<KeyboardWin32>();
    mouse = std::make_shared<MouseWin32>(window->GetNativeWindowHandle());

    gamepad = std::make_shared<directinput::GamepadDirectInput>();
    if (auto err = gamepad->Initialize(hInstance, window->GetNativeWindowHandle()); err != nullptr) {
        return errors::wrap(std::move(err), "GamepadDirectInput::Initialize() failed");
    }

    auto [resourceDir, resourceDirErr] = FileSystem::GetResourceDirectoryPath();
    if (resourceDirErr != nullptr) {
        return errors::wrap(std::move(resourceDirErr), "FileSystem::GetResourceDirectoryPath() failed");
    }
    auto contentDirectory = PathHelper::Join(resourceDir, "content");

    // NOTE: Create asset manager.
    assetManager = std::make_unique<AssetManager>(
        std::move(contentDirectory),
        audioEngine,
        graphicsDevice);

    // NOTE: Create IO service.
    ioService_ = std::make_unique<IOService>();
    if (auto err = ioService_->Initialize(clock_); err != nullptr) {
        return errors::wrap(std::move(err), "IOService::Initialize() failed");
    }
    httpClient = std::make_unique<HTTPClient>(ioService_.get());

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
    if (auto err = ioService_->Shutdown(); err != nullptr) {
        Log::Warning("pomdog", err->toString());
    }
    ioService_.reset();
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

void GameHostWin32::Impl::run(Game& game)
{
    while (!exitRequest) {
        clock_->Tick();
        MessagePump();
        DoEvents();
        gamepad->PollEvents();
        ioService_->Step();
        subsystemScheduler.OnUpdate();
        game.Update();
        RenderFrame(game);

        auto elapsedTime = clock_->GetElapsedTime();

        if (elapsedTime < presentationInterval) {
            ::Sleep(1);
        }
    }

    window->Close();
    MessagePump();
}

void GameHostWin32::Impl::exit()
{
    exitRequest = true;
}

void GameHostWin32::Impl::renderFrame(Game& game)
{
    POMDOG_ASSERT(window);

    if (!window || window->isMinimized()) {
        // skip rendering
        return;
    }

    game.Draw();
}

void GameHostWin32::Impl::doEvents()
{
    eventQueue->Emit();

    if (surfaceResizeRequest) {
        clientSizeChanged();
        surfaceResizeRequest = false;
    }
}

void GameHostWin32::Impl::processSystemEvents(const SystemEvent& event)
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

void GameHostWin32::Impl::clientSizeChanged()
{
    POMDOG_ASSERT(window);
    const auto bounds = window->getClientBounds();

    POMDOG_ASSERT(graphicsBridge);
    graphicsBridge->OnClientSizeChanged(bounds.width, bounds.Height);
    window->clientSizeChanged(bounds.width, bounds.Height);
}

std::shared_ptr<GameWindow>
GameHostWin32::Impl::getWindow() noexcept
{
    return window;
}

std::shared_ptr<GameClock>
GameHostWin32::Impl::getClock() noexcept
{
    return clock_;
}

std::shared_ptr<gpu::CommandQueue>
GameHostWin32::Impl::getCommandQueue() noexcept
{
    POMDOG_ASSERT(graphicsCommandQueue != nullptr);
    return graphicsCommandQueue;
}

std::shared_ptr<gpu::GraphicsDevice>
GameHostWin32::Impl::getGraphicsDevice() noexcept
{
    POMDOG_ASSERT(graphicsDevice != nullptr);
    return graphicsDevice;
}

std::shared_ptr<AudioEngine>
GameHostWin32::Impl::getAudioEngine() noexcept
{
    POMDOG_ASSERT(audioEngine != nullptr);
    return audioEngine;
}

std::shared_ptr<AssetManager>
GameHostWin32::Impl::getAssetManager(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(assetManager != nullptr);
    std::shared_ptr<AssetManager> shared{std::move(gameHost), assetManager.get()};
    return shared;
}

std::shared_ptr<Keyboard>
GameHostWin32::Impl::getKeyboard() noexcept
{
    POMDOG_ASSERT(keyboard != nullptr);
    return keyboard;
}

std::shared_ptr<Mouse>
GameHostWin32::Impl::getMouse() noexcept
{
    POMDOG_ASSERT(mouse != nullptr);
    return mouse;
}

std::shared_ptr<Gamepad>
GameHostWin32::Impl::getGamepad() noexcept
{
    POMDOG_ASSERT(gamepad != nullptr);
    return gamepad;
}

std::shared_ptr<IOService>
GameHostWin32::Impl::getIOService(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(ioService_ != nullptr);
    std::shared_ptr<IOService> shared{std::move(gameHost), ioService_.get()};
    return shared;
}

std::shared_ptr<HTTPClient>
GameHostWin32::Impl::getHTTPClient(std::shared_ptr<GameHost>&& gameHost) noexcept
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
GameHostWin32::initialize(
    const std::shared_ptr<GameWindowWin32>& window,
    HINSTANCE hInstance,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
    const gpu::PresentationParameters& presentationParameters,
    bool useOpenGL) noexcept
{
    POMDOG_ASSERT(impl != nullptr);
    return impl->initialize(
        window,
        hInstance,
        eventQueue,
        presentationParameters,
        useOpenGL);
}

void GameHostWin32::run(Game& game)
{
    POMDOG_ASSERT(impl);
    impl->run(game);
}

void GameHostWin32::exit()
{
    POMDOG_ASSERT(impl);
    impl->exit();
}

std::shared_ptr<GameWindow> GameHostWin32::getWindow() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->getWindow();
}

std::shared_ptr<GameClock> GameHostWin32::getClock() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->getClock();
}

std::shared_ptr<gpu::CommandQueue> GameHostWin32::getCommandQueue() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->getCommandQueue();
}

std::shared_ptr<gpu::GraphicsDevice> GameHostWin32::getGraphicsDevice() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->getGraphicsDevice();
}

std::shared_ptr<AudioEngine> GameHostWin32::getAudioEngine() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->getAudioEngine();
}

std::shared_ptr<AssetManager> GameHostWin32::getAssetManager() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetAssetManager(shared_from_this());
}

std::shared_ptr<Keyboard> GameHostWin32::getKeyboard() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->getKeyboard();
}

std::shared_ptr<Mouse> GameHostWin32::getMouse() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->getMouse();
}

std::shared_ptr<Gamepad> GameHostWin32::getGamepad() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->getGamepad();
}

std::shared_ptr<IOService> GameHostWin32::getIOService() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->getIOService(shared_from_this());
}

std::shared_ptr<HTTPClient> GameHostWin32::getHTTPClient() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->getHTTPClient(shared_from_this());
}

} // namespace pomdog::detail::win32
