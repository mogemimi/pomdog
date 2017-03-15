// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "GameHostMetal.hpp"
#include "GameWindowCocoa.hpp"
#include "KeyboardCocoa.hpp"
#include "MouseCocoa.hpp"
#include "../RenderSystem/GraphicsCommandQueueImmediate.hpp"
#include "../RenderSystem.Metal/GraphicsContextMetal.hpp"
#include "../RenderSystem.Metal/GraphicsDeviceMetal.hpp"
#include "../Application/SystemEvents.hpp"
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
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <utility>
#include <vector>
#include <mutex>
#include <thread>

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

using Pomdog::Detail::Metal::GraphicsContextMetal;
using Pomdog::Detail::Metal::GraphicsDeviceMetal;

namespace Pomdog {
namespace Detail {
namespace Cocoa {
namespace {

// MARK: This code is dup from 'src/RenderSystem.Metal/RenderTarget2DMetal.mm'
MTLPixelFormat ToMTLPixelFormat(DepthFormat depthFormat)
{
    POMDOG_ASSERT(depthFormat != DepthFormat::None);
    POMDOG_ASSERT_MESSAGE(depthFormat != DepthFormat::Depth16, "Not supported");

    switch (depthFormat) {
    case DepthFormat::Depth16: return MTLPixelFormatDepth32Float;
    case DepthFormat::Depth32: return MTLPixelFormatDepth32Float;
#if defined(MAC_OS_X_VERSION_10_11) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_11)
    case DepthFormat::Depth24Stencil8: return MTLPixelFormatDepth24Unorm_Stencil8;
#else
    case DepthFormat::Depth24Stencil8: return MTLPixelFormatDepth32Float_Stencil8;
#endif
    case DepthFormat::Depth32_Float_Stencil8_Uint: return MTLPixelFormatDepth32Float_Stencil8;
    case DepthFormat::None: return MTLPixelFormatInvalid;
    }
    return MTLPixelFormatDepth32Float;
}

void SetupMetalView(
    MTKView* view,
    id <MTLDevice> device,
    const PresentationParameters& presentationParameters)
{
    // Set the view to use the default device
    view.device = device;

    // Setup the render target, choose values based on your app
    view.sampleCount = presentationParameters.MultiSampleCount;
    view.depthStencilPixelFormat = ToMTLPixelFormat(presentationParameters.DepthStencilFormat);
}

} // unnamed namespace

// MARK: GameHostMetal::Impl

class GameHostMetal::Impl final {
public:
    Impl(
        MTKView* metalView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<EventQueue>& eventQueue,
        const PresentationParameters& presentationParameters);

    ~Impl();

    void InitializeGame(
        const std::weak_ptr<Game>& game,
        const std::function<void()>& onCompleted);

    void GameLoop();

    bool IsMetalSupported() const;

    void Exit();

    std::shared_ptr<GameWindow> GetWindow();

    std::shared_ptr<GameClock> GetClock(std::shared_ptr<GameHost> && gameHost);

    std::shared_ptr<GraphicsDevice> GetGraphicsDevice();

    std::shared_ptr<GraphicsCommandQueue> GetGraphicsCommandQueue();

    std::shared_ptr<AssetManager> GetAssetManager(std::shared_ptr<GameHost> && gameHost);

    std::shared_ptr<AudioEngine> GetAudioEngine();

    std::shared_ptr<Keyboard> GetKeyboard();

    std::shared_ptr<Mouse> GetMouse();

    SurfaceFormat GetBackBufferSurfaceFormat() const noexcept;

    DepthFormat GetBackBufferDepthStencilFormat() const noexcept;

private:
    void RenderFrame();

    void DoEvents();

    void ProcessSystemEvents(const Event& event);

    void ClientSizeChanged();

    void GameWillExit();

private:
    GameClock clock;
    ScopedConnection systemEventConnection;
    std::atomic_bool viewLiveResizing;
    std::function<void()> onCompleted;

    std::weak_ptr<Game> weakGame;
    std::shared_ptr<EventQueue> eventQueue;
    std::shared_ptr<GameWindowCocoa> window;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsContextMetal> graphicsContext;
    std::shared_ptr<GraphicsCommandQueue> graphicsCommandQueue;
    std::shared_ptr<AudioEngine> audioEngine;
    std::unique_ptr<Pomdog::AssetManager> assetManager;
    std::shared_ptr<KeyboardCocoa> keyboard;
    std::shared_ptr<MouseCocoa> mouse;

    __weak MTKView* metalView;
    Duration presentationInterval;
    SurfaceFormat backBufferSurfaceFormat;
    DepthFormat backBufferDepthStencilFormat;
    bool exitRequest;
};

GameHostMetal::Impl::Impl(
    MTKView* metalViewIn,
    const std::shared_ptr<GameWindowCocoa>& windowIn,
    const std::shared_ptr<EventQueue>& eventQueueIn,
    const PresentationParameters& presentationParameters)
    : viewLiveResizing(false)
    , eventQueue(eventQueueIn)
    , window(windowIn)
    , metalView(metalViewIn)
    , presentationInterval(Duration(1) / 60)
    , backBufferSurfaceFormat(presentationParameters.BackBufferFormat)
    , backBufferDepthStencilFormat(presentationParameters.DepthStencilFormat)
    , exitRequest(false)
{
    POMDOG_ASSERT(window);
    POMDOG_ASSERT(metalView != nil);

    window->SetView(metalView);

    // Create graphics device
    graphicsDevice = std::make_shared<GraphicsDevice>(
        std::make_unique<GraphicsDeviceMetal>());

    // Get MTLDevice object
    POMDOG_ASSERT(graphicsDevice);
    auto graphicsDeviceMetal = dynamic_cast<GraphicsDeviceMetal*>(
        graphicsDevice->GetNativeGraphicsDevice());

    POMDOG_ASSERT(graphicsDeviceMetal);
    id <MTLDevice> metalDevice = graphicsDeviceMetal->GetMTLDevice();

    if (metalDevice == nil) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Error: Metal is not supported on this device");
    }

    // Setup metal view
    SetupMetalView(metalView, metalDevice, presentationParameters);

    POMDOG_ASSERT(metalDevice != nil);

    // Create graphics context
    graphicsContext = std::make_shared<GraphicsContextMetal>(metalDevice);
    graphicsContext->SetMTKView(metalView);

    // Create graphics command queue
    graphicsCommandQueue = std::make_shared<GraphicsCommandQueue>(
        std::make_unique<GraphicsCommandQueueImmediate>(graphicsContext));

    // Create subsystems
    audioEngine = std::make_shared<AudioEngine>();
    keyboard = std::make_shared<KeyboardCocoa>();
    mouse = std::make_shared<MouseCocoa>();

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
}

GameHostMetal::Impl::~Impl()
{
    systemEventConnection.Disconnect();
    assetManager.reset();
    keyboard.reset();
    mouse.reset();
    audioEngine.reset();
    graphicsCommandQueue.reset();
    graphicsContext.reset();
    graphicsDevice.reset();
    window.reset();
    eventQueue.reset();
    metalView = nil;
}

void GameHostMetal::Impl::InitializeGame(
    const std::weak_ptr<Game>& weakGameIn,
    const std::function<void()>& onCompletedIn)
{
    POMDOG_ASSERT(!weakGameIn.expired());
    POMDOG_ASSERT(onCompletedIn);
    weakGame = weakGameIn;
    onCompleted = onCompletedIn;

    POMDOG_ASSERT(!weakGame.expired());
    auto game = weakGame.lock();

    game->Initialize();

    if (exitRequest) {
        GameWillExit();
    }
}

bool GameHostMetal::Impl::IsMetalSupported() const
{
    if (!graphicsDevice) {
        return false;
    }

    // Get MTLDevice object
    POMDOG_ASSERT(graphicsDevice);
    auto graphicsDeviceMetal = dynamic_cast<GraphicsDeviceMetal*>(
        graphicsDevice->GetNativeGraphicsDevice());

    POMDOG_ASSERT(graphicsDeviceMetal);
    id <MTLDevice> metalDevice = graphicsDeviceMetal->GetMTLDevice();

    return metalDevice != nil;
}

void GameHostMetal::Impl::GameWillExit()
{
    if (window) {
        window->SetView(nil);
    }

    if (onCompleted) {
        dispatch_async(dispatch_get_main_queue(), [=] {
            onCompleted();
        });
    }
}

void GameHostMetal::Impl::Exit()
{
    exitRequest = true;
    GameWillExit();
}

void GameHostMetal::Impl::GameLoop()
{
    POMDOG_ASSERT(graphicsContext != nullptr);

    if (exitRequest) {
        return;
    }

    graphicsContext->DispatchSemaphoreWait();

    POMDOG_ASSERT(!exitRequest);
    POMDOG_ASSERT(!weakGame.expired());

    auto game = weakGame.lock();
    POMDOG_ASSERT(game);

    clock.Tick();
    DoEvents();

    if (exitRequest) {
        return;
    }

    game->Update();

    if (!viewLiveResizing.load()) {
        RenderFrame();
    }
}

void GameHostMetal::Impl::RenderFrame()
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

    game->Draw();
}

void GameHostMetal::Impl::DoEvents()
{
    eventQueue->Emit();
}

void GameHostMetal::Impl::ProcessSystemEvents(const Event& event)
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

        ClientSizeChanged();
    }
    else {
        POMDOG_ASSERT(keyboard);
        POMDOG_ASSERT(mouse);
        keyboard->HandleEvent(event);
        mouse->HandleEvent(event);
    }
}

void GameHostMetal::Impl::ClientSizeChanged()
{
    auto bounds = window->GetClientBounds();
    window->ClientSizeChanged(bounds.Width, bounds.Height);
}

std::shared_ptr<GameWindow> GameHostMetal::Impl::GetWindow()
{
    return window;
}

std::shared_ptr<GameClock> GameHostMetal::Impl::GetClock(std::shared_ptr<GameHost> && gameHost)
{
    std::shared_ptr<GameClock> sharedClock(gameHost, &clock);
    return sharedClock;
}

std::shared_ptr<GraphicsDevice> GameHostMetal::Impl::GetGraphicsDevice()
{
    return graphicsDevice;
}

std::shared_ptr<GraphicsCommandQueue> GameHostMetal::Impl::GetGraphicsCommandQueue()
{
    return graphicsCommandQueue;
}

std::shared_ptr<AudioEngine> GameHostMetal::Impl::GetAudioEngine()
{
    return audioEngine;
}

std::shared_ptr<AssetManager> GameHostMetal::Impl::GetAssetManager(std::shared_ptr<GameHost> && gameHost)
{
    std::shared_ptr<AssetManager> sharedAssetManager(gameHost, assetManager.get());
    return sharedAssetManager;
}

std::shared_ptr<Keyboard> GameHostMetal::Impl::GetKeyboard()
{
    return keyboard;
}

std::shared_ptr<Mouse> GameHostMetal::Impl::GetMouse()
{
    return mouse;
}

SurfaceFormat GameHostMetal::Impl::GetBackBufferSurfaceFormat() const noexcept
{
    return backBufferSurfaceFormat;
}

DepthFormat GameHostMetal::Impl::GetBackBufferDepthStencilFormat() const noexcept
{
    return backBufferDepthStencilFormat;
}

// MARK: GameHostMetal

GameHostMetal::GameHostMetal(
    MTKView* metalView,
    const std::shared_ptr<GameWindowCocoa>& window,
    const std::shared_ptr<EventQueue>& eventQueue,
    const PresentationParameters& presentationParameters)
    : impl(std::make_unique<Impl>(metalView, window, eventQueue, presentationParameters))
{}

GameHostMetal::~GameHostMetal() = default;

void GameHostMetal::InitializeGame(
    const std::weak_ptr<Game>& game,
    const std::function<void()>& onCompleted)
{
    POMDOG_ASSERT(impl);
    impl->InitializeGame(game, onCompleted);
}

void GameHostMetal::GameLoop()
{
    POMDOG_ASSERT(impl);
    impl->GameLoop();
}

bool GameHostMetal::IsMetalSupported() const
{
    POMDOG_ASSERT(impl);
    return impl->IsMetalSupported();
}

void GameHostMetal::Exit()
{
    POMDOG_ASSERT(impl);
    impl->Exit();
}

std::shared_ptr<GameWindow> GameHostMetal::GetWindow()
{
    POMDOG_ASSERT(impl);
    return impl->GetWindow();
}

std::shared_ptr<GameClock> GameHostMetal::GetClock()
{
    POMDOG_ASSERT(impl);
    return impl->GetClock(shared_from_this());
}

std::shared_ptr<GraphicsDevice> GameHostMetal::GetGraphicsDevice()
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsDevice();
}

std::shared_ptr<GraphicsCommandQueue> GameHostMetal::GetGraphicsCommandQueue()
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsCommandQueue();
}

std::shared_ptr<AudioEngine> GameHostMetal::GetAudioEngine()
{
    POMDOG_ASSERT(impl);
    return impl->GetAudioEngine();
}

std::shared_ptr<AssetManager> GameHostMetal::GetAssetManager()
{
    POMDOG_ASSERT(impl);
    return impl->GetAssetManager(shared_from_this());
}

std::shared_ptr<Keyboard> GameHostMetal::GetKeyboard()
{
    POMDOG_ASSERT(impl);
    return impl->GetKeyboard();
}

std::shared_ptr<Mouse> GameHostMetal::GetMouse()
{
    POMDOG_ASSERT(impl);
    return impl->GetMouse();
}

SurfaceFormat GameHostMetal::GetBackBufferSurfaceFormat() const
{
    POMDOG_ASSERT(impl);
    return impl->GetBackBufferSurfaceFormat();
}

DepthFormat GameHostMetal::GetBackBufferDepthStencilFormat() const
{
    POMDOG_ASSERT(impl);
    return impl->GetBackBufferDepthStencilFormat();
}

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
