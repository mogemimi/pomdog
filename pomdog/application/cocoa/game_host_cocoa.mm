// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/cocoa/game_host_cocoa.h"
#include "pomdog/application/cocoa/game_window_cocoa.h"
#include "pomdog/application/cocoa/pomdog_opengl_view.h"
#include "pomdog/application/game.h"
#include "pomdog/application/system_events.h"
#include "pomdog/audio/openal/audio_engine_al.h"
#include "pomdog/chrono/apple/time_source_apple.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/gl4/graphics_context_gl4.h"
#include "pomdog/gpu/gl4/graphics_device_gl4.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/input/cocoa/keyboard_cocoa.h"
#include "pomdog/input/cocoa/mouse_cocoa.h"
#include "pomdog/input/iokit/gamepad_iokit.h"
#include "pomdog/logging/log.h"
#include "pomdog/network/http_client.h"
#include "pomdog/network/io_service.h"
#include "pomdog/platform/cocoa/opengl_context_cocoa.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"
#include "pomdog/utility/string_helper.h"
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

using pomdog::gpu::detail::gl4::GraphicsDeviceGL4;
using pomdog::gpu::detail::gl4::GraphicsContextGL4;
using pomdog::detail::IOKit::GamepadIOKit;
using pomdog::detail::openal::AudioEngineAL;

namespace pomdog::detail::cocoa {

// MARK: - GameHostCocoa::Impl

class GameHostCocoa::Impl final {
public:
    ~Impl();

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        PomdogOpenGLView* openGLView,
        const std::shared_ptr<GameWindowCocoa>& window,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const gpu::PresentationParameters& presentationParameters);

    [[nodiscard]] std::unique_ptr<Error>
    run(
        const std::weak_ptr<Game>& game,
        std::function<void()>&& onCompleted);

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
    void gameLoop();

    void renderFrame();

    void doEvents();

    void processSystemEvents(const SystemEvent& event);

    void clientSizeChanged();

    void gameWillExit();

    static CVReturn displayLinkCallback(
        CVDisplayLinkRef displayLink,
        const CVTimeStamp* now,
        const CVTimeStamp* outputTime,
        CVOptionFlags flagsIn,
        CVOptionFlags* flagsOut,
        void* displayLinkContext);

private:
    ScopedConnection systemEventConnection;
    std::mutex renderMutex;
    std::atomic_bool viewLiveResizing = false;
    CVDisplayLinkRef displayLink = nullptr;
    std::function<void()> onCompleted;

    std::weak_ptr<Game> weakGame;
    std::shared_ptr<detail::apple::TimeSourceApple> timeSource_;
    std::shared_ptr<GameClockImpl> clock_;
    std::shared_ptr<EventQueue<SystemEvent>> eventQueue;
    std::shared_ptr<GameWindowCocoa> window;
    std::shared_ptr<OpenGLContextCocoa> openGLContext;
    std::shared_ptr<GraphicsDeviceGL4> graphicsDevice;
    std::shared_ptr<GraphicsContextGL4> graphicsContext;
    std::shared_ptr<gpu::CommandQueue> graphicsCommandQueue;
    std::shared_ptr<AudioEngineAL> audioEngine;
    std::unique_ptr<AssetManager> assetManager;
    std::shared_ptr<KeyboardCocoa> keyboard;
    std::shared_ptr<MouseCocoa> mouse;
    std::shared_ptr<GamepadIOKit> gamepad;

    std::unique_ptr<IOService> ioService_;
    std::unique_ptr<HTTPClient> httpClient;

    __weak PomdogOpenGLView* openGLView = nullptr;
    Duration presentationInterval = Duration::zero();
    bool exitRequest = false;
    bool displayLinkEnabled = true;
};

std::unique_ptr<Error>
GameHostCocoa::Impl::initialize(
    PomdogOpenGLView* openGLViewIn,
    const std::shared_ptr<GameWindowCocoa>& windowIn,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueueIn,
    const gpu::PresentationParameters& presentationParameters)
{
    this->viewLiveResizing = false;
    this->displayLink = nullptr;
    this->eventQueue = eventQueueIn;
    this->window = windowIn;
    this->openGLView = openGLViewIn;
    this->exitRequest = false;
    this->displayLinkEnabled = true;

    if (presentationParameters.presentationInterval <= 0) {
        return errors::make("PresentationInterval must be > 0.");
    }
    POMDOG_ASSERT(presentationParameters.presentationInterval > 0);
    presentationInterval = Duration(1) / presentationParameters.presentationInterval;

    timeSource_ = std::make_shared<detail::apple::TimeSourceApple>();
    clock_ = std::make_shared<GameClockImpl>();
    if (auto err = clock_->Initialize(presentationParameters.presentationInterval, timeSource_); err != nullptr) {
        return errors::wrap(std::move(err), "GameClockImpl::Initialize() failed.");
    }

    POMDOG_ASSERT(window);
    window->setView(openGLView);

    // NOTE: Create OpenGL context.
    openGLContext = std::make_shared<OpenGLContextCocoa>();
    if (auto err = openGLContext->initialize(presentationParameters); err != nullptr) {
        return errors::wrap(std::move(err), "OpenGLContextCocoa::Initialize() failed.");
    }

    POMDOG_ASSERT(openGLView != nullptr);
    [openGLView setEventQueue:eventQueue];
    [openGLView setOpenGLContext:openGLContext];

    // Create graphics device and device resources
    openGLContext->lock();
    openGLContext->setView(openGLView);
    openGLContext->makeCurrent();

    // NOTE: Create a graphics device.
    graphicsDevice = std::make_shared<GraphicsDeviceGL4>();
    if (auto err = graphicsDevice->Initialize(presentationParameters); err != nullptr) {
        return errors::wrap(std::move(err), "GraphicsDeviceGL4::Initialize() failed.");
    }

    // NOTE: Create a graphics context.
    graphicsContext = std::make_shared<GraphicsContextGL4>();
    if (auto err = graphicsContext->Initialize(openGLContext, graphicsDevice); err != nullptr) {
        return errors::wrap(std::move(err), "GraphicsContextGL4::Initialize() failed.");
    }

    graphicsCommandQueue = std::make_shared<gpu::detail::CommandQueueImmediate>(graphicsContext);
    openGLContext->unlock();

    // NOTE: Create audio engine.
    audioEngine = std::make_shared<AudioEngineAL>();
    if (auto err = audioEngine->Initialize(); err != nullptr) {
        return errors::wrap(std::move(err), "AudioEngineAL::Initialize() failed.");
    }

    // Create subsystems
    keyboard = std::make_shared<KeyboardCocoa>();
    mouse = std::make_shared<MouseCocoa>();

    // NOTE: Create gamepad
    gamepad = std::make_shared<GamepadIOKit>();
    if (auto err = gamepad->Initialize(eventQueue); err != nullptr) {
        return errors::wrap(std::move(err), "GamepadIOKit::Initialize() failed.");
    }

    // Connect to system event signal
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
    if (auto err = ioService_->Initialize(clock_); err != nullptr) {
        return errors::wrap(std::move(err), "IOService::Initialize() failed.");
    }
    httpClient = std::make_unique<HTTPClient>(ioService_.get());

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &displayLinkCallback, this);

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
    if (auto err = ioService_->Shutdown(); err != nullptr) {
        Log::Warning("pomdog", err->toString());
    }
    ioService_.reset();
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
GameHostCocoa::Impl::run(
    const std::weak_ptr<Game>& weakGameIn,
    std::function<void()>&& onCompletedIn)
{
    POMDOG_ASSERT(!weakGameIn.expired());
    POMDOG_ASSERT(onCompletedIn);
    weakGame = weakGameIn;
    onCompleted = std::move(onCompletedIn);

    POMDOG_ASSERT(!weakGame.expired());
    auto game = weakGame.lock();

    openGLContext->lock();
    openGLContext->setView(openGLView);
    openGLContext->makeCurrent();

    if (auto err = game->initialize(); err != nullptr) {
        openGLContext->unlock();
        gameWillExit();
        return errors::wrap(std::move(err), "failed to initialzie game");
    }

    openGLContext->unlock();

    if (exitRequest) {
        gameWillExit();
        return nullptr;
    }

    POMDOG_ASSERT(openGLView != nullptr);

    auto nsOpenGLContext = openGLContext->getNativeOpenGLContext();
    NSOpenGLPixelFormat* nsPixelFormat = [nsOpenGLContext pixelFormat];
    CGLContextObj cglContext = [nsOpenGLContext CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [nsPixelFormat CGLPixelFormatObj];

    [openGLView setResizingCallback:[this](bool resizing) {
        viewLiveResizing = resizing;
    }];

    [openGLView setRenderCallback:[this] {
        std::lock_guard<std::mutex> lock(renderMutex);
        clientSizeChanged();

        // NOTE: In order to prevent the RenderFrame() function from being
        // executed before the Game::Update() function is called, if the frame
        // number is <= 0, do not render.
        if (clock_->GetFrameNumber() > 0) {
            renderFrame();
        }
    }];

    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(
        displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);

    return nullptr;
}

void GameHostCocoa::Impl::gameWillExit()
{
    if (openGLView != nullptr) {
        [openGLView setRenderCallback:[] {}];
    }

    if (window) {
        window->setView(nullptr);
    }

    if (onCompleted) {
        dispatch_async(dispatch_get_main_queue(), [this] {
            onCompleted();
        });
    }
}

void GameHostCocoa::Impl::exit()
{
    exitRequest = true;

    if (displayLinkEnabled) {
        if (displayLink != nullptr) {
            dispatch_async(dispatch_get_main_queue(), [this] {
                CVDisplayLinkStop(displayLink);
            });
        }
        gameWillExit();
    }
}

CVReturn
GameHostCocoa::Impl::displayLinkCallback(
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
        gameHost->gameLoop();
    });
    return kCVReturnSuccess;
}

void GameHostCocoa::Impl::gameLoop()
{
    POMDOG_ASSERT(!exitRequest);
    POMDOG_ASSERT(!weakGame.expired());

    std::lock_guard<std::mutex> lock(renderMutex);

    auto game = weakGame.lock();
    POMDOG_ASSERT(game);

    clock_->Tick();
    doEvents();
    ioService_->Step();

    if (exitRequest) {
        return;
    }

    openGLContext->lock();
    openGLContext->setView(openGLView);
    openGLContext->makeCurrent();
    game->update();
    openGLContext->unlock();

    if (!viewLiveResizing.load()) {
        renderFrame();
    }

    if (!displayLinkEnabled) {
        auto elapsedTime = clock_->GetElapsedTime();

        if (elapsedTime < presentationInterval) {
            lock.~lock_guard();
            auto sleepTime = (presentationInterval - elapsedTime);
            std::this_thread::sleep_for(sleepTime);
        }
    }
}

void GameHostCocoa::Impl::renderFrame()
{
    POMDOG_ASSERT(window);
    POMDOG_ASSERT(!weakGame.expired());

    bool skipRender = (!window || window->isMinimized() || [NSApp isHidden] == YES);

    if (skipRender) {
        return;
    }

    auto game = weakGame.lock();

    POMDOG_ASSERT(game);
    POMDOG_ASSERT(openGLView != nullptr);

    openGLContext->lock();
    openGLContext->setView(openGLView);
    openGLContext->makeCurrent();

    game->draw();

    openGLContext->unlock();
}

void GameHostCocoa::Impl::doEvents()
{
    eventQueue->Emit();
}

void GameHostCocoa::Impl::processSystemEvents(const SystemEvent& event)
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
        Log::Internal(StringHelper::Format(
            "ViewWillStartLiveResizeEvent: {w: %d, h: %d}",
            rect.width, rect.height));
        break;
    }
    case SystemEventKind::ViewDidEndLiveResizeEvent: {
        auto rect = window->getClientBounds();
        Log::Internal(StringHelper::Format(
            "ViewDidEndLiveResizeEvent: {w: %d, h: %d}",
            rect.width, rect.height));
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

void GameHostCocoa::Impl::clientSizeChanged()
{
    openGLContext->lock();
    openGLContext->makeCurrent();

    auto nativeContext = openGLContext->getNativeOpenGLContext();
    POMDOG_ASSERT(nativeContext != nullptr);
    [nativeContext update];

    POMDOG_ASSERT(graphicsDevice != nullptr);

    auto bounds = window->getClientBounds();

    graphicsDevice->clientSizeChanged(bounds.width, bounds.height);
    window->clientSizeChanged(bounds.width, bounds.height);

    openGLContext->unlock();
}

std::shared_ptr<GameWindow>
GameHostCocoa::Impl::getWindow() noexcept
{
    return window;
}

std::shared_ptr<GameClock>
GameHostCocoa::Impl::getClock() noexcept
{
    return clock_;
}

std::shared_ptr<gpu::GraphicsDevice>
GameHostCocoa::Impl::getGraphicsDevice() noexcept
{
    return graphicsDevice;
}

std::shared_ptr<gpu::CommandQueue>
GameHostCocoa::Impl::getCommandQueue() noexcept
{
    return graphicsCommandQueue;
}

std::shared_ptr<AudioEngine>
GameHostCocoa::Impl::getAudioEngine() noexcept
{
    return audioEngine;
}

std::shared_ptr<AssetManager>
GameHostCocoa::Impl::getAssetManager(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    std::shared_ptr<AssetManager> shared{gameHost, assetManager.get()};
    return shared;
}

std::shared_ptr<Keyboard>
GameHostCocoa::Impl::getKeyboard() noexcept
{
    return keyboard;
}

std::shared_ptr<Mouse>
GameHostCocoa::Impl::getMouse() noexcept
{
    return mouse;
}

std::shared_ptr<Gamepad>
GameHostCocoa::Impl::getGamepad() noexcept
{
    return gamepad;
}

std::shared_ptr<IOService>
GameHostCocoa::Impl::getIOService(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(ioService_ != nullptr);
    std::shared_ptr<IOService> shared{std::move(gameHost), ioService_.get()};
    return shared;
}

std::shared_ptr<HTTPClient>
GameHostCocoa::Impl::getHTTPClient(std::shared_ptr<GameHost>&& gameHost) noexcept
{
    POMDOG_ASSERT(httpClient != nullptr);
    std::shared_ptr<HTTPClient> shared(std::move(gameHost), httpClient.get());
    return shared;
}

// MARK: - GameHostCocoa

GameHostCocoa::GameHostCocoa()
    : impl_(std::make_unique<Impl>())
{
}

GameHostCocoa::~GameHostCocoa() = default;

std::unique_ptr<Error>
GameHostCocoa::initialize(
    PomdogOpenGLView* openGLView,
    const std::shared_ptr<GameWindowCocoa>& window,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
    const gpu::PresentationParameters& presentationParameters)
{
    POMDOG_ASSERT(impl_);
    return impl_->initialize(openGLView, window, eventQueue, presentationParameters);
}

std::unique_ptr<Error>
GameHostCocoa::run(
    const std::weak_ptr<Game>& game,
    std::function<void()>&& onCompleted)
{
    POMDOG_ASSERT(impl_);
    return impl_->run(game, std::move(onCompleted));
}

void GameHostCocoa::exit()
{
    POMDOG_ASSERT(impl_);
    impl_->exit();
}

std::shared_ptr<GameWindow> GameHostCocoa::getWindow() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getWindow();
}

std::shared_ptr<GameClock> GameHostCocoa::getClock() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getClock();
}

std::shared_ptr<gpu::GraphicsDevice> GameHostCocoa::getGraphicsDevice() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getGraphicsDevice();
}

std::shared_ptr<gpu::CommandQueue> GameHostCocoa::getCommandQueue() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getCommandQueue();
}

std::shared_ptr<AudioEngine> GameHostCocoa::getAudioEngine() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getAudioEngine();
}

std::shared_ptr<AssetManager> GameHostCocoa::getAssetManager() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getAssetManager(shared_from_this());
}

std::shared_ptr<Keyboard> GameHostCocoa::getKeyboard() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getKeyboard();
}

std::shared_ptr<Mouse> GameHostCocoa::getMouse() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getMouse();
}

std::shared_ptr<Gamepad> GameHostCocoa::getGamepad() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getGamepad();
}

std::shared_ptr<IOService> GameHostCocoa::getIOService() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getIOService(shared_from_this());
}

std::shared_ptr<HTTPClient> GameHostCocoa::getHTTPClient() noexcept
{
    POMDOG_ASSERT(impl_);
    return impl_->getHTTPClient(shared_from_this());
}

} // namespace pomdog::detail::cocoa
