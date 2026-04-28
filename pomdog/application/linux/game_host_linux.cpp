// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/linux/game_host_linux.h"
#include "pomdog/application/game.h"
#include "pomdog/application/game_host_options.h"
#include "pomdog/application/x11/game_window_x11.h"
#include "pomdog/application/x11/opengl_context_x11.h"
#include "pomdog/application/x11/x11_context.h"
#include "pomdog/audio/openal/audio_engine_al.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/chrono/detail/make_time_source.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/gl4/graphics_context_gl4.h"
#include "pomdog/gpu/gl4/graphics_device_gl4.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/input/backends/keyboard_impl.h"
#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/gamepad_service.h"
#include "pomdog/input/linux/gamepad_linux.h"
#include "pomdog/input/player_index.h"
#include "pomdog/input/x11/keyboard_x11.h"
#include "pomdog/input/x11/mouse_x11.h"
#include "pomdog/logging/log.h"
#include "pomdog/network/http_client.h"
#include "pomdog/network/io_service.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::linux {
namespace {

[[nodiscard]] bool
checkFrameBufferConfigSupport(::Display* display) noexcept
{
    int majorVer = 0;
    int minorVer = 0;

    if (glXQueryVersion(display, &majorVer, &minorVer) == False) {
        return false;
    }

    return (((majorVer == 1) && (minorVer >= 3)) || (majorVer >= 2));
}

[[nodiscard]] std::tuple<GLXFBConfig, std::unique_ptr<Error>>
chooseFramebufferConfig(
    Display* display,
    const gpu::PresentationParameters& presentationParameters)
{
    if (!checkFrameBufferConfigSupport(display)) {
        return std::make_tuple(nullptr, errors::make("GLX of version lower than 1.3.2 is not supported."));
    }

    class final {
        std::array<int, 40> attribs;
        std::size_t index = 0;

    public:
        void add(int key, int value) noexcept
        {
            POMDOG_ASSERT(index + 1 < attribs.size());
            attribs[index] = key;
            attribs[index + 1] = value;
            index += 2;
        }

        const int* data() const noexcept
        {
            return attribs.data();
        }
    } attributes;

    attributes.add(GLX_X_RENDERABLE, True);
    attributes.add(GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT);
    attributes.add(GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR);
    attributes.add(GLX_DOUBLEBUFFER, True);

    if (presentationParameters.multiSampleCount >= 2) {
        // NOTE: Enable multi-sampling
        attributes.add(GLX_SAMPLE_BUFFERS, 1);
        attributes.add(GLX_SAMPLES, presentationParameters.multiSampleCount);
    }
    else {
        // NOTE: No multi-sampling
        attributes.add(GLX_SAMPLE_BUFFERS, 0);
    }

    switch (presentationParameters.backBufferFormat) {
    case gpu::PixelFormat::R16G16B16A16_Float:
        attributes.add(GLX_RENDER_TYPE, GLX_RGBA_FLOAT_BIT_ARB);
        attributes.add(GLX_RED_SIZE, 16);
        attributes.add(GLX_GREEN_SIZE, 16);
        attributes.add(GLX_BLUE_SIZE, 16);
        attributes.add(GLX_ALPHA_SIZE, 16);
        break;
    case gpu::PixelFormat::R32G32B32A32_Float:
        attributes.add(GLX_RENDER_TYPE, GLX_RGBA_FLOAT_BIT_ARB);
        attributes.add(GLX_RED_SIZE, 32);
        attributes.add(GLX_GREEN_SIZE, 32);
        attributes.add(GLX_BLUE_SIZE, 32);
        attributes.add(GLX_ALPHA_SIZE, 32);
        break;
    case gpu::PixelFormat::R8G8B8A8_UNorm:
        attributes.add(GLX_RENDER_TYPE, GLX_RGBA_BIT);
        attributes.add(GLX_RED_SIZE, 8);
        attributes.add(GLX_GREEN_SIZE, 8);
        attributes.add(GLX_BLUE_SIZE, 8);
        attributes.add(GLX_ALPHA_SIZE, 8);
        break;
    default:
        attributes.add(GLX_RED_SIZE, 4);
        attributes.add(GLX_GREEN_SIZE, 4);
        attributes.add(GLX_BLUE_SIZE, 4);
        break;
    }

    switch (presentationParameters.depthStencilFormat) {
    case gpu::PixelFormat::Depth16:
        attributes.add(GLX_DEPTH_SIZE, 16);
        break;
    case gpu::PixelFormat::Depth24Stencil8:
        attributes.add(GLX_DEPTH_SIZE, 24);
        attributes.add(GLX_STENCIL_SIZE, 8);
        break;
    case gpu::PixelFormat::Depth32:
        attributes.add(GLX_DEPTH_SIZE, 32);
        break;
    case gpu::PixelFormat::Depth32_Float_Stencil8_Uint:
        attributes.add(GLX_DEPTH_SIZE, 32);
        attributes.add(GLX_STENCIL_SIZE, 8);
        break;
    case gpu::PixelFormat::Invalid:
        break;
    default:
        return std::make_tuple(nullptr, errors::make("invalid depth stencil format"));
    }

    attributes.add(None, None);

    int framebufferConfigCount = 0;
    auto framebufferConfigs = glXChooseFBConfig(
        display,
        DefaultScreen(display),
        attributes.data(),
        &framebufferConfigCount);

    if ((framebufferConfigs == nullptr) || (framebufferConfigCount <= 0)) {
        return std::make_tuple(nullptr, errors::make("failed to retrieve FBConfig"));
    }

    GLXFBConfig bestConfig = nullptr;
    int bestSampleCount = 0;

    for (int index = 0; index < framebufferConfigCount; ++index) {
        auto framebufferConfig = framebufferConfigs[index];
        auto visualInfo = glXGetVisualFromFBConfig(display, framebufferConfig);

        if (visualInfo != nullptr) {
            int sampleBuffers = 0;
            int sampleCount = 0;
            glXGetFBConfigAttrib(display, framebufferConfig, GLX_SAMPLE_BUFFERS, &sampleBuffers);
            glXGetFBConfigAttrib(display, framebufferConfig, GLX_SAMPLES, &sampleCount);

            if ((bestConfig == nullptr) || ((sampleBuffers != 0) && (sampleCount > bestSampleCount))) {
                bestConfig = framebufferConfig;
                bestSampleCount = sampleCount;
            }
        }

        XFree(visualInfo);
    }

    XFree(framebufferConfigs);

    if (bestConfig == nullptr) {
        return std::make_tuple(nullptr, errors::make("cannot find any matching FBConfig"));
    }

    return std::make_tuple(bestConfig, nullptr);
}

class GameHostLinuxImpl final : public GameHostLinux {
private:
    std::shared_ptr<TimeSource> timeSource_;
    std::shared_ptr<GameClockImpl> clock_;
    std::shared_ptr<x11::X11Context> x11Context_;
    std::shared_ptr<x11::GameWindowX11> window_;
    std::shared_ptr<x11::OpenGLContextX11> openGLContext_;
    std::shared_ptr<gpu::detail::gl4::GraphicsDeviceGL4> graphicsDevice_;
    std::shared_ptr<gpu::detail::gl4::GraphicsContextGL4> graphicsContext_;
    std::shared_ptr<gpu::detail::CommandQueueImmediate> commandQueue_;
    std::shared_ptr<openal::AudioEngineAL> audioEngine_;
    std::shared_ptr<KeyboardImpl> keyboardImpl_;
    std::unique_ptr<x11::KeyboardX11> keyboard_;
    std::shared_ptr<MouseImpl> mouseImpl_;
    std::unique_ptr<x11::MouseX11> mouse_;
    std::unique_ptr<GamepadServiceLinux> gamepad_;
    std::unique_ptr<IOService> ioService_;
    std::unique_ptr<HTTPClient> httpClient_;
    Duration presentationInterval_ = Duration::zero();
    gpu::PixelFormat backBufferSurfaceFormat_;
    gpu::PixelFormat backBufferDepthStencilFormat_;
    bool exitRequest_ = false;

public:
    ~GameHostLinuxImpl() override
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
        commandQueue_.reset();
        graphicsContext_.reset();
        graphicsDevice_.reset();
        openGLContext_.reset();
        window_.reset();
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const gpu::PresentationParameters& presentationParameters,
        const GameHostOptions& options)
    {
        backBufferSurfaceFormat_ = presentationParameters.backBufferFormat;
        backBufferDepthStencilFormat_ = presentationParameters.depthStencilFormat;
        exitRequest_ = false;

        POMDOG_ASSERT(presentationParameters.presentationInterval > 0);
        presentationInterval_ = Duration(1.0) / presentationParameters.presentationInterval;

        timeSource_ = detail::makeTimeSource();
        clock_ = std::make_shared<GameClockImpl>();
        if (auto err = clock_->initialize(presentationParameters.presentationInterval, timeSource_); err != nullptr) {
            return errors::wrap(std::move(err), "GameClockImpl::Initialize() failed.");
        }

        // NOTE: Create X11 context.
        x11Context_ = std::make_shared<x11::X11Context>();
        if (auto err = x11Context_->initialize(); err != nullptr) {
            return errors::wrap(std::move(err), "failed to initialize X11Context");
        }

        auto [framebufferConfig, framebufferConfigErr] =
            chooseFramebufferConfig(x11Context_->Display, presentationParameters);
        if (framebufferConfigErr != nullptr) {
            return errors::wrap(std::move(framebufferConfigErr), "ChooseFramebufferConfig() failed");
        }

        // NOTE: Create a game window.
        auto [window, windowErr] = x11::GameWindowX11::create(
            x11Context_,
            framebufferConfig,
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight);
        if (windowErr != nullptr) {
            return errors::wrap(std::move(windowErr), "GameWindowX11::create() failed.");
        }
        window_ = std::move(window);

        // NOTE: Apply the initial window mode.
        if (presentationParameters.windowMode != WindowMode::Windowed) {
            if (auto err = window_->setWindowMode(presentationParameters.windowMode); err != nullptr) {
                return errors::wrap(std::move(err), "unsupported initial WindowMode for Linux");
            }
        }

        // NOTE: Create an OpenGL context.
        auto [openGLContext, glErr] = x11::OpenGLContextX11::create(window_, framebufferConfig);
        if (glErr != nullptr) {
            return errors::wrap(std::move(glErr), "OpenGLContextX11::create() failed.");
        }
        openGLContext_ = std::move(openGLContext);
        if (!openGLContext_->isOpenGL3Supported()) {
            return errors::make("Pomdog doesn't support versions of OpenGL lower than 3.3/4.0.");
        }

        openGLContext_->makeCurrent();

        auto const errorCode = glewInit();
        if (GLEW_OK != errorCode) {
            std::string description = reinterpret_cast<const char*>(glewGetErrorString(errorCode));
            return errors::make("glewInit() failed: " + description);
        }

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

        // NOTE: Create audio engine (conditional).
        if (options.enableAudio) {
            audioEngine_ = std::make_shared<openal::AudioEngineAL>();
            if (auto err = audioEngine_->initialize(); err != nullptr) {
                return errors::wrap(std::move(err), "failed to initialize AudioEngineAL");
            }
        }

        // NOTE: Create subsystems (conditional).
        if (!options.headless) {
            keyboardImpl_ = std::make_shared<KeyboardImpl>();
            keyboard_ = std::make_unique<x11::KeyboardX11>(x11Context_->Display, keyboardImpl_);
            mouseImpl_ = std::make_shared<MouseImpl>();
            mouse_ = std::make_unique<x11::MouseX11>(mouseImpl_);
        }

        // NOTE: Create gamepad (conditional).
        if (options.enableGamepad && !options.headless) {
            gamepad_ = std::make_unique<linux::GamepadServiceLinux>();
            if (auto err = gamepad_->initialize(options.gameControllerDB); err != nullptr) {
                return errors::wrap(std::move(err), "GamepadServiceLinux::initialize() failed");
            }
        }

        // NOTE: Create IO service and HTTP client (conditional).
        if (options.enableNetwork) {
            ioService_ = std::make_unique<IOService>();
            if (auto err = ioService_->initialize(clock_); err != nullptr) {
                return errors::wrap(std::move(err), "failed to initialize IOService");
            }
            httpClient_ = std::make_unique<HTTPClient>(ioService_.get());
        }

        return nullptr;
    }

    void run(Game& game) override
    {
        while (!exitRequest_) {
            clock_->tick();
            if (keyboardImpl_) {
                keyboardImpl_->clearTextInput();
            }
            if (mouseImpl_) {
                mouseImpl_->clearScrollDelta();
            }
            messagePump();
            constexpr int64_t gamepadDetectionInterval = 240;
            if (gamepad_ && ((clock_->getFrameNumber() % gamepadDetectionInterval) == 1) && (clock_->getFrameRate() >= 30.0f)) {
                gamepad_->enumerateDevices();
            }
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

            game.update();
            renderFrame(game);

            auto elapsedTime = clock_->getElapsedTime();

            if (elapsedTime < presentationInterval_) {
                auto sleepTime = (presentationInterval_ - elapsedTime);
                std::this_thread::sleep_for(sleepTime);
            }
        }
    }

    void exit() override
    {
        exitRequest_ = true;
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

    std::shared_ptr<Touchscreen>
    getTouchscreen() noexcept override
    {
        return nullptr;
    }

    std::shared_ptr<GamepadService>
    getGamepadService() noexcept override
    {
        std::shared_ptr<GamepadService> shared{shared_from_this(), gamepad_.get()};
        return shared;
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
        std::shared_ptr<HTTPClient> shared{shared_from_this(), httpClient_.get()};
        return shared;
    }

private:
    void messagePump()
    {
        ::XLockDisplay(x11Context_->Display);
        const auto eventCount = XPending(x11Context_->Display);
        ::XUnlockDisplay(x11Context_->Display);

        for (int index = 0; index < eventCount; ++index) {
            ::XEvent event;
            ::XLockDisplay(x11Context_->Display);
            ::XNextEvent(window_->getNativeDisplay(), &event);
            ::XUnlockDisplay(x11Context_->Display);

            processEvent(event);
        }
    }

    void processEvent(::XEvent& event)
    {
        if (event.xany.window != window_->getNativeWindow()) {
            return;
        }

        switch (event.type) {
        case ClientMessage: {
            const auto& atoms = x11Context_->Atoms;
            if (static_cast<Atom>(event.xclient.data.l[0]) == atoms.WmDeleteWindow) {
                Log::Internal("X11: wmDeleteMessage");
                exitRequest_ = true;
            }
            break;
        }
        case ConfigureNotify: {
            POMDOG_ASSERT(graphicsDevice_ != nullptr);
            const auto presentationParameters = graphicsDevice_->getPresentationParameters();
            if ((presentationParameters.backBufferWidth != event.xconfigure.width) ||
                (presentationParameters.backBufferHeight != event.xconfigure.height)) {
                graphicsDevice_->clientSizeChanged(event.xconfigure.width, event.xconfigure.height);
            }
            break;
        }
        case KeyPress:
        case KeyRelease: {
            keyboard_->handleEvent(event, window_->getInputContext());
            break;
        }
        case ButtonPress:
        case ButtonRelease:
        case EnterNotify:
        case MotionNotify:
        case LeaveNotify: {
            mouse_->handleEvent(event);
            break;
        }
        default:
            break;
        }

        window_->processEvent(event);
    }

    void renderFrame(Game& game)
    {
        if ((window_ == nullptr) || window_->isMinimized()) {
            return;
        }

        game.draw();
    }
};

} // namespace

GameHostLinux::GameHostLinux() noexcept = default;

GameHostLinux::~GameHostLinux() = default;

std::tuple<std::shared_ptr<GameHostLinux>, std::unique_ptr<Error>>
GameHostLinux::create(const gpu::PresentationParameters& presentationParameters,
    const GameHostOptions& options) noexcept
{
    auto host = std::make_shared<GameHostLinuxImpl>();
    if (auto err = host->initialize(presentationParameters, options); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(host), nullptr);
}

} // namespace pomdog::detail::linux
