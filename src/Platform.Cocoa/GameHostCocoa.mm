// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "GameHostCocoa.hpp"
#include "GameWindowCocoa.hpp"
#include "OpenGLContextCocoa.hpp"
#include "KeyboardCocoa.hpp"
#include "MouseCocoa.hpp"
#include "../RenderSystem/GraphicsContext.hpp"
#include "../RenderSystem/GraphicsCommandQueueImmediate.hpp"
#include "../RenderSystem.GL4/GraphicsContextGL4.hpp"
#include "../RenderSystem.GL4/GraphicsDeviceGL4.hpp"
#include "../Application/SystemEvents.hpp"
#include "Pomdog/Platform/Cocoa/PomdogOpenGLView.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Audio/AudioEngine.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Signals/Event.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Graphics/GraphicsCommandQueue.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Input/KeyState.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <utility>
#include <vector>
#include <mutex>
#include <thread>

namespace Pomdog {
namespace Detail {
namespace Cocoa {
namespace {

static std::shared_ptr<OpenGLContextCocoa> CreateOpenGLContext(
    PresentationParameters const& presentationParameters)
{
    auto pixelFormat = CocoaOpenGLHelper::CreatePixelFormat(presentationParameters);
    return std::make_shared<OpenGLContextCocoa>(pixelFormat);
}
//-----------------------------------------------------------------------
static std::shared_ptr<GraphicsContext> CreateGraphicsContext(
    std::shared_ptr<OpenGLContextCocoa> const& openGLContext,
    std::weak_ptr<GameWindow> && gameWindow,
    PresentationParameters const& presentationParameters,
    std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
    POMDOG_ASSERT(openGLContext);
    POMDOG_ASSERT(!gameWindow.expired());
    using GL4::GraphicsContextGL4;

    auto nativeContext = std::make_unique<GraphicsContextGL4>(openGLContext, std::move(gameWindow));
    return std::make_shared<GraphicsContext>(std::move(nativeContext), presentationParameters);
}

} // unnamed namespace
//-----------------------------------------------------------------------
// MARK: - GameHostCocoa::Impl
//-----------------------------------------------------------------------
class GameHostCocoa::Impl final {
public:
    Impl(PomdogOpenGLView* openGLView,
        std::shared_ptr<GameWindowCocoa> const& window,
        std::shared_ptr<EventQueue> const& eventQueue,
        PresentationParameters const& presentationParameters);

    ~Impl();

    void Run(std::weak_ptr<Game> const& game,
        std::function<void()> const& onCompleted);

    void Exit();

    std::shared_ptr<Pomdog::GameWindow> GetWindow();

    std::shared_ptr<Pomdog::GameClock> GetClock(std::shared_ptr<GameHost> && gameHost);

    std::shared_ptr<Pomdog::GraphicsDevice> GetGraphicsDevice();

    std::shared_ptr<Pomdog::GraphicsCommandQueue> GetGraphicsCommandQueue();

    std::shared_ptr<Pomdog::AssetManager> GetAssetManager(std::shared_ptr<GameHost> && gameHost);

    std::shared_ptr<Pomdog::AudioEngine> GetAudioEngine();

    std::shared_ptr<Pomdog::Keyboard> GetKeyboard();

    std::shared_ptr<Pomdog::Mouse> GetMouse();

private:
    void GameLoop();

    void RenderFrame();

    void DoEvents();

    void ProcessSystemEvents(Event const& event);

    void ClientSizeChanged();

    void GameWillExit();

    static CVReturn DisplayLinkCallback(
        CVDisplayLinkRef displayLink,
        CVTimeStamp const* now,
        CVTimeStamp const* outputTime,
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
    std::shared_ptr<Pomdog::GraphicsDevice> graphicsDevice;
    std::shared_ptr<Detail::GraphicsContext> graphicsContext;
    std::shared_ptr<Pomdog::GraphicsCommandQueue> graphicsCommandQueue;
    std::shared_ptr<Pomdog::AudioEngine> audioEngine;
    std::unique_ptr<Pomdog::AssetManager> assetManager;
    std::shared_ptr<KeyboardCocoa> keyboard;
    std::shared_ptr<MouseCocoa> mouse;

    __weak PomdogOpenGLView* openGLView;
    Duration presentationInterval;
    bool exitRequest;
    bool displayLinkEnabled;
};
//-----------------------------------------------------------------------
GameHostCocoa::Impl::Impl(PomdogOpenGLView* openGLViewIn,
    std::shared_ptr<GameWindowCocoa> const& windowIn,
    std::shared_ptr<EventQueue> const& eventQueueIn,
    PresentationParameters const& presentationParameters)
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

    using Detail::GL4::GraphicsDeviceGL4;
    graphicsDevice = std::make_shared<Pomdog::GraphicsDevice>(std::make_unique<GraphicsDeviceGL4>());

    graphicsContext = CreateGraphicsContext(openGLContext, window, presentationParameters, graphicsDevice);
    graphicsCommandQueue = std::make_shared<Pomdog::GraphicsCommandQueue>(
        std::make_unique<GraphicsCommandQueueImmediate>(graphicsContext));
    openGLContext->Unlock();

    // Create subsystems
    audioEngine = std::make_shared<Pomdog::AudioEngine>();
    keyboard = std::make_shared<KeyboardCocoa>();
    mouse = std::make_shared<MouseCocoa>();

    // Connect to system event signal
    POMDOG_ASSERT(eventQueue);
    systemEventConnection = eventQueue->Connect(
        [this](Event const& event) { ProcessSystemEvents(event); });

    Detail::AssetLoaderContext loaderContext;
    loaderContext.RootDirectory = PathHelper::Join(FileSystem::GetResourceDirectoryPath(), "Content");
    loaderContext.GraphicsDevice = graphicsDevice;
    assetManager = std::make_unique<Pomdog::AssetManager>(std::move(loaderContext));

    POMDOG_ASSERT(presentationParameters.PresentationInterval > 0);
    presentationInterval = Duration(1) / presentationParameters.PresentationInterval;

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &DisplayLinkCallback, this);
}
//-----------------------------------------------------------------------
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
//-----------------------------------------------------------------------
void GameHostCocoa::Impl::Run(std::weak_ptr<Game> const& weakGameIn,
    std::function<void()> const& onCompletedIn)
{
    POMDOG_ASSERT(!weakGameIn.expired());
    POMDOG_ASSERT(onCompletedIn);
    weakGame = weakGameIn;
    onCompleted = onCompletedIn;

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
//-----------------------------------------------------------------------
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
//-----------------------------------------------------------------------
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
//-----------------------------------------------------------------------
CVReturn GameHostCocoa::Impl::DisplayLinkCallback(
    CVDisplayLinkRef displayLink,
    CVTimeStamp const* now,
    CVTimeStamp const* outputTime,
    CVOptionFlags flagsIn,
    CVOptionFlags* flagsOut,
    void* displayLinkContext)
{
    auto gameHost = reinterpret_cast<GameHostCocoa::Impl*>(displayLinkContext);
    POMDOG_ASSERT(gameHost != nullptr);
    gameHost->GameLoop();
    return kCVReturnSuccess;
}
//-----------------------------------------------------------------------
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
        auto elapsedTime = clock.ElapsedTime();

        if (elapsedTime < presentationInterval) {
            lock.~lock_guard();
            auto sleepTime = (presentationInterval - elapsedTime);
            std::this_thread::sleep_for(sleepTime);
        }
    }
}
//-----------------------------------------------------------------------
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
//-----------------------------------------------------------------------
void GameHostCocoa::Impl::DoEvents()
{
    eventQueue->Emit();
}
//-----------------------------------------------------------------------
void GameHostCocoa::Impl::ProcessSystemEvents(Event const& event)
{
    if (event.Is<WindowShouldCloseEvent>())
    {
        Log::Internal("WindowShouldCloseEvent");
        this->Exit();
    }
    else if (event.Is<WindowWillCloseEvent>())
    {
        Log::Internal("WindowWillCloseEvent");
    }
    else if (event.Is<ViewWillStartLiveResizeEvent>())
    {
        auto rect = window->GetClientBounds();
        Log::Internal(StringHelper::Format(
            "ViewWillStartLiveResizeEvent: {w: %d, h: %d}",
            rect.Width, rect.Height));
    }
    else if (event.Is<ViewDidEndLiveResizeEvent>())
    {
        auto rect = window->GetClientBounds();
        Log::Internal(StringHelper::Format(
            "ViewDidEndLiveResizeEvent: {w: %d, h: %d}",
            rect.Width, rect.Height));
    }
    else if (auto keyEvent = event.As<InputKeyEvent>())
    {
        POMDOG_ASSERT(keyboard);
        keyboard->SetKey(keyEvent->Key, keyEvent->State);
    }
    else if (auto inputTextEvent = event.As<InputTextEvent>())
    {
        POMDOG_ASSERT(keyboard);
        keyboard->Keyboard::TextInput(inputTextEvent->text);
    }
    else if (auto mousePositionEvent = event.As<MousePositionEvent>())
    {
        POMDOG_ASSERT(mouse);
        mouse->Position(mousePositionEvent->Position);
    }
    else if (auto mouseButtonEvent = event.As<MouseButtonEvent>())
    {
        POMDOG_ASSERT(mouse);
        if (mouseButtonEvent->Button == MouseButtons::Left) {
            mouse->LeftButton(mouseButtonEvent->State == MouseButtonState::Up
                ? ButtonState::Released
                : ButtonState::Pressed);
        }
        else if (mouseButtonEvent->Button == MouseButtons::Right) {
            mouse->RightButton(mouseButtonEvent->State == MouseButtonState::Up
                ? ButtonState::Released
                : ButtonState::Pressed);
        }
        else if (mouseButtonEvent->Button == MouseButtons::Middle) {
            mouse->MiddleButton(mouseButtonEvent->State == MouseButtonState::Up
                ? ButtonState::Released
                : ButtonState::Pressed);
        }
        else if (mouseButtonEvent->Button == MouseButtons::XButton1) {
            mouse->XButton1(mouseButtonEvent->State == MouseButtonState::Up
                ? ButtonState::Released
                : ButtonState::Pressed);
        }
        else if (mouseButtonEvent->Button == MouseButtons::XButton2) {
            mouse->XButton2(mouseButtonEvent->State == MouseButtonState::Up
                ? ButtonState::Released
                : ButtonState::Pressed);
        }
        mouse->Position(mouseButtonEvent->Position);
    }
    else if (auto scrollWheelEvent = event.As<ScrollWheelEvent>())
    {
        POMDOG_ASSERT(mouse);
        mouse->WheelDelta(scrollWheelEvent->ScrollingDeltaY);
    }
}
//-----------------------------------------------------------------------
void GameHostCocoa::Impl::ClientSizeChanged()
{
    openGLContext->Lock();
    openGLContext->MakeCurrent();
    {
        POMDOG_ASSERT(openGLContext->NativeOpenGLContext() != nil);
        [openGLContext->NativeOpenGLContext() update];

        auto bounds = window->GetClientBounds();
        window->ClientSizeChanged(bounds.Width, bounds.Height);
    }
    openGLContext->Unlock();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameWindow> GameHostCocoa::Impl::GetWindow()
{
    return window;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameClock> GameHostCocoa::Impl::GetClock(std::shared_ptr<GameHost> && gameHost)
{
    std::shared_ptr<Pomdog::GameClock> sharedClock(gameHost, &clock);
    return std::move(sharedClock);
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsDevice> GameHostCocoa::Impl::GetGraphicsDevice()
{
    return graphicsDevice;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsCommandQueue> GameHostCocoa::Impl::GetGraphicsCommandQueue()
{
    return graphicsCommandQueue;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AudioEngine> GameHostCocoa::Impl::GetAudioEngine()
{
    return audioEngine;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AssetManager> GameHostCocoa::Impl::GetAssetManager(std::shared_ptr<GameHost> && gameHost)
{
    std::shared_ptr<Pomdog::AssetManager> sharedAssetManager(gameHost, assetManager.get());
    return std::move(sharedAssetManager);
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Keyboard> GameHostCocoa::Impl::GetKeyboard()
{
    return keyboard;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Mouse> GameHostCocoa::Impl::GetMouse()
{
    return mouse;
}
//-----------------------------------------------------------------------
// MARK: - GameHostCocoa
//-----------------------------------------------------------------------
GameHostCocoa::GameHostCocoa(
    PomdogOpenGLView* openGLView,
    std::shared_ptr<GameWindowCocoa> const& window,
    std::shared_ptr<EventQueue> const& eventQueue,
    PresentationParameters const& presentationParameters)
    : impl(std::make_unique<Impl>(openGLView, window, eventQueue, presentationParameters))
{}
//-----------------------------------------------------------------------
GameHostCocoa::~GameHostCocoa() = default;
//-----------------------------------------------------------------------
void GameHostCocoa::Run(std::weak_ptr<Game> const& game,
    std::function<void()> const& onCompleted)
{
    POMDOG_ASSERT(impl);
    impl->Run(game, onCompleted);
}
//-----------------------------------------------------------------------
void GameHostCocoa::Exit()
{
    POMDOG_ASSERT(impl);
    impl->Exit();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameWindow> GameHostCocoa::Window()
{
    POMDOG_ASSERT(impl);
    return impl->GetWindow();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameClock> GameHostCocoa::Clock()
{
    POMDOG_ASSERT(impl);
    return impl->GetClock(shared_from_this());
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsDevice> GameHostCocoa::GraphicsDevice()
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsDevice();
}
//-----------------------------------------------------------------------
std::shared_ptr<GraphicsCommandQueue> GameHostCocoa::GraphicsCommandQueue()
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsCommandQueue();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AudioEngine> GameHostCocoa::AudioEngine()
{
    POMDOG_ASSERT(impl);
    return impl->GetAudioEngine();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AssetManager> GameHostCocoa::AssetManager()
{
    POMDOG_ASSERT(impl);
    return impl->GetAssetManager(shared_from_this());
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Keyboard> GameHostCocoa::Keyboard()
{
    POMDOG_ASSERT(impl);
    return impl->GetKeyboard();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Mouse> GameHostCocoa::Mouse()
{
    POMDOG_ASSERT(impl);
    return impl->GetMouse();
}
//-----------------------------------------------------------------------
} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
