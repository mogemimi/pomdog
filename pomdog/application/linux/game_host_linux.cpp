// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/linux/game_host_linux.h"
#include "pomdog/application/game.h"
#include "pomdog/application/x11/game_window_x11.h"
#include "pomdog/application/x11/x11_context.h"
#include "pomdog/audio/openal/audio_engine_al.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/chrono/detail/make_time_source.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/gl4/graphics_context_gl4.h"
#include "pomdog/gpu/gl4/graphics_device_gl4.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/input/linux/gamepad_linux.h"
#include "pomdog/input/x11/keyboard_x11.h"
#include "pomdog/logging/log.h"
#include "pomdog/network/http_client.h"
#include "pomdog/network/io_service.h"
#include "pomdog/platform/x11/opengl_context_x11.h"
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
            assert(index + 1 < attribs.size());
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
    case PixelFormat::R16G16B16A16_Float:
        attributes.add(GLX_RENDER_TYPE, GLX_RGBA_FLOAT_BIT_ARB);
        attributes.add(GLX_RED_SIZE, 16);
        attributes.add(GLX_GREEN_SIZE, 16);
        attributes.add(GLX_BLUE_SIZE, 16);
        attributes.add(GLX_ALPHA_SIZE, 16);
        break;
    case PixelFormat::R32G32B32A32_Float:
        attributes.add(GLX_RENDER_TYPE, GLX_RGBA_FLOAT_BIT_ARB);
        attributes.add(GLX_RED_SIZE, 32);
        attributes.add(GLX_GREEN_SIZE, 32);
        attributes.add(GLX_BLUE_SIZE, 32);
        attributes.add(GLX_ALPHA_SIZE, 32);
        break;
    case PixelFormat::R8G8B8A8_UNorm:
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
    case PixelFormat::Depth16:
        attributes.add(GLX_DEPTH_SIZE, 16);
        break;
    case PixelFormat::Depth24Stencil8:
        attributes.add(GLX_DEPTH_SIZE, 24);
        attributes.add(GLX_STENCIL_SIZE, 8);
        break;
    case PixelFormat::Depth32:
        attributes.add(GLX_DEPTH_SIZE, 32);
        break;
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        attributes.add(GLX_DEPTH_SIZE, 32);
        attributes.add(GLX_STENCIL_SIZE, 8);
        break;
    case PixelFormat::Invalid:
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

} // namespace

GameHostLinux::GameHostLinux() noexcept = default;

std::unique_ptr<Error>
GameHostLinux::initialize(const gpu::PresentationParameters& presentationParameters)
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
    window_ = std::make_shared<x11::GameWindowX11>();
    if (auto err = window_->initialize(
            x11Context_,
            framebufferConfig,
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight);
        err != nullptr) {
        return errors::wrap(std::move(framebufferConfigErr), "failed to initialize GameWindowX11");
    }

    // NOTE: Create an OpenGL context.
    openGLContext_ = std::make_shared<x11::OpenGLContextX11>();
    if (auto err = openGLContext_->initialize(window_, framebufferConfig); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize OpenGLContextX11");
    }
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

    // NOTE: Create audio engine.
    audioEngine_ = std::make_shared<openal::AudioEngineAL>();
    if (auto err = audioEngine_->initialize(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize AudioEngineAL");
    }

    keyboard_ = std::make_unique<x11::KeyboardX11>(x11Context_->Display);
    gamepad_ = std::make_unique<linux::GamepadLinux>();

    auto [resourceDir, resourceDirErr] = FileSystem::getResourceDirectoryPath();
    if (resourceDirErr != nullptr) {
        return errors::wrap(std::move(resourceDirErr), "FileSystem::getResourceDirectoryPath() failed.");
    }
    auto contentDirectory = filepaths::join(resourceDir, "content");

    // NOTE: Create asset manager.
    assetManager_ = std::make_shared<AssetManager>(
        std::move(contentDirectory),
        audioEngine_,
        graphicsDevice_);

    ioService_ = std::make_unique<IOService>();
    if (auto err = ioService_->initialize(clock_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize IOService");
    }
    httpClient_ = std::make_unique<HTTPClient>(ioService_.get());

    return nullptr;
}

GameHostLinux::~GameHostLinux()
{
    httpClient_.reset();
    if (auto err = ioService_->shutdown(); err != nullptr) {
        Log::Warning("pomdog", err->toString());
    }
    ioService_.reset();
    gamepad_.reset();
    keyboard_.reset();
    assetManager_.reset();
    audioEngine_.reset();
    commandQueue_.reset();
    graphicsContext_.reset();
    graphicsDevice_.reset();
    openGLContext_.reset();
    window_.reset();
}

void GameHostLinux::messagePump()
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

void GameHostLinux::processEvent(::XEvent& event)
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
        mouse_.handleEvent(event);
        break;
    }
    default:
        break;
    }

    window_->processEvent(event);
}

void GameHostLinux::run(Game& game)
{
    while (!exitRequest_) {
        clock_->tick();
        messagePump();
        constexpr int64_t gamepadDetectionInterval = 240;
        if (((clock_->getFrameNumber() % gamepadDetectionInterval) == 1) && (clock_->getFrameRate() >= 30.0f)) {
            gamepad_->enumerateDevices();
        }
        gamepad_->pollEvents();
        ioService_->step();

        game.update();
        renderFrame(game);

        auto elapsedTime = clock_->getElapsedTime();

        if (elapsedTime < presentationInterval_) {
            auto sleepTime = (presentationInterval_ - elapsedTime);
            std::this_thread::sleep_for(sleepTime);
        }
    }
}

void GameHostLinux::exit()
{
    exitRequest_ = true;
}

void GameHostLinux::renderFrame(Game& game)
{
    if ((window_ == nullptr) || window_->isMinimized()) {
        // skip rendering
        return;
    }

    game.draw();
}

std::shared_ptr<GameWindow> GameHostLinux::getWindow() noexcept
{
    return window_;
}

std::shared_ptr<GameClock> GameHostLinux::getClock() noexcept
{
    return clock_;
}

std::shared_ptr<gpu::GraphicsDevice> GameHostLinux::getGraphicsDevice() noexcept
{
    return graphicsDevice_;
}

std::shared_ptr<gpu::CommandQueue> GameHostLinux::getCommandQueue() noexcept
{
    return commandQueue_;
}

std::shared_ptr<AudioEngine> GameHostLinux::getAudioEngine() noexcept
{
    return audioEngine_;
}

std::shared_ptr<AssetManager> GameHostLinux::getAssetManager() noexcept
{
    POMDOG_ASSERT(assetManager_ != nullptr);
    return assetManager_;
}

std::shared_ptr<Keyboard> GameHostLinux::getKeyboard() noexcept
{
    auto gameHost = shared_from_this();
    std::shared_ptr<Keyboard> shared{std::move(gameHost), keyboard_.get()};
    return shared;
}

std::shared_ptr<Mouse> GameHostLinux::getMouse() noexcept
{
    auto gameHost = shared_from_this();
    std::shared_ptr<Mouse> shared{std::move(gameHost), &mouse_};
    return shared;
}

std::shared_ptr<Gamepad> GameHostLinux::getGamepad() noexcept
{
    auto gameHost = shared_from_this();
    std::shared_ptr<Gamepad> shared{std::move(gameHost), gamepad_.get()};
    return shared;
}

std::shared_ptr<IOService> GameHostLinux::getIOService() noexcept
{
    POMDOG_ASSERT(ioService_ != nullptr);
    auto gameHost = shared_from_this();
    std::shared_ptr<IOService> shared{std::move(gameHost), ioService_.get()};
    return shared;
}

std::shared_ptr<HTTPClient> GameHostLinux::getHTTPClient() noexcept
{
    POMDOG_ASSERT(httpClient_ != nullptr);
    auto gameHost = shared_from_this();
    std::shared_ptr<HTTPClient> shared{std::move(gameHost), httpClient_.get()};
    return shared;
}

} // namespace pomdog::detail::linux
