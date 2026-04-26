// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/cocoa/game_host_cocoa.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/application/cocoa/game_window_cocoa.h"
#include "pomdog/application/cocoa/opengl_context_cocoa.h"
#include "pomdog/application/cocoa/pomdog_opengl_view.h"
#include "pomdog/application/game.h"
#include "pomdog/application/game_host_options.h"
#include "pomdog/audio/openal/audio_engine_al.h"
#include "pomdog/chrono/apple/time_source_apple.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/gl4/graphics_context_gl4.h"
#include "pomdog/gpu/gl4/graphics_device_gl4.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/input/backends/keyboard_impl.h"
#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/cocoa/keyboard_cocoa.h"
#include "pomdog/input/cocoa/mouse_cocoa.h"
#include "pomdog/input/gamepad_service.h"
#include "pomdog/input/iokit/gamepad_iokit.h"
#include "pomdog/input/player_index.h"
#include "pomdog/logging/log.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/network/http_client.h"
#include "pomdog/network/io_service.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <crt_externs.h>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::IOKit::GamepadServiceIOKit;
using pomdog::detail::openal::AudioEngineAL;
using pomdog::gpu::detail::gl4::GraphicsContextGL4;
using pomdog::gpu::detail::gl4::GraphicsDeviceGL4;

namespace pomdog::detail::cocoa {
namespace {

class GameHostCocoaImpl final : public GameHostCocoa {
private:
    std::mutex renderMutex_;
    std::atomic_bool viewLiveResizing_ = false;
    CVDisplayLinkRef displayLink_ = nullptr;
    std::function<void()> onCompleted_;

    std::weak_ptr<Game> weakGame_;
    std::shared_ptr<detail::apple::TimeSourceApple> timeSource_;
    std::shared_ptr<GameClockImpl> clock_;
    std::shared_ptr<SystemEventQueue> eventQueue_;
    std::shared_ptr<GameWindowCocoa> window_;
    std::shared_ptr<OpenGLContextCocoa> openGLContext_;
    std::shared_ptr<GraphicsDeviceGL4> graphicsDevice_;
    std::shared_ptr<GraphicsContextGL4> graphicsContext_;
    std::shared_ptr<gpu::CommandQueue> graphicsCommandQueue_;
    std::shared_ptr<AudioEngineAL> audioEngine_;
    std::shared_ptr<KeyboardImpl> keyboardImpl_;
    std::unique_ptr<KeyboardCocoa> keyboard_;
    std::shared_ptr<MouseImpl> mouseImpl_;
    std::unique_ptr<MouseCocoa> mouse_;
    std::shared_ptr<GamepadServiceIOKit> gamepad_;

    std::unique_ptr<IOService> ioService_;
    std::unique_ptr<HTTPClient> httpClient_;

    __weak PomdogOpenGLView* openGLView_ = nullptr;
    Duration presentationInterval_ = Duration::zero();
    bool exitRequest_ = false;
    bool displayLinkEnabled_ = true;

public:
    ~GameHostCocoaImpl() override
    {
        if (displayLink_ != nullptr) {
            CVDisplayLinkRelease(displayLink_);
            displayLink_ = nullptr;
        }

        if (openGLView_ != nullptr) {
            [openGLView_ setEventQueue:{}];
        }

        httpClient_.reset();
        if (ioService_ != nullptr) {
            if (auto err = ioService_->shutdown(); err != nullptr) {
                Log::Warning("pomdog", err->toString());
            }
        }
        ioService_.reset();
        gamepad_.reset();
        keyboard_.reset();
        keyboardImpl_.reset();
        mouse_.reset();
        mouseImpl_.reset();
        audioEngine_.reset();
        graphicsCommandQueue_.reset();
        graphicsContext_.reset();
        graphicsDevice_.reset();
        openGLContext_.reset();
        window_.reset();
        eventQueue_.reset();
        openGLView_ = nullptr;
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        PomdogOpenGLView* openGLViewIn,
        const std::shared_ptr<GameWindowCocoa>& windowIn,
        const std::shared_ptr<SystemEventQueue>& eventQueueIn,
        const gpu::PresentationParameters& presentationParameters,
        const GameHostOptions& options)
    {
        viewLiveResizing_ = false;
        displayLink_ = nullptr;
        eventQueue_ = eventQueueIn;
        window_ = windowIn;
        openGLView_ = openGLViewIn;
        exitRequest_ = false;
        displayLinkEnabled_ = true;

        if (presentationParameters.presentationInterval <= 0) {
            return errors::make("PresentationInterval must be > 0.");
        }
        POMDOG_ASSERT(presentationParameters.presentationInterval > 0);
        presentationInterval_ = Duration(1) / presentationParameters.presentationInterval;

        timeSource_ = std::make_shared<detail::apple::TimeSourceApple>();
        clock_ = std::make_shared<GameClockImpl>();
        if (auto err = clock_->initialize(presentationParameters.presentationInterval, timeSource_); err != nullptr) {
            return errors::wrap(std::move(err), "GameClockImpl::initialize() failed.");
        }

        POMDOG_ASSERT(window_);
        window_->setView(openGLView_);

        // NOTE: Create OpenGL context.
        auto [glContext, glErr] = OpenGLContextCocoa::create(presentationParameters);
        if (glErr != nullptr) {
            return errors::wrap(std::move(glErr), "OpenGLContextCocoa::create() failed.");
        }
        openGLContext_ = std::move(glContext);

        POMDOG_ASSERT(openGLView_ != nullptr);
        [openGLView_ setEventQueue:eventQueue_];
        [openGLView_ setOpenGLContext:openGLContext_];

        // Create graphics device and device resources
        openGLContext_->lock();
        openGLContext_->setView(openGLView_);
        openGLContext_->makeCurrent();

        // NOTE: Create a graphics device.
        graphicsDevice_ = std::make_shared<GraphicsDeviceGL4>();
        if (auto err = graphicsDevice_->initialize(presentationParameters); err != nullptr) {
            return errors::wrap(std::move(err), "GraphicsDeviceGL4::initialize() failed.");
        }

        // NOTE: Create a graphics context.
        graphicsContext_ = std::make_shared<GraphicsContextGL4>();
        if (auto err = graphicsContext_->initialize(openGLContext_, graphicsDevice_); err != nullptr) {
            return errors::wrap(std::move(err), "GraphicsContextGL4::initialize() failed.");
        }

        graphicsCommandQueue_ = std::make_shared<gpu::detail::CommandQueueImmediate>(graphicsContext_);
        openGLContext_->unlock();

        // NOTE: Create audio engine (conditional).
        if (options.enableAudio) {
            audioEngine_ = std::make_shared<AudioEngineAL>();
            if (auto err = audioEngine_->initialize(); err != nullptr) {
                return errors::wrap(std::move(err), "AudioEngineAL::initialize() failed.");
            }
        }

        // Create subsystems (conditional).
        if (!options.headless) {
            keyboardImpl_ = std::make_shared<KeyboardImpl>();
            keyboard_ = std::make_unique<KeyboardCocoa>(keyboardImpl_);
            mouseImpl_ = std::make_shared<MouseImpl>();
            mouse_ = std::make_unique<MouseCocoa>(mouseImpl_);
        }

        // NOTE: Create gamepad (conditional).
        if (options.enableGamepad && !options.headless) {
            gamepad_ = std::make_shared<GamepadServiceIOKit>();
            if (auto err = gamepad_->initialize(options.gameControllerDB); err != nullptr) {
                return errors::wrap(std::move(err), "GamepadServiceIOKit::initialize() failed.");
            }
        }

        // Connect to system event signal
        POMDOG_ASSERT(eventQueue_ != nullptr);

        // NOTE: Create IO service and HTTP client (conditional).
        if (options.enableNetwork) {
            ioService_ = std::make_unique<IOService>();
            if (auto err = ioService_->initialize(clock_); err != nullptr) {
                return errors::wrap(std::move(err), "IOService::initialize() failed.");
            }
            httpClient_ = std::make_unique<HTTPClient>(ioService_.get());
        }

        CVDisplayLinkCreateWithActiveCGDisplays(&displayLink_);
        CVDisplayLinkSetOutputCallback(displayLink_, &displayLinkCallback, this);

        return nullptr;
    }

    std::unique_ptr<Error>
    run(
        const std::weak_ptr<Game>& weakGameIn,
        std::function<void()>&& onCompletedIn) override
    {
        POMDOG_ASSERT(!weakGameIn.expired());
        POMDOG_ASSERT(onCompletedIn);
        weakGame_ = weakGameIn;
        onCompleted_ = std::move(onCompletedIn);

        POMDOG_ASSERT(!weakGame_.expired());
        auto game = weakGame_.lock();

        openGLContext_->lock();
        openGLContext_->setView(openGLView_);
        openGLContext_->makeCurrent();

        if (auto err = game->initialize(shared_from_this()); err != nullptr) {
            openGLContext_->unlock();
            gameWillExit();
            return errors::wrap(std::move(err), "failed to initialize game");
        }

        openGLContext_->unlock();

        if (exitRequest_) {
            gameWillExit();
            return nullptr;
        }

        POMDOG_ASSERT(openGLView_ != nullptr);

        auto nsOpenGLContext = openGLContext_->getNativeOpenGLContext();
        NSOpenGLPixelFormat* nsPixelFormat = [nsOpenGLContext pixelFormat];
        CGLContextObj cglContext = [nsOpenGLContext CGLContextObj];
        CGLPixelFormatObj cglPixelFormat = [nsPixelFormat CGLPixelFormatObj];

        [openGLView_ setResizingCallback:[this](bool resizing) {
            viewLiveResizing_ = resizing;
        }];

        [openGLView_ setRenderCallback:[this] {
            std::lock_guard<std::mutex> lock(renderMutex_);
            clientSizeChanged();

            // NOTE: In order to prevent the RenderFrame() function from being
            // executed before the Game::Update() function is called, if the frame
            // number is <= 0, do not render.
            if (clock_->getFrameNumber() > 0) {
                renderFrame();
            }
        }];

        CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(
            displayLink_, cglContext, cglPixelFormat);
        CVDisplayLinkStart(displayLink_);

        return nullptr;
    }

    void exit() override
    {
        exitRequest_ = true;

        if (displayLinkEnabled_) {
            if (displayLink_ != nullptr) {
                dispatch_async(dispatch_get_main_queue(), [this] {
                    CVDisplayLinkStop(displayLink_);
                });
            }
            gameWillExit();
        }
    }

    std::shared_ptr<GameWindow>
    getWindow() noexcept override
    {
        return window_;
    }

    std::shared_ptr<GameClock>
    getClock() noexcept override
    {
        return clock_;
    }

    std::shared_ptr<gpu::GraphicsDevice>
    getGraphicsDevice() noexcept override
    {
        return graphicsDevice_;
    }

    std::shared_ptr<gpu::CommandQueue>
    getCommandQueue() noexcept override
    {
        return graphicsCommandQueue_;
    }

    std::shared_ptr<AudioEngine>
    getAudioEngine() noexcept override
    {
        return audioEngine_;
    }

    std::shared_ptr<Keyboard>
    getKeyboard() noexcept override
    {
        return keyboardImpl_;
    }

    std::shared_ptr<Mouse>
    getMouse() noexcept override
    {
        return mouseImpl_;
    }

    std::shared_ptr<Gamepad>
    getGamepad() noexcept override
    {
        return gamepad_->getGamepad(PlayerIndex::One);
    }

    std::shared_ptr<GamepadService>
    getGamepadService() noexcept override
    {
        return gamepad_;
    }

    std::shared_ptr<Touchscreen>
    getTouchscreen() noexcept override
    {
        return nullptr;
    }

    std::shared_ptr<IOService>
    getIOService() noexcept override
    {
        if (ioService_ == nullptr) {
            return nullptr;
        }
        std::shared_ptr<IOService> shared{shared_from_this(), ioService_.get()};
        return shared;
    }

    std::shared_ptr<HTTPClient>
    getHTTPClient() noexcept override
    {
        if (httpClient_ == nullptr) {
            return nullptr;
        }
        std::shared_ptr<HTTPClient> shared(shared_from_this(), httpClient_.get());
        return shared;
    }

private:
    void gameLoop()
    {
        POMDOG_ASSERT(!exitRequest_);
        POMDOG_ASSERT(!weakGame_.expired());

        std::lock_guard<std::mutex> lock(renderMutex_);

        auto game = weakGame_.lock();
        POMDOG_ASSERT(game);

        clock_->tick();
        if (keyboardImpl_) {
            keyboardImpl_->clearTextInput();
        }
        if (mouseImpl_) {
            mouseImpl_->clearScrollDelta();
        }
        doEvents();
        if (gamepad_) {
            gamepad_->pollEvents();
        }
        if (audioEngine_) {
            audioEngine_->makeCurrentContext();
            audioEngine_->update();
        }
        if (ioService_) {
            ioService_->step();
        }

        if (exitRequest_) {
            return;
        }

        openGLContext_->lock();
        openGLContext_->setView(openGLView_);
        openGLContext_->makeCurrent();
        game->update();
        openGLContext_->unlock();

        if (!viewLiveResizing_.load()) {
            renderFrame();
        }

        if (!displayLinkEnabled_) {
            auto elapsedTime = clock_->getElapsedTime();

            if (elapsedTime < presentationInterval_) {
                lock.~lock_guard();
                auto sleepTime = (presentationInterval_ - elapsedTime);
                std::this_thread::sleep_for(sleepTime);
            }
        }
    }

    void renderFrame()
    {
        POMDOG_ASSERT(window_);
        POMDOG_ASSERT(!weakGame_.expired());

        bool skipRender = (!window_ || window_->isMinimized() || [NSApp isHidden] == YES);

        if (skipRender) {
            return;
        }

        auto game = weakGame_.lock();

        POMDOG_ASSERT(game);
        POMDOG_ASSERT(openGLView_ != nullptr);

        openGLContext_->lock();
        openGLContext_->setView(openGLView_);
        openGLContext_->makeCurrent();

        game->draw();

        openGLContext_->unlock();
    }

    void doEvents()
    {
        eventQueue_->emit([this](SystemEvent event) {
            processSystemEvents(std::move(event));
        });
    }

    void processSystemEvents(const SystemEvent& event)
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
            auto rect = window_->getClientBounds();
            Log::Internal(pomdog::format(
                "ViewWillStartLiveResizeEvent: w={}, h={}",
                rect.width, rect.height));
            break;
        }
        case SystemEventKind::ViewDidEndLiveResizeEvent: {
            auto rect = window_->getClientBounds();
            Log::Internal(pomdog::format(
                "ViewDidEndLiveResizeEvent: w={}, h={}",
                rect.width, rect.height));
            break;
        }
        default:
            if (keyboard_) {
                keyboard_->handleEvent(event);
            }
            if (mouse_) {
                mouse_->handleEvent(event);
            }
            break;
        }
    }

    void clientSizeChanged()
    {
        openGLContext_->lock();
        openGLContext_->makeCurrent();

        auto nativeContext = openGLContext_->getNativeOpenGLContext();
        POMDOG_ASSERT(nativeContext != nullptr);
        [nativeContext update];

        POMDOG_ASSERT(graphicsDevice_ != nullptr);

        auto bounds = window_->getClientBounds();

        graphicsDevice_->clientSizeChanged(bounds.width, bounds.height);
        window_->clientSizeChanged(bounds.width, bounds.height);

        openGLContext_->unlock();
    }

    void gameWillExit()
    {
        if (openGLView_ != nullptr) {
            [openGLView_ setRenderCallback:[] {}];
        }

        if (window_) {
            window_->setView(nullptr);
        }

        if (onCompleted_) {
            dispatch_async(dispatch_get_main_queue(), [this] {
                onCompleted_();
            });
        }
    }

    static CVReturn displayLinkCallback(
        [[maybe_unused]] CVDisplayLinkRef displayLink,
        [[maybe_unused]] const CVTimeStamp* now,
        [[maybe_unused]] const CVTimeStamp* outputTime,
        [[maybe_unused]] CVOptionFlags flagsIn,
        [[maybe_unused]] CVOptionFlags* flagsOut,
        void* displayLinkContext)
    {
        dispatch_sync(dispatch_get_main_queue(), ^{
            auto gameHost = reinterpret_cast<GameHostCocoaImpl*>(displayLinkContext);
            POMDOG_ASSERT(gameHost != nullptr);
            gameHost->gameLoop();
        });
        return kCVReturnSuccess;
    }
};

} // namespace

GameHostCocoa::GameHostCocoa() = default;

GameHostCocoa::~GameHostCocoa() = default;

std::tuple<std::shared_ptr<GameHostCocoa>, std::unique_ptr<Error>>
GameHostCocoa::create(
    PomdogOpenGLView* openGLView,
    const std::shared_ptr<GameWindowCocoa>& window,
    const std::shared_ptr<SystemEventQueue>& eventQueue,
    const gpu::PresentationParameters& presentationParameters,
    const GameHostOptions& options)
{
    auto host = std::make_shared<GameHostCocoaImpl>();
    if (auto err = host->initialize(openGLView, window, eventQueue, presentationParameters, options); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(host), nullptr);
}

} // namespace pomdog::detail::cocoa
