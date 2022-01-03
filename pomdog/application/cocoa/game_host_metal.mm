// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/cocoa/game_host_metal.h"
#include "pomdog/application/cocoa/game_window_cocoa.h"
#include "pomdog/application/game.h"
#include "pomdog/application/system_events.h"
#include "pomdog/audio/openal/audio_engine_al.h"
#include "pomdog/chrono/apple/time_source_apple.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/metal/frame_counter.h"
#include "pomdog/gpu/metal/graphics_context_metal.h"
#include "pomdog/gpu/metal/graphics_device_metal.h"
#include "pomdog/gpu/metal/metal_format_helper.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/input/cocoa/keyboard_cocoa.h"
#include "pomdog/input/cocoa/mouse_cocoa.h"
#include "pomdog/input/iokit/gamepad_iokit.h"
#include "pomdog/input/key_state.h"
#include "pomdog/logging/log.h"
#include "pomdog/network/http_client.h"
#include "pomdog/network/io_service.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"
#include "pomdog/utility/string_helper.h"
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

using pomdog::gpu::detail::metal::GraphicsContextMetal;
using pomdog::gpu::detail::metal::GraphicsDeviceMetal;
using pomdog::gpu::detail::metal::ToPixelFormat;
using pomdog::gpu::detail::metal::FrameCounter;
using pomdog::detail::IOKit::GamepadIOKit;
using pomdog::detail::openal::AudioEngineAL;

namespace pomdog::detail::cocoa {
namespace {

void SetupMetalView(
    MTKView* view,
    id<MTLDevice> device,
    const gpu::PresentationParameters& presentationParameters)
{
    // Set the view to use the default device
    view.device = device;

    // Setup the render target, choose values based on your app
    view.sampleCount = presentationParameters.MultiSampleCount;
    view.depthStencilPixelFormat = ToPixelFormat(presentationParameters.DepthStencilFormat);
}

} // namespace

// MARK: GameHostMetal::Impl

class GameHostMetal::Impl final {
public:
    ~Impl();

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        MTKView* metalView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const gpu::PresentationParameters& presentationParameters);

    [[nodiscard]] std::unique_ptr<Error>
    InitializeGame(
        const std::weak_ptr<Game>& game,
        const std::function<void()>& onCompleted);

    void GameLoop();

    bool IsMetalSupported() const;

    void Exit();

    [[nodiscard]] std::shared_ptr<GameWindow>
    GetWindow() noexcept;

    [[nodiscard]] std::shared_ptr<GameClock>
    GetClock() noexcept;

    [[nodiscard]] std::shared_ptr<gpu::GraphicsDevice>
    GetGraphicsDevice() noexcept;

    [[nodiscard]] std::shared_ptr<gpu::CommandQueue>
    GetCommandQueue() noexcept;

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
    void RenderFrame();

    void DoEvents();

    void ProcessSystemEvents(const SystemEvent& event);

    void ClientSizeChanged();

    void GameWillExit();

private:
    ScopedConnection systemEventConnection;
    std::atomic_bool viewLiveResizing = false;
    std::function<void()> onCompleted;

    std::weak_ptr<Game> weakGame;
    std::shared_ptr<detail::apple::TimeSourceApple> timeSource_;
    std::shared_ptr<GameClockImpl> clock_;
    std::shared_ptr<EventQueue<SystemEvent>> eventQueue;
    std::shared_ptr<GameWindowCocoa> window;
    std::shared_ptr<GraphicsDeviceMetal> graphicsDevice;
    std::shared_ptr<GraphicsContextMetal> graphicsContext;
    std::shared_ptr<gpu::CommandQueue> graphicsCommandQueue;
    std::shared_ptr<FrameCounter> frameCounter_;
    std::shared_ptr<AudioEngineAL> audioEngine;
    std::unique_ptr<AssetManager> assetManager;
    std::shared_ptr<KeyboardCocoa> keyboard;
    std::shared_ptr<MouseCocoa> mouse;
    std::shared_ptr<GamepadIOKit> gamepad;

    std::unique_ptr<IOService> ioService_;
    std::unique_ptr<HTTPClient> httpClient;

    __weak MTKView* metalView = nullptr;
    Duration presentationInterval = Duration::zero();
    bool exitRequest = false;
};

std::unique_ptr<Error>
GameHostMetal::Impl::Initialize(
    MTKView* metalViewIn,
    const std::shared_ptr<GameWindowCocoa>& windowIn,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueueIn,
    const gpu::PresentationParameters& presentationParameters)
{
    this->viewLiveResizing = false;
    this->eventQueue = eventQueueIn;
    this->window = windowIn;
    this->metalView = metalViewIn;
    this->presentationInterval = Duration(1) / 60;
    this->exitRequest = false;

    POMDOG_ASSERT(window);
    POMDOG_ASSERT(metalView != nullptr);

    timeSource_ = std::make_shared<detail::apple::TimeSourceApple>();
    clock_ = std::make_shared<GameClockImpl>();
    if (auto err = clock_->Initialize(presentationParameters.PresentationInterval, timeSource_); err != nullptr) {
        return errors::Wrap(std::move(err), "GameClockImpl::Initialize() failed.");
    }

    window->SetView(metalView);

    frameCounter_ = std::make_shared<FrameCounter>();

    // NOTE: Create graphics device
    graphicsDevice = std::make_shared<GraphicsDeviceMetal>();
    if (auto err = graphicsDevice->Initialize(presentationParameters, frameCounter_); err != nullptr) {
        return errors::New("failed to initialize GraphicsDeviceMetal");
    }

    // NOTE: Get MTLDevice object.
    POMDOG_ASSERT(graphicsDevice != nullptr);
    id<MTLDevice> metalDevice = graphicsDevice->GetMTLDevice();

    if (metalDevice == nullptr) {
        return errors::New("Metal is not supported on this device.");
    }

    // NOTE: Setup metal view
    SetupMetalView(metalView, metalDevice, presentationParameters);

    POMDOG_ASSERT(metalDevice != nullptr);

    // NOTE: Create graphics context
    graphicsContext = std::make_shared<GraphicsContextMetal>(metalDevice);
    graphicsContext->SetMTKView(metalView);

    // NOTE: Create graphics command queue
    graphicsCommandQueue = std::make_shared<gpu::detail::CommandQueueImmediate>(graphicsContext);

    // NOTE: Create audio engine.
    audioEngine = std::make_shared<AudioEngineAL>();
    if (auto err = audioEngine->Initialize(); err != nullptr) {
        return errors::Wrap(std::move(err), "AudioEngineAL::Initialize() failed.");
    }

    // NOTE: Create subsystems
    keyboard = std::make_shared<KeyboardCocoa>();
    mouse = std::make_shared<MouseCocoa>();

    // NOTE: Create gamepad
    gamepad = std::make_shared<GamepadIOKit>();
    if (auto err = gamepad->Initialize(eventQueue); err != nullptr) {
        return errors::Wrap(std::move(err), "GamepadIOKit::Initialize() failed.");
    }

    // NOTE: Connect to system event signal
    POMDOG_ASSERT(eventQueue);
    systemEventConnection = eventQueue->Connect(
        [this](const SystemEvent& event) { ProcessSystemEvents(event); });

    auto [resourceDir, resourceDirErr] = FileSystem::GetResourceDirectoryPath();
    if (resourceDirErr != nullptr) {
        return errors::Wrap(std::move(resourceDirErr), "FileSystem::GetResourceDirectoryPath() failed.");
    }
    auto contentDirectory = PathHelper::Join(resourceDir, "content");

    // NOTE: Create asset manager.
    assetManager = std::make_unique<AssetManager>(
        std::move(contentDirectory),
        audioEngine,
        graphicsDevice);

    ioService_ = std::make_unique<IOService>();
    if (auto err = ioService_->Initialize(clock_); err != nullptr) {
        return errors::Wrap(std::move(err), "IOService::Initialize() failed.");
    }
    httpClient = std::make_unique<HTTPClient>(ioService_.get());

    POMDOG_ASSERT(presentationParameters.PresentationInterval > 0);
    presentationInterval = Duration(1) / presentationParameters.PresentationInterval;

    return nullptr;
}

GameHostMetal::Impl::~Impl()
{
    systemEventConnection.Disconnect();
    httpClient.reset();
    if (auto err = ioService_->Shutdown(); err != nullptr) {
        Log::Warning("pomdog", err->ToString());
    }
    ioService_.reset();
    assetManager.reset();
    gamepad.reset();
    keyboard.reset();
    mouse.reset();
    audioEngine.reset();
    frameCounter_.reset();
    graphicsCommandQueue.reset();
    graphicsContext.reset();
    graphicsDevice.reset();
    window.reset();
    eventQueue.reset();
    metalView = nullptr;
}

std::unique_ptr<Error>
GameHostMetal::Impl::InitializeGame(
    const std::weak_ptr<Game>& weakGameIn,
    const std::function<void()>& onCompletedIn)
{
    POMDOG_ASSERT(!weakGameIn.expired());
    POMDOG_ASSERT(onCompletedIn);
    weakGame = weakGameIn;
    onCompleted = onCompletedIn;

    POMDOG_ASSERT(!weakGame.expired());
    auto game = weakGame.lock();

    if (auto err = game->Initialize(); err != nullptr) {
        GameWillExit();
        return errors::Wrap(std::move(err), "failed to initialize game");
    }

    if (exitRequest) {
        GameWillExit();
    }

    return nullptr;
}

bool GameHostMetal::Impl::IsMetalSupported() const
{
    if (!graphicsDevice) {
        return false;
    }

    // NOTE: Get MTLDevice object.
    POMDOG_ASSERT(graphicsDevice != nullptr);
    id<MTLDevice> metalDevice = graphicsDevice->GetMTLDevice();

    return metalDevice != nullptr;
}

void GameHostMetal::Impl::GameWillExit()
{
    if (window) {
        window->SetView(nullptr);
    }

    if (onCompleted) {
        dispatch_async(dispatch_get_main_queue(), [this] {
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
    frameCounter_->UpdateFrame();

    POMDOG_ASSERT(!exitRequest);
    POMDOG_ASSERT(!weakGame.expired());

    auto game = weakGame.lock();
    POMDOG_ASSERT(game);

    clock_->Tick();
    DoEvents();
    ioService_->Step();

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

    bool skipRender = (!window || window->IsMinimized() || [NSApp isHidden] == YES);

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

void GameHostMetal::Impl::ProcessSystemEvents(const SystemEvent& event)
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

        ClientSizeChanged();
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

void GameHostMetal::Impl::ClientSizeChanged()
{
    POMDOG_ASSERT(graphicsDevice != nullptr);
    auto bounds = window->GetClientBounds();

    graphicsDevice->ClientSizeChanged(bounds.Width, bounds.Height);
    window->ClientSizeChanged(bounds.Width, bounds.Height);
}

std::shared_ptr<GameWindow>
GameHostMetal::Impl::GetWindow() noexcept
{
    return window;
}

std::shared_ptr<GameClock>
GameHostMetal::Impl::GetClock() noexcept
{
    return clock_;
}

std::shared_ptr<gpu::GraphicsDevice>
GameHostMetal::Impl::GetGraphicsDevice() noexcept
{
    return graphicsDevice;
}

std::shared_ptr<gpu::CommandQueue>
GameHostMetal::Impl::GetCommandQueue() noexcept
{
    return graphicsCommandQueue;
}

std::shared_ptr<AudioEngine>
GameHostMetal::Impl::GetAudioEngine() noexcept
{
    return audioEngine;
}

std::shared_ptr<AssetManager>
GameHostMetal::Impl::GetAssetManager(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    std::shared_ptr<AssetManager> shared{gameHost, assetManager.get()};
    return shared;
}

std::shared_ptr<Keyboard>
GameHostMetal::Impl::GetKeyboard() noexcept
{
    return keyboard;
}

std::shared_ptr<Mouse>
GameHostMetal::Impl::GetMouse() noexcept
{
    return mouse;
}

std::shared_ptr<Gamepad>
GameHostMetal::Impl::GetGamepad() noexcept
{
    return gamepad;
}

std::shared_ptr<IOService>
GameHostMetal::Impl::GetIOService(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(ioService_ != nullptr);
    std::shared_ptr<IOService> shared{std::move(gameHost), ioService_.get()};
    return shared;
}

std::shared_ptr<HTTPClient>
GameHostMetal::Impl::GetHTTPClient(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(httpClient != nullptr);
    std::shared_ptr<HTTPClient> shared(std::move(gameHost), httpClient.get());
    return shared;
}

// MARK: GameHostMetal

GameHostMetal::GameHostMetal()
    : impl(std::make_unique<Impl>())
{
}

GameHostMetal::~GameHostMetal() = default;

std::unique_ptr<Error>
GameHostMetal::Initialize(
    MTKView* metalView,
    const std::shared_ptr<GameWindowCocoa>& window,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
    const gpu::PresentationParameters& presentationParameters)
{
    POMDOG_ASSERT(impl);
    return impl->Initialize(metalView, window, eventQueue, presentationParameters);
}

std::unique_ptr<Error>
GameHostMetal::InitializeGame(
    const std::weak_ptr<Game>& game,
    const std::function<void()>& onCompleted)
{
    POMDOG_ASSERT(impl != nullptr);
    return impl->InitializeGame(game, onCompleted);
}

void GameHostMetal::GameLoop()
{
    POMDOG_ASSERT(impl);
    impl->GameLoop();
}

bool GameHostMetal::IsMetalSupported() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->IsMetalSupported();
}

void GameHostMetal::Exit()
{
    POMDOG_ASSERT(impl);
    impl->Exit();
}

std::shared_ptr<GameWindow> GameHostMetal::GetWindow() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetWindow();
}

std::shared_ptr<GameClock> GameHostMetal::GetClock() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetClock();
}

std::shared_ptr<gpu::GraphicsDevice> GameHostMetal::GetGraphicsDevice() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetGraphicsDevice();
}

std::shared_ptr<gpu::CommandQueue> GameHostMetal::GetCommandQueue() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetCommandQueue();
}

std::shared_ptr<AudioEngine> GameHostMetal::GetAudioEngine() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetAudioEngine();
}

std::shared_ptr<AssetManager> GameHostMetal::GetAssetManager() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetAssetManager(shared_from_this());
}

std::shared_ptr<Keyboard> GameHostMetal::GetKeyboard() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetKeyboard();
}

std::shared_ptr<Mouse> GameHostMetal::GetMouse() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetMouse();
}

std::shared_ptr<Gamepad> GameHostMetal::GetGamepad() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetGamepad();
}

std::shared_ptr<IOService> GameHostMetal::GetIOService() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetIOService(shared_from_this());
}

std::shared_ptr<HTTPClient> GameHostMetal::GetHTTPClient() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->GetHTTPClient(shared_from_this());
}

} // namespace pomdog::detail::cocoa
