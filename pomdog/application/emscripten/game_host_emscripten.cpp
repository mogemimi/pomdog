// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/emscripten/game_host_emscripten.h"
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
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::emscripten {
namespace {

void mainLoopCallback(void* arg)
{
    auto* gameHost = reinterpret_cast<GameHostEmscripten*>(arg);
    POMDOG_ASSERT(gameHost != nullptr);
    gameHost->renderFrame();
}

} // namespace

GameHostEmscripten::GameHostEmscripten() noexcept = default;

std::unique_ptr<Error>
GameHostEmscripten::initialize(
    const gpu::PresentationParameters& presentationParameters,
    const std::string& targetCanvas)
{
    exitRequest_ = false;

    POMDOG_ASSERT(presentationParameters.presentationInterval > 0);
    presentationInterval_ = Duration(1.0) / presentationParameters.presentationInterval;

    // NOTE: Create time source and clock.
    timeSource_ = detail::makeTimeSource();
    clock_ = std::make_shared<GameClockImpl>();
    if (auto err = clock_->initialize(presentationParameters.presentationInterval, timeSource_);
        err != nullptr) {
        return errors::wrap(std::move(err), "GameClockImpl::initialize() failed.");
    }

    // NOTE: Create a game window.
    window_ = std::make_shared<GameWindowEmscripten>();
    if (auto err = window_->initialize(
            targetCanvas,
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize GameWindowEmscripten");
    }

    // NOTE: Create an OpenGL context.
    openGLContext_ = std::make_shared<OpenGLContextEmscripten>();
    if (auto err = openGLContext_->initialize(targetCanvas, presentationParameters);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize OpenGLContextEmscripten");
    }
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

    // NOTE: Create event queue.
    eventQueue_ = std::make_shared<EventQueue<SystemEvent>>();
    systemEventConnection_ = eventQueue_->connect([this](const SystemEvent& event) {
        processSystemEvents(event);
    });

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

GameHostEmscripten::~GameHostEmscripten()
{
    systemEventConnection_.disconnect();
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

void GameHostEmscripten::run(Game& game, int argc, const char* const* argv)
{
    game_ = &game;

    openGLContext_->makeCurrent();
    audioEngine_->makeCurrentContext();

    if (auto err = game_->initialize(shared_from_this(), argc, argv); err != nullptr) {
        Log::Critical("pomdog", err->toString());
        return;
    }

    openGLContext_->clearCurrent();
    audioEngine_->clearCurrentContext();

    // NOTE: Run the main loop using requestAnimationFrame (fps=0).
    emscripten_set_main_loop_arg(mainLoopCallback, this, 0, true);
}

void GameHostEmscripten::exit()
{
    exitRequest_ = true;
    emscripten_cancel_main_loop();
}

void GameHostEmscripten::renderFrame()
{
    if (exitRequest_) {
        return;
    }

    POMDOG_ASSERT(game_ != nullptr);

    {
        audioEngine_->makeCurrentContext();
        openGLContext_->makeCurrent();

        clock_->tick();
        keyboardImpl_->clearTextInput();
        mouseImpl_->clearScrollDelta();
        eventQueue_->emit();
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
}

std::shared_ptr<GameWindow>
GameHostEmscripten::getWindow() noexcept
{
    return window_;
}

std::shared_ptr<GameClock>
GameHostEmscripten::getClock() noexcept
{
    return clock_;
}

std::shared_ptr<gpu::GraphicsDevice>
GameHostEmscripten::getGraphicsDevice() noexcept
{
    return graphicsDevice_;
}

std::shared_ptr<gpu::CommandQueue>
GameHostEmscripten::getCommandQueue() noexcept
{
    return commandQueue_;
}

std::shared_ptr<AudioEngine>
GameHostEmscripten::getAudioEngine() noexcept
{
    return audioEngine_;
}

std::shared_ptr<Keyboard>
GameHostEmscripten::getKeyboard() noexcept
{
    return keyboardImpl_;
}

std::shared_ptr<Mouse>
GameHostEmscripten::getMouse() noexcept
{
    return mouseImpl_;
}

std::shared_ptr<Gamepad>
GameHostEmscripten::getGamepad() noexcept
{
    return gamepad_->getGamepad(PlayerIndex::One);
}

std::shared_ptr<GamepadService>
GameHostEmscripten::getGamepadService() noexcept
{
    return gamepad_;
}

std::shared_ptr<Touchscreen>
GameHostEmscripten::getTouchscreen() noexcept
{
    return touchscreen_;
}

std::shared_ptr<IOService>
GameHostEmscripten::getIOService() noexcept
{
    // NOTE: IOService is not supported on Emscripten.
    return nullptr;
}

std::shared_ptr<HTTPClient>
GameHostEmscripten::getHTTPClient() noexcept
{
    // NOTE: HTTPClient is not supported on Emscripten.
    return nullptr;
}

void GameHostEmscripten::processSystemEvents(const SystemEvent& event)
{
    switch (event.kind) {
    case SystemEventKind::WindowShouldCloseEvent: {
        exit();
        break;
    }
    default:
        keyboard_->handleMessage(event);
        mouse_->handleMessage(event);
        touchscreen_->handleMessage(event);
        break;
    }
}

} // namespace pomdog::detail::emscripten
