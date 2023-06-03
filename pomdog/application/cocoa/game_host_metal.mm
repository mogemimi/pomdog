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
    view.sampleCount = presentationParameters.multiSampleCount;
    view.depthStencilPixelFormat = ToPixelFormat(presentationParameters.depthStencilFormat);
}

} // namespace

// MARK: GameHostMetal::Impl

class GameHostMetal::Impl final {
public:
    ~Impl();

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        MTKView* metalView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const gpu::PresentationParameters& presentationParameters);

    [[nodiscard]] std::unique_ptr<Error>
    initializeGame(
        const std::weak_ptr<Game>& game,
        const std::function<void()>& onCompleted);

    void gameLoop();

    bool isMetalSupported() const;

    void exit();

    [[nodiscard]] std::shared_ptr<GameWindow>
    getWindow() noexcept;

    [[nodiscard]] std::shared_ptr<GameClock>
    getClock() noexcept;

    [[nodiscard]] std::shared_ptr<gpu::GraphicsDevice>
    getGraphicsDevice() noexcept;

    [[nodiscard]] std::shared_ptr<gpu::CommandQueue>
    getCommandQueue() noexcept;

    [[nodiscard]] std::shared_ptr<AssetManager>
    getAssetManager(std::shared_ptr<GameHost>&& gameHost) noexcept;

    [[nodiscard]] std::shared_ptr<AudioEngine>
    getAudioEngine() noexcept;

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
    void renderFrame();

    void doEvents();

    void processSystemEvents(const SystemEvent& event);

    void clientSizeChanged();

    void gameWillExit();

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
GameHostMetal::Impl::initialize(
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
    if (auto err = clock_->initialize(presentationParameters.presentationInterval, timeSource_); err != nullptr) {
        return errors::wrap(std::move(err), "GameClockImpl::Initialize() failed.");
    }

    window->setView(metalView);

    frameCounter_ = std::make_shared<FrameCounter>();

    // NOTE: Create graphics device
    graphicsDevice = std::make_shared<GraphicsDeviceMetal>();
    if (auto err = graphicsDevice->Initialize(presentationParameters, frameCounter_); err != nullptr) {
        return errors::make("failed to initialize GraphicsDeviceMetal");
    }

    // NOTE: Get MTLDevice object.
    POMDOG_ASSERT(graphicsDevice != nullptr);
    id<MTLDevice> metalDevice = graphicsDevice->GetMTLDevice();

    if (metalDevice == nullptr) {
        return errors::make("Metal is not supported on this device.");
    }

    // NOTE: Setup metal view
    SetupMetalView(metalView, metalDevice, presentationParameters);

    POMDOG_ASSERT(metalDevice != nullptr);

    // NOTE: Create graphics context
    graphicsContext = std::make_shared<GraphicsContextMetal>(metalDevice);
    graphicsContext->setMTKView(metalView);

    // NOTE: Create graphics command queue
    graphicsCommandQueue = std::make_shared<gpu::detail::CommandQueueImmediate>(graphicsContext);

    // NOTE: Create audio engine.
    audioEngine = std::make_shared<AudioEngineAL>();
    if (auto err = audioEngine->initialize(); err != nullptr) {
        return errors::wrap(std::move(err), "AudioEngineAL::Initialize() failed.");
    }

    // NOTE: Create subsystems
    keyboard = std::make_shared<KeyboardCocoa>();
    mouse = std::make_shared<MouseCocoa>();

    // NOTE: Create gamepad
    gamepad = std::make_shared<GamepadIOKit>();
    if (auto err = gamepad->Initialize(eventQueue); err != nullptr) {
        return errors::wrap(std::move(err), "GamepadIOKit::Initialize() failed.");
    }

    // NOTE: Connect to system event signal
    POMDOG_ASSERT(eventQueue);
    systemEventConnection = eventQueue->Connect(
        [this](const SystemEvent& event) { processSystemEvents(event); });

    auto [resourceDir, resourceDirErr] = FileSystem::GetResourceDirectoryPath();
    if (resourceDirErr != nullptr) {
        return errors::wrap(std::move(resourceDirErr), "FileSystem::GetResourceDirectoryPath() failed.");
    }
    auto contentDirectory = PathHelper::Join(resourceDir, "content");

    // NOTE: Create asset manager.
    assetManager = std::make_unique<AssetManager>(
        std::move(contentDirectory),
        audioEngine,
        graphicsDevice);

    ioService_ = std::make_unique<IOService>();
    if (auto err = ioService_->initialize(clock_); err != nullptr) {
        return errors::wrap(std::move(err), "IOService::Initialize() failed.");
    }
    httpClient = std::make_unique<HTTPClient>(ioService_.get());

    POMDOG_ASSERT(presentationParameters.presentationInterval > 0);
    presentationInterval = Duration(1) / presentationParameters.presentationInterval;

    return nullptr;
}

GameHostMetal::Impl::~Impl()
{
    systemEventConnection.Disconnect();
    httpClient.reset();
    if (auto err = ioService_->shutdown(); err != nullptr) {
        Log::Warning("pomdog", err->toString());
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
GameHostMetal::Impl::initializeGame(
    const std::weak_ptr<Game>& weakGameIn,
    const std::function<void()>& onCompletedIn)
{
    POMDOG_ASSERT(!weakGameIn.expired());
    POMDOG_ASSERT(onCompletedIn);
    weakGame = weakGameIn;
    onCompleted = onCompletedIn;

    POMDOG_ASSERT(!weakGame.expired());
    auto game = weakGame.lock();

    if (auto err = game->initialize(); err != nullptr) {
        gameWillExit();
        return errors::wrap(std::move(err), "failed to initialize game");
    }

    if (exitRequest) {
        gameWillExit();
    }

    return nullptr;
}

bool GameHostMetal::Impl::isMetalSupported() const
{
    if (!graphicsDevice) {
        return false;
    }

    // NOTE: Get MTLDevice object.
    POMDOG_ASSERT(graphicsDevice != nullptr);
    id<MTLDevice> metalDevice = graphicsDevice->GetMTLDevice();

    return metalDevice != nullptr;
}

void GameHostMetal::Impl::gameWillExit()
{
    if (window) {
        window->setView(nullptr);
    }

    if (onCompleted) {
        dispatch_async(dispatch_get_main_queue(), [this] {
            onCompleted();
        });
    }
}

void GameHostMetal::Impl::exit()
{
    exitRequest = true;
    gameWillExit();
}

void GameHostMetal::Impl::gameLoop()
{
    POMDOG_ASSERT(graphicsContext != nullptr);

    if (exitRequest) {
        return;
    }

    graphicsContext->dispatchSemaphoreWait();
    frameCounter_->updateFrame();

    POMDOG_ASSERT(!exitRequest);
    POMDOG_ASSERT(!weakGame.expired());

    auto game = weakGame.lock();
    POMDOG_ASSERT(game);

    clock_->tick();
    doEvents();
    ioService_->step();

    if (exitRequest) {
        return;
    }

    game->update();

    if (!viewLiveResizing.load()) {
        renderFrame();
    }
}

void GameHostMetal::Impl::renderFrame()
{
    POMDOG_ASSERT(window);
    POMDOG_ASSERT(!weakGame.expired());

    bool skipRender = (!window || window->isMinimized() || [NSApp isHidden] == YES);

    if (skipRender) {
        return;
    }

    auto game = weakGame.lock();

    POMDOG_ASSERT(game);

    game->draw();
}

void GameHostMetal::Impl::doEvents()
{
    eventQueue->Emit();
}

void GameHostMetal::Impl::processSystemEvents(const SystemEvent& event)
{
    switch (event.kind) {
    case SystemEventKind::WindowShouldCloseEvent:
        Log::Internal("WindowShouldCloseEvent");
        exit();
        break;
    case SystemEventKind::WindowWillCloseEvent:
        Log::Internal("WindowWillCloseEvent");
        break;
    case SystemEventKind::ViewWillStartLiveResizeEvent: {
        auto rect = window->getClientBounds();
        Log::Internal(strings::format(
            "ViewWillStartLiveResizeEvent: {w: %d, h: %d}",
            rect.width, rect.height));
        break;
    }
    case SystemEventKind::ViewDidEndLiveResizeEvent: {
        auto rect = window->getClientBounds();
        Log::Internal(strings::format(
            "ViewDidEndLiveResizeEvent: {w: %d, h: %d}",
            rect.width, rect.height));

        clientSizeChanged();
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

void GameHostMetal::Impl::clientSizeChanged()
{
    POMDOG_ASSERT(graphicsDevice != nullptr);
    auto bounds = window->getClientBounds();

    graphicsDevice->clientSizeChanged(bounds.width, bounds.height);
    window->clientSizeChanged(bounds.width, bounds.height);
}

std::shared_ptr<GameWindow>
GameHostMetal::Impl::getWindow() noexcept
{
    return window;
}

std::shared_ptr<GameClock>
GameHostMetal::Impl::getClock() noexcept
{
    return clock_;
}

std::shared_ptr<gpu::GraphicsDevice>
GameHostMetal::Impl::getGraphicsDevice() noexcept
{
    return graphicsDevice;
}

std::shared_ptr<gpu::CommandQueue>
GameHostMetal::Impl::getCommandQueue() noexcept
{
    return graphicsCommandQueue;
}

std::shared_ptr<AudioEngine>
GameHostMetal::Impl::getAudioEngine() noexcept
{
    return audioEngine;
}

std::shared_ptr<AssetManager>
GameHostMetal::Impl::getAssetManager(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    std::shared_ptr<AssetManager> shared{gameHost, assetManager.get()};
    return shared;
}

std::shared_ptr<Keyboard>
GameHostMetal::Impl::getKeyboard() noexcept
{
    return keyboard;
}

std::shared_ptr<Mouse>
GameHostMetal::Impl::getMouse() noexcept
{
    return mouse;
}

std::shared_ptr<Gamepad>
GameHostMetal::Impl::getGamepad() noexcept
{
    return gamepad;
}

std::shared_ptr<IOService>
GameHostMetal::Impl::getIOService(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(ioService_ != nullptr);
    std::shared_ptr<IOService> shared{std::move(gameHost), ioService_.get()};
    return shared;
}

std::shared_ptr<HTTPClient>
GameHostMetal::Impl::getHTTPClient(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(httpClient != nullptr);
    std::shared_ptr<HTTPClient> shared(std::move(gameHost), httpClient.get());
    return shared;
}

// MARK: GameHostMetal

GameHostMetal::GameHostMetal()
    : impl_(std::make_unique<Impl>())
{
}

GameHostMetal::~GameHostMetal() = default;

std::unique_ptr<Error>
GameHostMetal::initialize(
    MTKView* metalView,
    const std::shared_ptr<GameWindowCocoa>& window,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
    const gpu::PresentationParameters& presentationParameters)
{
    POMDOG_ASSERT(impl_);
    return impl_->initialize(metalView, window, eventQueue, presentationParameters);
}

std::unique_ptr<Error>
GameHostMetal::initializeGame(
    const std::weak_ptr<Game>& game,
    const std::function<void()>& onCompleted)
{
    POMDOG_ASSERT(impl_);
    return impl_->initializeGame(game, onCompleted);
}

void GameHostMetal::gameLoop()
{
    POMDOG_ASSERT(impl_);
    impl_->gameLoop();
}

bool GameHostMetal::isMetalSupported() const noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->isMetalSupported();
}

void GameHostMetal::exit()
{
    POMDOG_ASSERT(impl_);
    impl_->exit();
}

std::shared_ptr<GameWindow> GameHostMetal::getWindow() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getWindow();
}

std::shared_ptr<GameClock> GameHostMetal::getClock() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getClock();
}

std::shared_ptr<gpu::GraphicsDevice> GameHostMetal::getGraphicsDevice() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getGraphicsDevice();
}

std::shared_ptr<gpu::CommandQueue> GameHostMetal::getCommandQueue() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getCommandQueue();
}

std::shared_ptr<AudioEngine> GameHostMetal::getAudioEngine() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getAudioEngine();
}

std::shared_ptr<AssetManager> GameHostMetal::getAssetManager() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getAssetManager(shared_from_this());
}

std::shared_ptr<Keyboard> GameHostMetal::getKeyboard() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getKeyboard();
}

std::shared_ptr<Mouse> GameHostMetal::getMouse() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getMouse();
}

std::shared_ptr<Gamepad> GameHostMetal::getGamepad() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getGamepad();
}

std::shared_ptr<IOService> GameHostMetal::getIOService() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getIOService(shared_from_this());
}

std::shared_ptr<HTTPClient> GameHostMetal::getHTTPClient() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getHTTPClient(shared_from_this());
}

} // namespace pomdog::detail::cocoa
