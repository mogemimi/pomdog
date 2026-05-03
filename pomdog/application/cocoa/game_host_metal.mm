// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/cocoa/game_host_metal.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/application/cocoa/game_window_cocoa.h"
#include "pomdog/application/game.h"
#include "pomdog/application/game_host_options.h"
#include "pomdog/audio/openal/audio_engine_al.h"
#include "pomdog/chrono/apple/time_source_apple.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
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
#include "pomdog/input/backends/keyboard_impl.h"
#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/cocoa/keyboard_cocoa.h"
#include "pomdog/input/cocoa/mouse_cocoa.h"
#include "pomdog/input/gamepad_service.h"
#include "pomdog/input/iokit/gamepad_iokit.h"
#include "pomdog/input/key_state.h"
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
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include <crt_externs.h>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::IOKit::GamepadServiceIOKit;
using pomdog::detail::openal::AudioEngineAL;
using pomdog::gpu::detail::metal::FrameCounter;
using pomdog::gpu::detail::metal::GraphicsContextMetal;
using pomdog::gpu::detail::metal::GraphicsDeviceMetal;
using pomdog::gpu::detail::metal::ToPixelFormat;

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

class GameHostMetalImpl final : public GameHostMetal {
private:
    std::atomic_bool viewLiveResizing_ = false;
    std::function<void()> onCompleted_;

    std::weak_ptr<Game> weakGame_;
    std::shared_ptr<detail::apple::TimeSourceApple> timeSource_;
    std::shared_ptr<GameClockImpl> clock_;
    std::shared_ptr<SystemEventQueue> eventQueue_;
    std::shared_ptr<GameWindowCocoa> window_;
    std::shared_ptr<GraphicsDeviceMetal> graphicsDevice_;
    std::shared_ptr<GraphicsContextMetal> graphicsContext_;
    std::shared_ptr<gpu::CommandQueue> graphicsCommandQueue_;
    std::shared_ptr<FrameCounter> frameCounter_;
    std::shared_ptr<AudioEngineAL> audioEngine_;
    std::shared_ptr<KeyboardImpl> keyboardImpl_;
    std::unique_ptr<KeyboardCocoa> keyboard_;
    std::shared_ptr<MouseImpl> mouseImpl_;
    std::unique_ptr<MouseCocoa> mouse_;
    std::shared_ptr<GamepadServiceIOKit> gamepad_;

    std::unique_ptr<IOService> ioService_;
    std::unique_ptr<HTTPClient> httpClient_;

    __weak MTKView* metalView_ = nullptr;
    Duration presentationInterval_ = Duration::zero();
    std::optional<bool> pendingDisplaySync_;
    Rect2D lastReportedBounds_ = {0, 0, 0, 0};
    bool exitRequest_ = false;
    bool displaySyncEnabled_ = true;

public:
    ~GameHostMetalImpl() override
    {
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
        frameCounter_.reset();
        graphicsCommandQueue_.reset();
        graphicsContext_.reset();
        graphicsDevice_.reset();
        window_.reset();
        eventQueue_.reset();
        metalView_ = nullptr;
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        MTKView* metalViewIn,
        const std::shared_ptr<GameWindowCocoa>& windowIn,
        const std::shared_ptr<SystemEventQueue>& eventQueueIn,
        const gpu::PresentationParameters& presentationParameters,
        const GameHostOptions& options)
    {
        viewLiveResizing_ = false;
        eventQueue_ = eventQueueIn;
        window_ = windowIn;
        metalView_ = metalViewIn;
        presentationInterval_ = Duration(1) / 60;
        exitRequest_ = false;

        POMDOG_ASSERT(window_ != nullptr);
        POMDOG_ASSERT(metalView_ != nullptr);

        timeSource_ = std::make_shared<detail::apple::TimeSourceApple>();
        clock_ = std::make_shared<GameClockImpl>();
        if (auto err = clock_->initialize(presentationParameters.presentationInterval, timeSource_); err != nullptr) {
            return errors::wrap(std::move(err), "GameClockImpl::initialize() failed.");
        }

        window_->setView(metalView_);

        frameCounter_ = std::make_shared<FrameCounter>();

        // NOTE: Create graphics device
        graphicsDevice_ = std::make_shared<GraphicsDeviceMetal>();
        if (auto err = graphicsDevice_->initialize(presentationParameters, frameCounter_); err != nullptr) {
            return errors::wrap(std::move(err), "GraphicsDeviceMetal::initialize() failed.");
        }

        // NOTE: Get MTLDevice object.
        POMDOG_ASSERT(graphicsDevice_ != nullptr);
        id<MTLDevice> metalDevice = graphicsDevice_->getMTLDevice();

        if (metalDevice == nullptr) {
            return errors::make("Metal is not supported on this device.");
        }

        // NOTE: Setup metal view
        SetupMetalView(metalView_, metalDevice, presentationParameters);

        POMDOG_ASSERT(metalDevice != nullptr);

        // NOTE: Create graphics context
        graphicsContext_ = std::make_shared<GraphicsContextMetal>(metalDevice);
        graphicsContext_->setMTKView(metalView_);

        // NOTE: Create graphics command queue
        graphicsCommandQueue_ = std::make_shared<gpu::detail::CommandQueueImmediate>(graphicsContext_);

        // NOTE: Create audio engine (conditional).
        if (options.enableAudio) {
            audioEngine_ = std::make_shared<AudioEngineAL>();
            if (auto err = audioEngine_->initialize(); err != nullptr) {
                return errors::wrap(std::move(err), "AudioEngineAL::initialize() failed.");
            }
        }

        // NOTE: Create subsystems (conditional).
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

        // NOTE: Connect to system event signal
        POMDOG_ASSERT(eventQueue_ != nullptr);

        // NOTE: Create IO service and HTTP client (conditional).
        if (options.enableNetwork) {
            ioService_ = std::make_unique<IOService>();
            if (auto err = ioService_->initialize(clock_); err != nullptr) {
                return errors::wrap(std::move(err), "IOService::initialize() failed.");
            }
            httpClient_ = std::make_unique<HTTPClient>(ioService_.get());
        }

        POMDOG_ASSERT(presentationParameters.presentationInterval > 0);
        presentationInterval_ = Duration(1) / presentationParameters.presentationInterval;
        // NOTE: Read the initial display-sync state from CAMetalLayer so that
        // displaySyncEnabled_ reflects the actual hardware configuration.
        if (@available(macOS 10.13, *)) {
            if (auto* layer = static_cast<CAMetalLayer*>(metalView_.layer)) {
                displaySyncEnabled_ = layer.displaySyncEnabled;
            }
        }

        return nullptr;
    }

    std::unique_ptr<Error>
    initializeGame(
        const std::weak_ptr<Game>& weakGameIn,
        const std::function<void()>& onCompletedIn) override
    {
        POMDOG_ASSERT(!weakGameIn.expired());
        POMDOG_ASSERT(onCompletedIn);
        weakGame_ = weakGameIn;
        onCompleted_ = onCompletedIn;

        POMDOG_ASSERT(!weakGame_.expired());
        auto game = weakGame_.lock();

        if (auto err = game->initialize(shared_from_this()); err != nullptr) {
            gameWillExit();
            return errors::wrap(std::move(err), "failed to initialize game");
        }

        if (exitRequest_) {
            gameWillExit();
        }

        return nullptr;
    }

    void gameLoop() override
    {
        POMDOG_ASSERT(graphicsContext_ != nullptr);

        if (exitRequest_) {
            return;
        }

        applyPendingDisplaySettings();

        graphicsContext_->dispatchSemaphoreWait();
        frameCounter_->updateFrame();

        POMDOG_ASSERT(!exitRequest_);
        POMDOG_ASSERT(!weakGame_.expired());

        auto game = weakGame_.lock();
        POMDOG_ASSERT(game);

        clock_->tick();
        if (keyboardImpl_) {
            keyboardImpl_->clearTextInput();
        }
        if (mouseImpl_) {
            mouseImpl_->clearScrollDelta();
        }
        window_->applyPendingWindowRequests();
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

        game->update();

        if (!viewLiveResizing_.load()) {
            renderFrame();
        }
    }

    bool isMetalSupported() const noexcept override
    {
        if (!graphicsDevice_) {
            return false;
        }

        POMDOG_ASSERT(graphicsDevice_ != nullptr);
        id<MTLDevice> metalDevice = graphicsDevice_->getMTLDevice();

        return metalDevice != nullptr;
    }

    void exit() override
    {
        exitRequest_ = true;
        gameWillExit();
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

    [[nodiscard]] bool
    getDisplaySyncEnabled() const noexcept override
    {
        return displaySyncEnabled_;
    }

    void
    setDisplaySyncEnabled(bool enabled) noexcept override
    {
        pendingDisplaySync_ = enabled;
    }

private:
    void applyPendingDisplaySettings() noexcept
    {
        if (pendingDisplaySync_.has_value()) {
            const bool enabled = *pendingDisplaySync_;
            if (@available(macOS 10.13, *)) {
                if (auto* layer = static_cast<CAMetalLayer*>(metalView_.layer)) {
                    layer.displaySyncEnabled = enabled ? YES : NO;
                }
            }
            displaySyncEnabled_ = enabled;
            pendingDisplaySync_ = std::nullopt;
        }
    }

    void renderFrame()
    {
        POMDOG_ASSERT(window_ != nullptr);
        POMDOG_ASSERT(!weakGame_.expired());

        bool skipRender = (!window_ || window_->isMinimized() || [NSApp isHidden] == YES);

        if (skipRender) {
            return;
        }

        auto game = weakGame_.lock();

        POMDOG_ASSERT(game);

        game->draw();
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

            clientSizeChanged();
            break;
        }
        case SystemEventKind::WindowModeChangedEvent: {
            if (auto* e = std::get_if<WindowModeChangedEvent>(&event.data); e != nullptr) {
                window_->windowModeChanged(e->windowMode);
            }
            clientSizeChanged();
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
        POMDOG_ASSERT(graphicsDevice_ != nullptr);
        auto bounds = window_->getClientBounds();

        // NOTE: Only notify when the client area dimensions actually changed since
        // the last report.  This prevents redundant framebuffer reallocations and
        // UI layout updates when, for example, only the window mode changes without
        // affecting the size.
        if (bounds.width != lastReportedBounds_.width ||
            bounds.height != lastReportedBounds_.height) {
            lastReportedBounds_ = bounds;

            graphicsDevice_->clientSizeChanged(bounds.width, bounds.height);
            window_->clientSizeChanged(bounds.width, bounds.height);
        }
    }

    void gameWillExit()
    {
        if (window_) {
            window_->setView(nullptr);
        }

        if (onCompleted_) {
            dispatch_async(dispatch_get_main_queue(), [this] {
                onCompleted_();
            });
        }
    }
};

} // namespace

GameHostMetal::GameHostMetal() = default;

GameHostMetal::~GameHostMetal() = default;

std::tuple<std::shared_ptr<GameHostMetal>, std::unique_ptr<Error>>
GameHostMetal::create(
    MTKView* metalView,
    const std::shared_ptr<GameWindowCocoa>& window,
    const std::shared_ptr<SystemEventQueue>& eventQueue,
    const gpu::PresentationParameters& presentationParameters,
    const GameHostOptions& options)
{
    auto host = std::make_shared<GameHostMetalImpl>();
    if (auto err = host->initialize(metalView, window, eventQueue, presentationParameters, options); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(host), nullptr);
}

} // namespace pomdog::detail::cocoa
