// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "GameHostCocoa.hpp"
#include "GameWindowCocoa.hpp"
#include "KeyboardCocoa.hpp"
#include "MouseCocoa.hpp"
#include "OpenGLContextCocoa.hpp"
#include "PomdogOpenGLView.hpp"
#include "../Application/SystemEvents.hpp"
#include "../Audio.OpenAL/AudioEngineAL.hpp"
#include "../Graphics.Backends/GraphicsCommandQueueImmediate.hpp"
#include "../Graphics.GL4/GraphicsContextGL4.hpp"
#include "../Graphics.GL4/GraphicsDeviceGL4.hpp"
#include "../Input.IOKit/GamepadIOKit.hpp"
#include "Pomdog/Application/FileSystem.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/GraphicsCommandQueue.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Network/HTTPClient.hpp"
#include "Pomdog/Network/IOService.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

using Pomdog::Detail::GL4::GraphicsDeviceGL4;
using Pomdog::Detail::GL4::GraphicsContextGL4;
using Pomdog::Detail::IOKit::GamepadIOKit;
using Pomdog::Detail::OpenAL::AudioEngineAL;

namespace Pomdog::Detail::Cocoa {

// MARK: - GameHostCocoa::Impl

class GameHostCocoa::Impl final {
public:
    ~Impl();

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        PomdogOpenGLView* openGLView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const PresentationParameters& presentationParameters);

    [[nodiscard]] std::unique_ptr<Error>
    Run(
        const std::weak_ptr<Game>& game,
        std::function<void()>&& onCompleted);

    void Exit();

    [[nodiscard]] std::shared_ptr<GameWindow>
    GetWindow() noexcept;

    [[nodiscard]] std::shared_ptr<GameClock>
    GetClock(std::shared_ptr<GameHost>&& gameHost) noexcept;

    [[nodiscard]] std::shared_ptr<GraphicsDevice>
    GetGraphicsDevice() noexcept;

    [[nodiscard]] std::shared_ptr<GraphicsCommandQueue>
    GetGraphicsCommandQueue() noexcept;

    [[nodiscard]] std::shared_ptr<AssetManager>
    GetAssetManager(std::shared_ptr<GameHost>&& gameHost) noexcept;

    [[nodiscard]] std::shared_ptr<AudioEngine>
    GetAudioEngine() noexcept;

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
    void GameLoop();

    void RenderFrame();

    void DoEvents();

    void ProcessSystemEvents(const SystemEvent& event);

    void ClientSizeChanged();

    void GameWillExit();

    static CVReturn DisplayLinkCallback(
        CVDisplayLinkRef displayLink,
        const CVTimeStamp* now,
        const CVTimeStamp* outputTime,
        CVOptionFlags flagsIn,
        CVOptionFlags* flagsOut,
        void* displayLinkContext);

private:
    GameClock clock;
    ScopedConnection systemEventConnection;
    std::mutex renderMutex;
    std::atomic_bool viewLiveResizing = false;
    CVDisplayLinkRef displayLink = nullptr;
    std::function<void()> onCompleted;

    std::weak_ptr<Game> weakGame;
    std::shared_ptr<EventQueue<SystemEvent>> eventQueue;
    std::shared_ptr<GameWindowCocoa> window;
    std::shared_ptr<OpenGLContextCocoa> openGLContext;
    std::shared_ptr<GraphicsDeviceGL4> graphicsDevice;
    std::shared_ptr<GraphicsContextGL4> graphicsContext;
    std::shared_ptr<GraphicsCommandQueue> graphicsCommandQueue;
    std::shared_ptr<AudioEngineAL> audioEngine;
    std::unique_ptr<AssetManager> assetManager;
    std::shared_ptr<KeyboardCocoa> keyboard;
    std::shared_ptr<MouseCocoa> mouse;
    std::shared_ptr<GamepadIOKit> gamepad;

    std::unique_ptr<IOService> ioService;
    std::unique_ptr<HTTPClient> httpClient;

    __weak PomdogOpenGLView* openGLView = nullptr;
    Duration presentationInterval = Duration::zero();
    bool exitRequest = false;
    bool displayLinkEnabled = true;
};

std::unique_ptr<Error>
GameHostCocoa::Impl::Initialize(
    PomdogOpenGLView* openGLViewIn,
    const std::shared_ptr<GameWindowCocoa>& windowIn,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueueIn,
    const PresentationParameters& presentationParameters)
{
    this->viewLiveResizing = false;
    this->displayLink = nullptr;
    this->eventQueue = eventQueueIn;
    this->window = windowIn;
    this->openGLView = openGLViewIn;
    this->exitRequest = false;
    this->displayLinkEnabled = true;

    if (presentationParameters.PresentationInterval <= 0) {
        return Errors::New("PresentationInterval must be > 0.");
    }
    POMDOG_ASSERT(presentationParameters.PresentationInterval > 0);
    presentationInterval = Duration(1) / presentationParameters.PresentationInterval;

    POMDOG_ASSERT(window);
    window->SetView(openGLView);

    // NOTE: Create OpenGL context.
    openGLContext = std::make_shared<OpenGLContextCocoa>();
    if (auto err = openGLContext->Initialize(presentationParameters); err != nullptr) {
        return Errors::Wrap(std::move(err), "OpenGLContextCocoa::Initialize() failed.");
    }

    POMDOG_ASSERT(openGLView != nullptr);
    [openGLView setEventQueue:eventQueue];
    [openGLView setOpenGLContext:openGLContext];

    // Create graphics device and device resources
    openGLContext->Lock();
    openGLContext->SetView(openGLView);
    openGLContext->MakeCurrent();

    // NOTE: Create a graphics device.
    graphicsDevice = std::make_shared<GraphicsDeviceGL4>();
    if (auto err = graphicsDevice->Initialize(presentationParameters); err != nullptr) {
        return Errors::Wrap(std::move(err), "GraphicsDeviceGL4::Initialize() failed.");
    }

    // NOTE: Create a graphics context.
    graphicsContext = std::make_shared<GraphicsContextGL4>();
    if (auto err = graphicsContext->Initialize(openGLContext, graphicsDevice); err != nullptr) {
        return Errors::Wrap(std::move(err), "GraphicsContextGL4::Initialize() failed.");
    }

    graphicsCommandQueue = std::make_shared<GraphicsCommandQueueImmediate>(graphicsContext);
    openGLContext->Unlock();

    // NOTE: Create audio engine.
    audioEngine = std::make_shared<AudioEngineAL>();
    if (auto err = audioEngine->Initialize(); err != nullptr) {
        return Errors::Wrap(std::move(err), "AudioEngineAL::Initialize() failed.");
    }

    // Create subsystems
    keyboard = std::make_shared<KeyboardCocoa>();
    mouse = std::make_shared<MouseCocoa>();

    // NOTE: Create gamepad
    gamepad = std::make_shared<GamepadIOKit>();
    if (auto err = gamepad->Initialize(eventQueue); err != nullptr) {
        return Errors::Wrap(std::move(err), "GamepadIOKit::Initialize() failed.");
    }

    // Connect to system event signal
    POMDOG_ASSERT(eventQueue);
    systemEventConnection = eventQueue->Connect(
        [this](const SystemEvent& event) { ProcessSystemEvents(event); });

    auto [resourceDir, resourceDirErr] = FileSystem::GetResourceDirectoryPath();
    if (resourceDirErr != nullptr) {
        return Errors::Wrap(std::move(resourceDirErr), "FileSystem::GetResourceDirectoryPath() failed.");
    }
    auto contentDirectory = PathHelper::Join(resourceDir, "Content");

    // NOTE: Create asset manager.
    assetManager = std::make_unique<AssetManager>(
        std::move(contentDirectory),
        audioEngine,
        graphicsDevice);

    ioService = std::make_unique<IOService>(&clock);
    if (auto err = ioService->Initialize(); err != nullptr) {
        return Errors::Wrap(std::move(err), "IOService::Initialize() failed.");
    }
    httpClient = std::make_unique<HTTPClient>(ioService.get());

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &DisplayLinkCallback, this);

    return nullptr;
}

GameHostCocoa::Impl::~Impl()
{
    if (displayLink != nullptr) {
        CVDisplayLinkRelease(displayLink);
        displayLink = nullptr;
    }

    if (openGLView != nullptr) {
        [openGLView setEventQueue:{}];
    }

    systemEventConnection.Disconnect();
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
    graphicsCommandQueue.reset();
    graphicsContext.reset();
    graphicsDevice.reset();
    openGLContext.reset();
    window.reset();
    eventQueue.reset();
    openGLView = nullptr;
}

std::unique_ptr<Error>
GameHostCocoa::Impl::Run(
    const std::weak_ptr<Game>& weakGameIn,
    std::function<void()>&& onCompletedIn)
{
    POMDOG_ASSERT(!weakGameIn.expired());
    POMDOG_ASSERT(onCompletedIn);
    weakGame = weakGameIn;
    onCompleted = std::move(onCompletedIn);

    POMDOG_ASSERT(!weakGame.expired());
    auto game = weakGame.lock();

    openGLContext->Lock();
    openGLContext->SetView(openGLView);
    openGLContext->MakeCurrent();

    if (auto err = game->Initialize(); err != nullptr) {
        openGLContext->Unlock();
        GameWillExit();
        return Errors::Wrap(std::move(err), "failed to initialzie game");
    }

    openGLContext->Unlock();

    if (exitRequest) {
        GameWillExit();
        return nullptr;
    }

    POMDOG_ASSERT(openGLView != nullptr);

    auto nsOpenGLContext = openGLContext->GetNativeOpenGLContext();
    NSOpenGLPixelFormat* nsPixelFormat = [nsOpenGLContext pixelFormat];
    CGLContextObj cglContext = [nsOpenGLContext CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [nsPixelFormat CGLPixelFormatObj];

    [openGLView setResizingCallback:[this](bool resizing) {
        viewLiveResizing = resizing;
    }];

    [openGLView setRenderCallback:[this] {
        std::lock_guard<std::mutex> lock(renderMutex);
        ClientSizeChanged();

        // NOTE: In order to prevent the RenderFrame() function from being
        // executed before the Game::Update() function is called, if the frame
        // number is <= 0, do not render.
        if (clock.GetFrameNumber() > 0) {
            RenderFrame();
        }
    }];

    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(
        displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);

    return nullptr;
}

void GameHostCocoa::Impl::GameWillExit()
{
    if (openGLView != nullptr) {
        [openGLView setRenderCallback:[] {}];
    }

    if (window) {
        window->SetView(nullptr);
    }

    if (onCompleted) {
        dispatch_async(dispatch_get_main_queue(), [this] {
            onCompleted();
        });
    }
}

void GameHostCocoa::Impl::Exit()
{
    exitRequest = true;

    if (displayLinkEnabled) {
        if (displayLink != nullptr) {
            dispatch_async(dispatch_get_main_queue(), [this] {
                CVDisplayLinkStop(displayLink);
            });
        }
        GameWillExit();
    }
}

CVReturn
GameHostCocoa::Impl::DisplayLinkCallback(
    [[maybe_unused]] CVDisplayLinkRef displayLink,
    [[maybe_unused]] const CVTimeStamp* now,
    [[maybe_unused]] const CVTimeStamp* outputTime,
    [[maybe_unused]] CVOptionFlags flagsIn,
    [[maybe_unused]] CVOptionFlags* flagsOut,
    void* displayLinkContext)
{
    dispatch_sync(dispatch_get_main_queue(), ^{
        auto gameHost = reinterpret_cast<GameHostCocoa::Impl*>(displayLinkContext);
        POMDOG_ASSERT(gameHost != nullptr);
        gameHost->GameLoop();
    });
    return kCVReturnSuccess;
}

void GameHostCocoa::Impl::GameLoop()
{
    POMDOG_ASSERT(!exitRequest);
    POMDOG_ASSERT(!weakGame.expired());

    std::lock_guard<std::mutex> lock(renderMutex);

    auto game = weakGame.lock();
    POMDOG_ASSERT(game);

    clock.Tick();
    DoEvents();
    ioService->Step();

    if (exitRequest) {
        return;
    }

    openGLContext->Lock();
    openGLContext->SetView(openGLView);
    openGLContext->MakeCurrent();
    game->Update();
    openGLContext->Unlock();

    if (!viewLiveResizing.load()) {
        RenderFrame();
    }

    if (!displayLinkEnabled) {
        auto elapsedTime = clock.GetElapsedTime();

        if (elapsedTime < presentationInterval) {
            lock.~lock_guard();
            auto sleepTime = (presentationInterval - elapsedTime);
            std::this_thread::sleep_for(sleepTime);
        }
    }
}

void GameHostCocoa::Impl::RenderFrame()
{
    POMDOG_ASSERT(window);
    POMDOG_ASSERT(!weakGame.expired());

    bool skipRender = (!window || window->IsMinimized() || [NSApp isHidden] == YES);

    if (skipRender) {
        return;
    }

    auto game = weakGame.lock();

    POMDOG_ASSERT(game);
    POMDOG_ASSERT(openGLView != nullptr);

    openGLContext->Lock();
    openGLContext->SetView(openGLView);
    openGLContext->MakeCurrent();

    game->Draw();

    openGLContext->Unlock();
}

void GameHostCocoa::Impl::DoEvents()
{
    eventQueue->Emit();
}

void GameHostCocoa::Impl::ProcessSystemEvents(const SystemEvent& event)
{
    switch (event.Kind) {
    case SystemEventKind::WindowShouldCloseEvent:
        Log::Internal("WindowShouldCloseEvent");
        this->Exit();
        break;
    case SystemEventKind::WindowWillCloseEvent:
        Log::Internal("WindowWillCloseEvent");
        break;
    case SystemEventKind::ViewWillStartLiveResizeEvent: {
        auto rect = window->GetClientBounds();
        Log::Internal(StringHelper::Format(
            "ViewWillStartLiveResizeEvent: {w: %d, h: %d}",
            rect.Width, rect.Height));
        break;
    }
    case SystemEventKind::ViewDidEndLiveResizeEvent: {
        auto rect = window->GetClientBounds();
        Log::Internal(StringHelper::Format(
            "ViewDidEndLiveResizeEvent: {w: %d, h: %d}",
            rect.Width, rect.Height));
        break;
    }
    default:
        POMDOG_ASSERT(keyboard);
        POMDOG_ASSERT(mouse);
        POMDOG_ASSERT(gamepad);
        keyboard->HandleEvent(event);
        mouse->HandleEvent(event);
        gamepad->HandleEvent(event);
        break;
    }
}

void GameHostCocoa::Impl::ClientSizeChanged()
{
    openGLContext->Lock();
    openGLContext->MakeCurrent();

    auto nativeContext = openGLContext->GetNativeOpenGLContext();
    POMDOG_ASSERT(nativeContext != nullptr);
    [nativeContext update];

    POMDOG_ASSERT(graphicsDevice != nullptr);

    auto bounds = window->GetClientBounds();

    graphicsDevice->ClientSizeChanged(bounds.Width, bounds.Height);
    window->ClientSizeChanged(bounds.Width, bounds.Height);

    openGLContext->Unlock();
}

std::shared_ptr<GameWindow>
GameHostCocoa::Impl::GetWindow() noexcept
{
    return window;
}

std::shared_ptr<GameClock>
GameHostCocoa::Impl::GetClock(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    std::shared_ptr<GameClock> shared{gameHost, &clock};
    return shared;
}

std::shared_ptr<GraphicsDevice>
GameHostCocoa::Impl::GetGraphicsDevice() noexcept
{
    return graphicsDevice;
}

std::shared_ptr<GraphicsCommandQueue>
GameHostCocoa::Impl::GetGraphicsCommandQueue() noexcept
{
    return graphicsCommandQueue;
}

std::shared_ptr<AudioEngine>
GameHostCocoa::Impl::GetAudioEngine() noexcept
{
    return audioEngine;
}

std::shared_ptr<AssetManager>
GameHostCocoa::Impl::GetAssetManager(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    std::shared_ptr<AssetManager> shared{gameHost, assetManager.get()};
    return shared;
}

std::shared_ptr<Keyboard>
GameHostCocoa::Impl::GetKeyboard() noexcept
{
    return keyboard;
}

std::shared_ptr<Mouse>
GameHostCocoa::Impl::GetMouse() noexcept
{
    return mouse;
}

std::shared_ptr<Gamepad>
GameHostCocoa::Impl::GetGamepad() noexcept
{
    return gamepad;
}

std::shared_ptr<IOService>
GameHostCocoa::Impl::GetIOService(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(ioService != nullptr);
    std::shared_ptr<IOService> shared{std::move(gameHost), ioService.get()};
    return shared;
}

std::shared_ptr<HTTPClient>
GameHostCocoa::Impl::GetHTTPClient(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(httpClient != nullptr);
    std::shared_ptr<HTTPClient> shared(std::move(gameHost), httpClient.get());
    return shared;
}

// MARK: - GameHostCocoa

GameHostCocoa::GameHostCocoa()
    : impl(std::make_unique<Impl>())
{
}

GameHostCocoa::~GameHostCocoa() = default;

std::unique_ptr<Error>
GameHostCocoa::Initialize(
    PomdogOpenGLView* openGLView,
    const std::shared_ptr<GameWindowCocoa>& window,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
    const PresentationParameters& presentationParameters)
{
    POMDOG_ASSERT(impl);
    return impl->Initialize(openGLView, window, eventQueue, presentationParameters);
}

std::unique_ptr<Error>
GameHostCocoa::Run(
    const std::weak_ptr<Game>& game,
    std::function<void()>&& onCompleted)
{
    POMDOG_ASSERT(impl != nullptr);
    return impl->Run(game, std::move(onCompleted));
}

void GameHostCocoa::Exit()
{
    POMDOG_ASSERT(impl);
    impl->Exit();
}

std::shared_ptr<GameWindow> GameHostCocoa::GetWindow() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetWindow();
}

std::shared_ptr<GameClock> GameHostCocoa::GetClock() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetClock(shared_from_this());
}

std::shared_ptr<GraphicsDevice> GameHostCocoa::GetGraphicsDevice() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsDevice();
}

std::shared_ptr<GraphicsCommandQueue> GameHostCocoa::GetGraphicsCommandQueue() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsCommandQueue();
}

std::shared_ptr<AudioEngine> GameHostCocoa::GetAudioEngine() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetAudioEngine();
}

std::shared_ptr<AssetManager> GameHostCocoa::GetAssetManager() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetAssetManager(shared_from_this());
}

std::shared_ptr<Keyboard> GameHostCocoa::GetKeyboard() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetKeyboard();
}

std::shared_ptr<Mouse> GameHostCocoa::GetMouse() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetMouse();
}

std::shared_ptr<Gamepad> GameHostCocoa::GetGamepad() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetGamepad();
}

std::shared_ptr<IOService> GameHostCocoa::GetIOService() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetIOService(shared_from_this());
}

std::shared_ptr<HTTPClient> GameHostCocoa::GetHTTPClient() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetHTTPClient(shared_from_this());
}

} // namespace Pomdog::Detail::Cocoa
