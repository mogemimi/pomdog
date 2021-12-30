// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/linux/game_host_linux.h"
#include "pomdog/application/game.h"
#include "pomdog/audio/openal/audio_engine_al.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/chrono/detail/make_time_source.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/gl4/graphics_context_gl4.h"
#include "pomdog/gpu/gl4/graphics_device_gl4.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/input/backends/native_gamepad.h"
#include "pomdog/input/x11/gamepad_factory.h"
#include "pomdog/logging/log.h"
#include "pomdog/network/http_client.h"
#include "pomdog/network/io_service.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"
#include <chrono>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

using pomdog::detail::NativeGamepad;
using pomdog::gpu::detail::gl4::GraphicsContextGL4;
using pomdog::gpu::detail::gl4::GraphicsDeviceGL4;
using pomdog::detail::openal::AudioEngineAL;
using pomdog::detail::x11::X11Context;
using pomdog::detail::x11::KeyboardX11;
using pomdog::detail::x11::MouseX11;
using pomdog::detail::x11::OpenGLContextX11;
using pomdog::detail::x11::GameWindowX11;

namespace pomdog::detail::linux {
namespace {

[[nodiscard]] bool
CheckFrameBufferConfigSupport(::Display* display) noexcept
{
    int majorVer = 0;
    int minorVer = 0;

    if (glXQueryVersion(display, &majorVer, &minorVer) == False) {
        return false;
    }

    return (((majorVer == 1) && (minorVer >= 3)) || (majorVer >= 2));
}

[[nodiscard]] std::tuple<GLXFBConfig, std::unique_ptr<Error>>
ChooseFramebufferConfig(
    Display* display,
    const gpu::PresentationParameters& presentationParameters)
{
    if (!CheckFrameBufferConfigSupport(display)) {
        return std::make_tuple(nullptr, errors::New("GLX of version lower than 1.3.2 is not supported."));
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

    if (presentationParameters.MultiSampleCount >= 2) {
        // NOTE: Enable multi-sampling
        attributes.add(GLX_SAMPLE_BUFFERS, 1);
        attributes.add(GLX_SAMPLES, presentationParameters.MultiSampleCount);
    }
    else {
        // NOTE: No multi-sampling
        attributes.add(GLX_SAMPLE_BUFFERS, 0);
    }

    switch (presentationParameters.BackBufferFormat) {
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

    switch (presentationParameters.DepthStencilFormat) {
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
        return std::make_tuple(nullptr, errors::New("invalid depth stencil format"));
    }

    attributes.add(None, None);

    int framebufferConfigCount = 0;
    auto framebufferConfigs = glXChooseFBConfig(
        display,
        DefaultScreen(display),
        attributes.data(),
        &framebufferConfigCount);

    if ((framebufferConfigs == nullptr) || (framebufferConfigCount <= 0)) {
        return std::make_tuple(nullptr, errors::New("failed to retrieve FBConfig"));
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
        return std::make_tuple(nullptr, errors::New("cannot find any matching FBConfig"));
    }

    return std::make_tuple(bestConfig, nullptr);
}

} // namespace

// MARK: - GameHostLinux

class GameHostLinux::Impl final {
public:
    ~Impl();

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const gpu::PresentationParameters& presentationParameters);

    void Run(Game& game);

    void Exit();

private:
    void MessagePump();
    void ProcessEvent(::XEvent& event);

    void RenderFrame(Game& game);

public:
    std::shared_ptr<detail::TimeSource> timeSource_;
    std::shared_ptr<GameClockImpl> clock_;
    std::shared_ptr<X11Context> x11Context;
    std::shared_ptr<GameWindowX11> window;
    std::shared_ptr<OpenGLContextX11> openGLContext;
    std::shared_ptr<GraphicsDeviceGL4> graphicsDevice;
    std::shared_ptr<GraphicsContextGL4> graphicsContext;
    std::shared_ptr<gpu::CommandQueue> graphicsCommandQueue;
    std::shared_ptr<AudioEngineAL> audioEngine;
    std::unique_ptr<AssetManager> assetManager;
    std::unique_ptr<KeyboardX11> keyboard;
    MouseX11 mouse;
    std::unique_ptr<NativeGamepad> gamepad;
    std::unique_ptr<IOService> ioService_;
    std::unique_ptr<HTTPClient> httpClient;
    Duration presentationInterval;
    PixelFormat backBufferSurfaceFormat;
    PixelFormat backBufferDepthStencilFormat;
    bool exitRequest = false;
};

std::unique_ptr<Error>
GameHostLinux::Impl::Initialize(const gpu::PresentationParameters& presentationParameters)
{
    backBufferSurfaceFormat = presentationParameters.BackBufferFormat;
    backBufferDepthStencilFormat = presentationParameters.DepthStencilFormat;
    exitRequest = false;

    POMDOG_ASSERT(presentationParameters.PresentationInterval > 0);
    presentationInterval = Duration(1.0) / presentationParameters.PresentationInterval;

    timeSource_ = detail::makeTimeSource();
    clock_ = std::make_shared<GameClockImpl>();
    if (auto err = clock_->Initialize(presentationParameters.PresentationInterval, timeSource_); err != nullptr) {
        return errors::Wrap(std::move(err), "GameClockImpl::Initialize() failed.");
    }

    // NOTE: Create X11 context.
    x11Context = std::make_shared<X11Context>();
    if (auto err = x11Context->Initialize(); err != nullptr) {
        return errors::Wrap(std::move(err), "failed to initialize X11Context");
    }

    auto [framebufferConfig, framebufferConfigErr] =
        ChooseFramebufferConfig(x11Context->Display, presentationParameters);
    if (framebufferConfigErr != nullptr) {
        return errors::Wrap(std::move(framebufferConfigErr), "ChooseFramebufferConfig() failed");
    }

    // NOTE: Create a game window.
    window = std::make_shared<GameWindowX11>();
    if (auto err = window->Initialize(
            x11Context,
            framebufferConfig,
            presentationParameters.BackBufferWidth,
            presentationParameters.BackBufferHeight);
        err != nullptr) {
        return errors::Wrap(std::move(framebufferConfigErr), "failed to initialize GameWindowX11");
    }

    // NOTE: Create an OpenGL context.
    openGLContext = std::make_shared<OpenGLContextX11>();
    if (auto err = openGLContext->Initialize(window, framebufferConfig); err != nullptr) {
        return errors::Wrap(std::move(err), "failed to initialize OpenGLContextX11");
    }
    if (!openGLContext->IsOpenGL3Supported()) {
        return errors::New("Pomdog doesn't support versions of OpenGL lower than 3.3/4.0.");
    }

    openGLContext->MakeCurrent();

    auto const errorCode = glewInit();
    if (GLEW_OK != errorCode) {
        std::string description = reinterpret_cast<const char*>(glewGetErrorString(errorCode));
        return errors::New("glewInit() failed: " + description);
    }

    // NOTE: Create a graphics device.
    graphicsDevice = std::make_shared<GraphicsDeviceGL4>();
    if (auto err = graphicsDevice->Initialize(presentationParameters); err != nullptr) {
        return errors::Wrap(std::move(err), "failed to initialize GraphicsDeviceGL4");
    }

    // NOTE: Create a graphics context.
    graphicsContext = std::make_shared<GraphicsContextGL4>();
    if (auto err = graphicsContext->Initialize(openGLContext, graphicsDevice); err != nullptr) {
        return errors::Wrap(std::move(err), "failed to initialize GraphicsContextGL4");
    }

    graphicsCommandQueue = std::make_shared<gpu::detail::CommandQueueImmediate>(graphicsContext);

    // NOTE: Create audio engine.
    audioEngine = std::make_shared<AudioEngineAL>();
    if (auto err = audioEngine->Initialize(); err != nullptr) {
        return errors::Wrap(std::move(err), "failed to initialize AudioEngineAL");
    }

    keyboard = std::make_unique<KeyboardX11>(x11Context->Display);
    gamepad = detail::x11::CreateGamepad();

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
        return errors::Wrap(std::move(err), "failed to initialize IOService");
    }
    httpClient = std::make_unique<HTTPClient>(ioService_.get());

    return nullptr;
}

GameHostLinux::Impl::~Impl()
{
    httpClient.reset();
    if (auto err = ioService_->Shutdown(); err != nullptr) {
        Log::Warning("pomdog", err->ToString());
    }
    ioService_.reset();
    gamepad.reset();
    keyboard.reset();
    assetManager.reset();
    audioEngine.reset();
    graphicsCommandQueue.reset();
    graphicsContext.reset();
    graphicsDevice.reset();
    openGLContext.reset();
    window.reset();
}

void GameHostLinux::Impl::MessagePump()
{
    ::XLockDisplay(x11Context->Display);
    const auto eventCount = XPending(x11Context->Display);
    ::XUnlockDisplay(x11Context->Display);

    for (int index = 0; index < eventCount; ++index) {
        ::XEvent event;
        ::XLockDisplay(x11Context->Display);
        ::XNextEvent(window->GetNativeDisplay(), &event);
        ::XUnlockDisplay(x11Context->Display);

        ProcessEvent(event);
    }
}

void GameHostLinux::Impl::ProcessEvent(::XEvent& event)
{
    if (event.xany.window != window->GetNativeWindow()) {
        return;
    }

    switch (event.type) {
    case ClientMessage: {
        const auto& atoms = x11Context->Atoms;
        if (static_cast<Atom>(event.xclient.data.l[0]) == atoms.WmDeleteWindow) {
            Log::Internal("X11: wmDeleteMessage");
            exitRequest = true;
        }
        break;
    }
    case ConfigureNotify: {
        POMDOG_ASSERT(graphicsDevice != nullptr);
        auto presentationParameters = graphicsDevice->GetPresentationParameters();
        if ((presentationParameters.BackBufferWidth != event.xconfigure.width) ||
            (presentationParameters.BackBufferHeight != event.xconfigure.height)) {
            graphicsDevice->ClientSizeChanged(event.xconfigure.width, event.xconfigure.height);
        }
        break;
    }
    case KeyPress:
    case KeyRelease: {
        keyboard->HandleEvent(event, window->GetInputContext());
        break;
    }
    case ButtonPress:
    case ButtonRelease:
    case EnterNotify:
    case MotionNotify:
    case LeaveNotify: {
        mouse.HandleEvent(event);
        break;
    }
    default:
        break;
    }

    window->ProcessEvent(event);
}

void GameHostLinux::Impl::Run(Game& game)
{
    while (!exitRequest) {
        clock_->Tick();
        MessagePump();
        constexpr int64_t gamepadDetectionInterval = 240;
        if (((clock_->GetFrameNumber() % gamepadDetectionInterval) == 1) && (clock_->GetFrameRate() >= 30.0f)) {
            gamepad->EnumerateDevices();
        }
        gamepad->PollEvents();
        ioService_->Step();

        game.Update();
        RenderFrame(game);

        auto elapsedTime = clock_->GetElapsedTime();

        if (elapsedTime < presentationInterval) {
            auto sleepTime = (presentationInterval - elapsedTime);
            std::this_thread::sleep_for(sleepTime);
        }
    }
}

void GameHostLinux::Impl::Exit()
{
    exitRequest = true;
}

void GameHostLinux::Impl::RenderFrame(Game& game)
{
    if (!window || window->IsMinimized()) {
        // skip rendering
        return;
    }

    game.Draw();
}

// MARK: - GameHostLinux

GameHostLinux::GameHostLinux() noexcept
    : impl(std::make_unique<Impl>())
{
}

GameHostLinux::~GameHostLinux() = default;

std::unique_ptr<Error>
GameHostLinux::Initialize(const gpu::PresentationParameters& presentationParameters)
{
    POMDOG_ASSERT(impl != nullptr);
    return impl->Initialize(presentationParameters);
}

void GameHostLinux::Run(Game& game)
{
    POMDOG_ASSERT(impl);
    impl->Run(game);
}

void GameHostLinux::Exit()
{
    POMDOG_ASSERT(impl);
    impl->Exit();
}

std::shared_ptr<GameWindow> GameHostLinux::GetWindow() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->window;
}

std::shared_ptr<GameClock> GameHostLinux::GetClock() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->clock_;
}

std::shared_ptr<gpu::GraphicsDevice> GameHostLinux::GetGraphicsDevice() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->graphicsDevice;
}

std::shared_ptr<gpu::CommandQueue> GameHostLinux::GetCommandQueue() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->graphicsCommandQueue;
}

std::shared_ptr<AudioEngine> GameHostLinux::GetAudioEngine() noexcept
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->audioEngine);
    return impl->audioEngine;
}

std::shared_ptr<AssetManager> GameHostLinux::GetAssetManager() noexcept
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->assetManager);
    auto gameHost = shared_from_this();
    std::shared_ptr<AssetManager> shared{gameHost, impl->assetManager.get()};
    return shared;
}

std::shared_ptr<Keyboard> GameHostLinux::GetKeyboard() noexcept
{
    POMDOG_ASSERT(impl);
    auto gameHost = shared_from_this();
    std::shared_ptr<Keyboard> shared{gameHost, impl->keyboard.get()};
    return shared;
}

std::shared_ptr<Mouse> GameHostLinux::GetMouse() noexcept
{
    POMDOG_ASSERT(impl);
    auto gameHost = shared_from_this();
    std::shared_ptr<Mouse> shared{gameHost, &impl->mouse};
    return shared;
}

std::shared_ptr<Gamepad> GameHostLinux::GetGamepad() noexcept
{
    POMDOG_ASSERT(impl);
    auto gameHost = shared_from_this();
    std::shared_ptr<Gamepad> shared{gameHost, impl->gamepad.get()};
    return shared;
}

std::shared_ptr<IOService> GameHostLinux::GetIOService() noexcept
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->ioService_);
    auto gameHost = shared_from_this();
    std::shared_ptr<IOService> shared{gameHost, impl->ioService_.get()};
    return shared;
}

std::shared_ptr<HTTPClient> GameHostLinux::GetHTTPClient() noexcept
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->httpClient);
    auto gameHost = shared_from_this();
    std::shared_ptr<HTTPClient> shared{gameHost, impl->httpClient.get()};
    return shared;
}

} // namespace pomdog::detail::linux
