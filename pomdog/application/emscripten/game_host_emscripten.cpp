// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/emscripten/game_host_emscripten.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/emscripten/game_window_emscripten.h"
#include "pomdog/application/emscripten/opengl_context_emscripten.h"
#include "pomdog/application/game.h"
#include "pomdog/audio/openal/audio_engine_al.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/chrono/detail/make_time_source.h"
#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/gl4/graphics_context_gl4.h"
#include "pomdog/gpu/gl4/graphics_device_gl4.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/input/backends/keyboard_impl.h"
#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/emscripten/gamepad_emscripten.h"
#include "pomdog/input/emscripten/keyboard_emscripten.h"
#include "pomdog/input/emscripten/mouse_emscripten.h"
#include "pomdog/input/emscripten/touchscreen_emscripten.h"
#include "pomdog/input/gamepad_service.h"
#include "pomdog/input/player_index.h"
#include "pomdog/logging/log.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <thread>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::emscripten {
namespace {

void waitForTargetFrameRate(Duration targetSecondsPerFrame, const GameClockImpl& clock) noexcept
{
    POMDOG_ASSERT(targetSecondsPerFrame >= Duration::zero());

    const auto elapsedTime = clock.getElapsedTime();
    const auto sleepTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        targetSecondsPerFrame - elapsedTime);
    if (sleepTime > std::chrono::milliseconds{1}) {
        std::this_thread::sleep_for(sleepTime);
    }
}

class GameHostEmscriptenImpl final : public GameHostEmscripten {
private:
    std::shared_ptr<SystemEventQueue> eventQueue_;
    std::shared_ptr<TimeSource> timeSource_;
    std::shared_ptr<GameClockImpl> clock_;
    std::shared_ptr<GameWindowEmscripten> window_;
    std::shared_ptr<OpenGLContextEmscripten> openGLContext_;
    std::shared_ptr<gpu::detail::gl4::GraphicsDeviceGL4> graphicsDevice_;
    std::shared_ptr<gpu::detail::gl4::GraphicsContextGL4> graphicsContext_;
    std::shared_ptr<gpu::detail::CommandQueueImmediate> commandQueue_;
    std::shared_ptr<openal::AudioEngineAL> audioEngine_;
    std::shared_ptr<KeyboardImpl> keyboardImpl_;
    std::shared_ptr<KeyboardEmscripten> keyboard_;
    std::shared_ptr<MouseImpl> mouseImpl_;
    std::unique_ptr<MouseEmscripten> mouse_;
    std::shared_ptr<GamepadServiceEmscripten> gamepad_;
    std::shared_ptr<TouchscreenEmscripten> touchscreen_;
    unsafe_ptr<Game> game_ = nullptr;
    std::optional<i32> maxFramesPerSecond_;
    std::optional<Duration> targetFrameDuration_;
    std::optional<std::optional<i32>> pendingMaxFPS_;
    Rect2D lastReportedBounds_ = {0, 0, 0, 0};
    bool exitRequest_ = false;

public:
    ~GameHostEmscriptenImpl() override
    {
        if (gamepad_) {
            gamepad_.reset();
        }
        if (touchscreen_) {
            if (window_) {
                touchscreen_->unsubscribeEvent(window_->getTargetCanvas());
            }
            touchscreen_.reset();
        }
        if (mouse_) {
            if (window_) {
                mouse_->unsubscribeEvent(window_->getTargetCanvas());
            }
            mouse_.reset();
        }
        if (keyboard_) {
            keyboard_->unsubscribeEvent();
            keyboard_.reset();
        }
        keyboardImpl_.reset();
        mouseImpl_.reset();
        eventQueue_.reset();
        audioEngine_.reset();
        commandQueue_.reset();
        graphicsContext_.reset();
        graphicsDevice_.reset();
        openGLContext_.reset();
        window_.reset();
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const gpu::PresentationParameters& presentationParameters,
        const GameHostOptions& options,
        const std::string& targetCanvas)
    {
        exitRequest_ = false;

        // NOTE: Create time source and clock.
        timeSource_ = detail::makeTimeSource();
        clock_ = std::make_shared<GameClockImpl>();
        if (auto err = clock_->initialize(options.maxFramesPerSecond.value_or(60), timeSource_);
            err != nullptr) {
            return errors::wrap(std::move(err), "GameClockImpl::initialize() failed.");
        }

        if (options.maxFramesPerSecond != std::nullopt) {
            if (*options.maxFramesPerSecond <= 0) {
                return errors::make("maxFramesPerSecond must be > 0");
            }
            maxFramesPerSecond_ = *options.maxFramesPerSecond;
            targetFrameDuration_ = Duration(1.0) / *options.maxFramesPerSecond;
        }

        // NOTE: Create event queue (must be before window so fullscreen callbacks can enqueue).
        eventQueue_ = std::make_shared<SystemEventQueue>();

        // NOTE: Create a game window.
        auto [window, windowErr] = GameWindowEmscripten::create(
            targetCanvas,
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            eventQueue_);
        if (windowErr != nullptr) {
            return errors::wrap(std::move(windowErr), "GameWindowEmscripten::create() failed.");
        }
        window_ = std::move(window);

        // NOTE: Apply initial window mode (validates unsupported modes).
        if (presentationParameters.windowMode != WindowMode::Windowed) {
            if (auto err = window_->requestWindowMode(presentationParameters.windowMode); err != nullptr) {
                return errors::wrap(std::move(err), "unsupported initial WindowMode for Emscripten");
            }
            window_->applyPendingWindowRequests();
        }

        // NOTE: Create an OpenGL context.
        auto [openGLContext, glErr] = OpenGLContextEmscripten::create(targetCanvas, presentationParameters);
        if (glErr != nullptr) {
            return errors::wrap(std::move(glErr), "OpenGLContextEmscripten::create() failed.");
        }
        openGLContext_ = std::move(openGLContext);
        openGLContext_->makeCurrent();

        // NOTE: Create a graphics device.
        graphicsDevice_ = std::make_shared<gpu::detail::gl4::GraphicsDeviceGL4>();
        if (auto err = graphicsDevice_->initialize(presentationParameters); err != nullptr) {
            return errors::wrap(std::move(err), "failed to initialize GraphicsDeviceGL4");
        }

        // NOTE: Create a graphics context.
        graphicsContext_ = std::make_shared<gpu::detail::gl4::GraphicsContextGL4>();
        if (auto err = graphicsContext_->initialize(openGLContext_, graphicsDevice_); err != nullptr) {
            return errors::wrap(std::move(err), "failed to initialize GraphicsContextGL4");
        }

        commandQueue_ = std::make_shared<gpu::detail::CommandQueueImmediate>(graphicsContext_);

        // NOTE: Create audio engine.
        audioEngine_ = std::make_shared<openal::AudioEngineAL>();
        if (auto err = audioEngine_->initialize(); err != nullptr) {
            return errors::wrap(std::move(err), "failed to initialize AudioEngineAL");
        }

        // NOTE: Create input devices.
        keyboardImpl_ = std::make_shared<KeyboardImpl>();
        keyboard_ = std::make_shared<KeyboardEmscripten>(keyboardImpl_);
        keyboard_->subscribeEvent(eventQueue_);

        mouseImpl_ = std::make_shared<MouseImpl>();
        mouse_ = std::make_unique<MouseEmscripten>(mouseImpl_);
        mouse_->subscribeEvent(window_->getTargetCanvas(), eventQueue_);

        touchscreen_ = std::make_shared<TouchscreenEmscripten>();
        touchscreen_->subscribeEvent(window_->getTargetCanvas(), eventQueue_);

        gamepad_ = std::make_shared<GamepadServiceEmscripten>();

        return nullptr;
    }

    void run(Game& game) override
    {
        game_ = &game;

        openGLContext_->makeCurrent();
        audioEngine_->makeCurrentContext();

        if (auto err = game_->initialize(shared_from_this()); err != nullptr) {
            Log::Critical("pomdog", err->toString());
            return;
        }

        openGLContext_->clearCurrent();
        audioEngine_->clearCurrentContext();

        // NOTE: Run the main loop using requestAnimationFrame (fps=0).
        emscripten_set_main_loop_arg(mainLoopCallback, this, 0, true);
    }

    void exit() override
    {
        exitRequest_ = true;
        emscripten_cancel_main_loop();
    }

    void renderFrame()
    {
        if (exitRequest_) {
            return;
        }

        POMDOG_ASSERT(game_ != nullptr);

        {
            audioEngine_->makeCurrentContext();
            openGLContext_->makeCurrent();

            // NOTE: Apply pending frame rate changes at the start of each frame
            // boundary so that update/draw within a frame see consistent settings.
            applyPendingDisplaySettings();

            clock_->tick();
            keyboardImpl_->clearTextInput();
            mouseImpl_->clearScrollDelta();
            bool surfaceResizeRequest = false;
            window_->applyPendingWindowRequests();
            eventQueue_->emit([this, &surfaceResizeRequest](SystemEvent event) {
                processSystemEvents(std::move(event), surfaceResizeRequest);
            });

            // NOTE: Process any pending surface-resize notifications collected during
            // event dispatch above. Fires at most once per frame and only when the
            // client area dimensions actually changed since the last notification.
            if (surfaceResizeRequest) {
                const auto bounds = window_->getClientBounds();
                if (bounds.width != lastReportedBounds_.width ||
                    bounds.height != lastReportedBounds_.height) {
                    lastReportedBounds_ = bounds;

                    // NOTE: The canvas was resized (e.g. by `applyClientBounds` or the fullscreen
                    // `canvasResizedCallback`). Resize the OpenGL/graphics device to match.
                    graphicsDevice_->clientSizeChanged(bounds.width, bounds.height);

                    // NOTE: Notify game code that the client area size has changed.
                    window_->clientSizeChanged(bounds.width, bounds.height);
                }
            }
            gamepad_->pollEvents();

            if (touchscreen_->isMouseSimulationEnabled()) {
                touchscreen_->simulateUsingMouse(*mouseImpl_);
            }

            game_->update();

            openGLContext_->clearCurrent();
            audioEngine_->clearCurrentContext();
        }
        {
            audioEngine_->makeCurrentContext();
            openGLContext_->makeCurrent();

            game_->draw();

            openGLContext_->swapBuffers();
            openGLContext_->clearCurrent();

            audioEngine_->update();
            audioEngine_->clearCurrentContext();
        }

        // NOTE: If an explicit FPS cap is set, sleep to limit the frame rate.
        // On Emscripten requestAnimationFrame already paces frames to the
        // display's refresh rate, but a sleep_for here allows the game to cap
        // below 60 fps (e.g. 30 fps) when explicitly requested.
        if (targetFrameDuration_ != std::nullopt) {
            waitForTargetFrameRate(*targetFrameDuration_, *clock_);
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
        return commandQueue_;
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
        return touchscreen_;
    }

    std::shared_ptr<IOService>
    getIOService() noexcept override
    {
        // NOTE: IOService is not supported on Emscripten.
        return nullptr;
    }

    std::shared_ptr<HTTPClient>
    getHTTPClient() noexcept override
    {
        // NOTE: HTTPClient is not supported on Emscripten.
        return nullptr;
    }

    [[nodiscard]] std::optional<i32>
    getMaxFramesPerSecond() const noexcept override
    {
        return maxFramesPerSecond_;
    }

    void
    setMaxFramesPerSecond(std::optional<i32> maxFPS) noexcept override
    {
        POMDOG_ASSERT(!maxFPS.has_value() || *maxFPS > 0);
        pendingMaxFPS_ = maxFPS;
    }

    [[nodiscard]] bool
    getDisplaySyncEnabled() const noexcept override
    {
        // NOTE: On Emscripten the browser always synchronises rendering to the
        // display via requestAnimationFrame.  V-Sync is always effectively enabled
        // and cannot be disabled.
        return true;
    }

    void
    setDisplaySyncEnabled([[maybe_unused]] bool enabled) noexcept override
    {
        // NOTE: Emscripten does not provide a way to disable display sync (V-Sync).
        // The browser controls frame pacing through requestAnimationFrame.
    }

private:
    void applyPendingDisplaySettings() noexcept
    {
        if (pendingMaxFPS_.has_value()) {
            maxFramesPerSecond_ = *pendingMaxFPS_;
            if (maxFramesPerSecond_.has_value()) {
                POMDOG_ASSERT(*maxFramesPerSecond_ > 0);
                targetFrameDuration_ = Duration(1.0) / *maxFramesPerSecond_;
            }
            else {
                targetFrameDuration_ = std::nullopt;
            }
            pendingMaxFPS_ = std::nullopt;
        }
    }

    void processSystemEvents(const SystemEvent& event, bool& surfaceResizeRequest)
    {
        switch (event.kind) {
        case SystemEventKind::WindowShouldCloseEvent: {
            exit();
            break;
        }
        case SystemEventKind::WindowModeChangedEvent: {
            // NOTE: Resize graphics to match new window size and fire signal.
            if (auto* e = std::get_if<WindowModeChangedEvent>(&event.data); e != nullptr) {
                surfaceResizeRequest = true;
                window_->windowModeChanged(e->windowMode);
            }
            break;
        }
        case SystemEventKind::ViewNeedsUpdateSurfaceEvent: {
            surfaceResizeRequest = true;
            break;
        }
        default:
            keyboard_->handleMessage(event);
            mouse_->handleMessage(event);
            touchscreen_->handleMessage(event);
            break;
        }
    }

    static void
    mainLoopCallback(void* arg)
    {
        auto* gameHost = static_cast<GameHostEmscriptenImpl*>(arg);
        POMDOG_ASSERT(gameHost != nullptr);
        gameHost->renderFrame();
    }
};

} // namespace

GameHostEmscripten::GameHostEmscripten() noexcept = default;

GameHostEmscripten::~GameHostEmscripten() = default;

std::tuple<std::shared_ptr<GameHostEmscripten>, std::unique_ptr<Error>>
GameHostEmscripten::create(
    const gpu::PresentationParameters& presentationParameters,
    const GameHostOptions& options,
    const std::string& targetCanvas) noexcept
{
    auto host = std::make_shared<GameHostEmscriptenImpl>();
    if (auto err = host->initialize(presentationParameters, options, targetCanvas); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(host), nullptr);
}

} // namespace pomdog::detail::emscripten
