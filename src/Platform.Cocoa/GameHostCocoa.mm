// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GameHostCocoa.hpp"
#include "GameWindowCocoa.hpp"
#include "KeyboardCocoa.hpp"
#include "MouseCocoa.hpp"
#include "OpenGLContextCocoa.hpp"
#include "PomdogOpenGLView.hpp"
#include "../Application/SystemEvents.hpp"
#include "../InputSystem.IOKit/GamepadIOKit.hpp"
#include "../RenderSystem.GL4/GraphicsContextGL4.hpp"
#include "../RenderSystem.GL4/GraphicsDeviceGL4.hpp"
#include "../RenderSystem/GraphicsCommandQueueImmediate.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Audio/AudioEngine.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/GraphicsCommandQueue.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Signals/Event.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

using Pomdog::Detail::GL4::GraphicsDeviceGL4;
using Pomdog::Detail::GL4::GraphicsContextGL4;
using Pomdog::Detail::InputSystem::Apple::GamepadIOKit;

namespace Pomdog {
namespace Detail {
namespace Cocoa {
namespace {

std::shared_ptr<OpenGLContextCocoa> CreateOpenGLContext(
    const PresentationParameters& presentationParameters)
{
    auto pixelFormat = CocoaOpenGLHelper::CreatePixelFormat(presentationParameters);
    return std::make_shared<OpenGLContextCocoa>(pixelFormat);
}

} // unnamed namespace

// MARK: - GameHostCocoa::Impl

class GameHostCocoa::Impl final {
public:
    Impl(PomdogOpenGLView* openGLView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<EventQueue>& eventQueue,
        const PresentationParameters& presentationParameters);

    ~Impl();

    void Run(const std::weak_ptr<Game>& game,
        std::function<void()>&& onCompleted);

    void Exit();

    std::shared_ptr<GameWindow> GetWindow();

    std::shared_ptr<GameClock> GetClock(std::shared_ptr<GameHost> && gameHost);

    std::shared_ptr<GraphicsDevice> GetGraphicsDevice();

    std::shared_ptr<GraphicsCommandQueue> GetGraphicsCommandQueue();

    std::shared_ptr<AssetManager> GetAssetManager(std::shared_ptr<GameHost> && gameHost);

    std::shared_ptr<AudioEngine> GetAudioEngine();

    std::shared_ptr<Keyboard> GetKeyboard();

    std::shared_ptr<Mouse> GetMouse();

    std::shared_ptr<Gamepad> GetGamepad();

private:
    void GameLoop();

    void RenderFrame();

    void DoEvents();

    void ProcessSystemEvents(const Event& event);

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
    std::atomic_bool viewLiveResizing;
    CVDisplayLinkRef displayLink;
    std::function<void()> onCompleted;

    std::weak_ptr<Game> weakGame;
    std::shared_ptr<EventQueue> eventQueue;
    std::shared_ptr<GameWindowCocoa> window;
    std::shared_ptr<OpenGLContextCocoa> openGLContext;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsContextGL4> graphicsContext;
    std::shared_ptr<GraphicsCommandQueue> graphicsCommandQueue;
    std::shared_ptr<AudioEngine> audioEngine;
    std::unique_ptr<Pomdog::AssetManager> assetManager;
    std::shared_ptr<KeyboardCocoa> keyboard;
    std::shared_ptr<MouseCocoa> mouse;
    std::shared_ptr<GamepadIOKit> gamepad;

    __weak PomdogOpenGLView* openGLView;
    Duration presentationInterval;
    bool exitRequest;
    bool displayLinkEnabled;
};

GameHostCocoa::Impl::Impl(
    PomdogOpenGLView* openGLViewIn,
    const std::shared_ptr<GameWindowCocoa>& windowIn,
    const std::shared_ptr<EventQueue>& eventQueueIn,
    const PresentationParameters& presentationParameters)
    : viewLiveResizing(false)
    , displayLink(nullptr)
    , eventQueue(eventQueueIn)
    , window(windowIn)
    , openGLView(openGLViewIn)
    , presentationInterval(Duration(1) / 60)
    , exitRequest(false)
    , displayLinkEnabled(true)
{
    POMDOG_ASSERT(window);
    window->SetView(openGLView);

    // Create OpenGL context
    openGLContext = CreateOpenGLContext(presentationParameters);

    POMDOG_ASSERT(openGLView != nil);
    [openGLView setEventQueue: eventQueue];
    [openGLView setOpenGLContext:openGLContext];

    // Create graphics device and device resources
    openGLContext->Lock();
    openGLContext->SetView(openGLView);
    openGLContext->MakeCurrent();

    graphicsDevice = std::make_shared<GraphicsDevice>(
        std::make_unique<GraphicsDeviceGL4>(presentationParameters));

    graphicsContext = std::make_shared<GraphicsContextGL4>(openGLContext, window);
    graphicsCommandQueue = std::make_shared<GraphicsCommandQueue>(
        std::make_unique<GraphicsCommandQueueImmediate>(graphicsContext));
    openGLContext->Unlock();

    // Create subsystems
    audioEngine = std::make_shared<Pomdog::AudioEngine>();
    keyboard = std::make_shared<KeyboardCocoa>();
    mouse = std::make_shared<MouseCocoa>();
    gamepad = std::make_shared<GamepadIOKit>(eventQueue);

    // Connect to system event signal
    POMDOG_ASSERT(eventQueue);
    systemEventConnection = eventQueue->Connect(
        [this](const Event& event) { ProcessSystemEvents(event); });

    Detail::AssetLoaderContext loaderContext;
    loaderContext.RootDirectory = PathHelper::Join(FileSystem::GetResourceDirectoryPath(), "Content");
    loaderContext.GraphicsDevice = graphicsDevice;
    assetManager = std::make_unique<Pomdog::AssetManager>(std::move(loaderContext));

    POMDOG_ASSERT(presentationParameters.PresentationInterval > 0);
    presentationInterval = Duration(1) / presentationParameters.PresentationInterval;

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &DisplayLinkCallback, this);
}

GameHostCocoa::Impl::~Impl()
{
    if (displayLink != nullptr) {
        CVDisplayLinkRelease(displayLink);
        displayLink = nullptr;
    }

    if (openGLView != nil) {
        [openGLView setEventQueue:{}];
    }

    systemEventConnection.Disconnect();
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
    openGLView = nil;
}

void GameHostCocoa::Impl::Run(
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
    game->Initialize();
    openGLContext->Unlock();

    if (exitRequest) {
        GameWillExit();
        return;
    }

    POMDOG_ASSERT(openGLView != nil);

    NSOpenGLContext* nsOpenGLContext = openGLContext->NativeOpenGLContext();
    NSOpenGLPixelFormat* nsPixelFormat = [nsOpenGLContext pixelFormat];
    CGLContextObj cglContext = [nsOpenGLContext CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [nsPixelFormat CGLPixelFormatObj];

    [openGLView setResizingCallback: [this](bool resizing) {
        viewLiveResizing = resizing;
    }];

    [openGLView setRenderCallback: [this] {
        std::lock_guard<std::mutex> lock(renderMutex);
        ClientSizeChanged();
        RenderFrame();
    }];

    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(
        displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

void GameHostCocoa::Impl::GameWillExit()
{
    if (openGLView != nil) {
        [openGLView setRenderCallback: []{}];
    }

    if (window) {
        window->SetView(nil);
    }

    if (onCompleted) {
        dispatch_async(dispatch_get_main_queue(), [=] {
            onCompleted();
        });
    }
}

void GameHostCocoa::Impl::Exit()
{
    exitRequest = true;

    if (displayLinkEnabled) {
        if (displayLink != nullptr) {
            CVDisplayLinkStop(displayLink);
        }
        GameWillExit();
    }
}

CVReturn GameHostCocoa::Impl::DisplayLinkCallback(
    [[maybe_unused]] CVDisplayLinkRef displayLink,
    [[maybe_unused]] const CVTimeStamp* now,
    [[maybe_unused]] const CVTimeStamp* outputTime,
    [[maybe_unused]] CVOptionFlags flagsIn,
    [[maybe_unused]] CVOptionFlags* flagsOut,
    void* displayLinkContext)
{
    auto gameHost = reinterpret_cast<GameHostCocoa::Impl*>(displayLinkContext);
    POMDOG_ASSERT(gameHost != nullptr);
    gameHost->GameLoop();
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

    bool skipRender = (!window || window->IsMinimized()
        || [NSApp isHidden] == YES);

    if (skipRender) {
        return;
    }

    auto game = weakGame.lock();

    POMDOG_ASSERT(game);
    POMDOG_ASSERT(openGLView != nil);

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

void GameHostCocoa::Impl::ProcessSystemEvents(const Event& event)
{
    if (event.Is<WindowShouldCloseEvent>()) {
        Log::Internal("WindowShouldCloseEvent");
        this->Exit();
    }
    else if (event.Is<WindowWillCloseEvent>()) {
        Log::Internal("WindowWillCloseEvent");
    }
    else if (event.Is<ViewWillStartLiveResizeEvent>()) {
        auto rect = window->GetClientBounds();
        Log::Internal(StringHelper::Format(
            "ViewWillStartLiveResizeEvent: {w: %d, h: %d}",
            rect.Width, rect.Height));
    }
    else if (event.Is<ViewDidEndLiveResizeEvent>()) {
        auto rect = window->GetClientBounds();
        Log::Internal(StringHelper::Format(
            "ViewDidEndLiveResizeEvent: {w: %d, h: %d}",
            rect.Width, rect.Height));
    }
    else {
        POMDOG_ASSERT(keyboard);
        POMDOG_ASSERT(mouse);
        POMDOG_ASSERT(gamepad);
        keyboard->HandleEvent(event);
        mouse->HandleEvent(event);
        gamepad->HandleEvent(event);
    }
}

void GameHostCocoa::Impl::ClientSizeChanged()
{
    openGLContext->Lock();
    openGLContext->MakeCurrent();

    POMDOG_ASSERT(openGLContext->NativeOpenGLContext() != nil);
    [openGLContext->NativeOpenGLContext() update];

    POMDOG_ASSERT(graphicsDevice);
    POMDOG_ASSERT(graphicsDevice->GetNativeGraphicsDevice());

    auto nativeDevice = static_cast<GraphicsDeviceGL4*>(graphicsDevice->GetNativeGraphicsDevice());
    auto bounds = window->GetClientBounds();

    nativeDevice->ClientSizeChanged(bounds.Width, bounds.Height);
    window->ClientSizeChanged(bounds.Width, bounds.Height);

    openGLContext->Unlock();
}

std::shared_ptr<GameWindow> GameHostCocoa::Impl::GetWindow()
{
    return window;
}

std::shared_ptr<GameClock> GameHostCocoa::Impl::GetClock(std::shared_ptr<GameHost> && gameHost)
{
    std::shared_ptr<GameClock> sharedClock(gameHost, &clock);
    return sharedClock;
}

std::shared_ptr<GraphicsDevice> GameHostCocoa::Impl::GetGraphicsDevice()
{
    return graphicsDevice;
}

std::shared_ptr<GraphicsCommandQueue> GameHostCocoa::Impl::GetGraphicsCommandQueue()
{
    return graphicsCommandQueue;
}

std::shared_ptr<AudioEngine> GameHostCocoa::Impl::GetAudioEngine()
{
    return audioEngine;
}

std::shared_ptr<AssetManager> GameHostCocoa::Impl::GetAssetManager(std::shared_ptr<GameHost> && gameHost)
{
    std::shared_ptr<AssetManager> sharedAssetManager(gameHost, assetManager.get());
    return sharedAssetManager;
}

std::shared_ptr<Keyboard> GameHostCocoa::Impl::GetKeyboard()
{
    return keyboard;
}

std::shared_ptr<Mouse> GameHostCocoa::Impl::GetMouse()
{
    return mouse;
}

std::shared_ptr<Gamepad> GameHostCocoa::Impl::GetGamepad()
{
    return gamepad;
}

// MARK: - GameHostCocoa

GameHostCocoa::GameHostCocoa(
    PomdogOpenGLView* openGLView,
    const std::shared_ptr<GameWindowCocoa>& window,
    const std::shared_ptr<EventQueue>& eventQueue,
    const PresentationParameters& presentationParameters)
    : impl(std::make_unique<Impl>(openGLView, window, eventQueue, presentationParameters))
{
}

GameHostCocoa::~GameHostCocoa() = default;

void GameHostCocoa::Run(
    const std::weak_ptr<Game>& game,
    std::function<void()>&& onCompleted)
{
    POMDOG_ASSERT(impl);
    impl->Run(game, std::move(onCompleted));
}

void GameHostCocoa::Exit()
{
    POMDOG_ASSERT(impl);
    impl->Exit();
}

std::shared_ptr<GameWindow> GameHostCocoa::GetWindow()
{
    POMDOG_ASSERT(impl);
    return impl->GetWindow();
}

std::shared_ptr<GameClock> GameHostCocoa::GetClock()
{
    POMDOG_ASSERT(impl);
    return impl->GetClock(shared_from_this());
}

std::shared_ptr<GraphicsDevice> GameHostCocoa::GetGraphicsDevice()
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsDevice();
}

std::shared_ptr<GraphicsCommandQueue> GameHostCocoa::GetGraphicsCommandQueue()
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsCommandQueue();
}

std::shared_ptr<AudioEngine> GameHostCocoa::GetAudioEngine()
{
    POMDOG_ASSERT(impl);
    return impl->GetAudioEngine();
}

std::shared_ptr<AssetManager> GameHostCocoa::GetAssetManager()
{
    POMDOG_ASSERT(impl);
    return impl->GetAssetManager(shared_from_this());
}

std::shared_ptr<Keyboard> GameHostCocoa::GetKeyboard()
{
    POMDOG_ASSERT(impl);
    return impl->GetKeyboard();
}

std::shared_ptr<Mouse> GameHostCocoa::GetMouse()
{
    POMDOG_ASSERT(impl);
    return impl->GetMouse();
}

std::shared_ptr<Gamepad> GameHostCocoa::GetGamepad()
{
    POMDOG_ASSERT(impl);
    return impl->GetGamepad();
}

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
