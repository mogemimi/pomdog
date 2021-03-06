// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "GameHostX11.hpp"
#include "GamepadFactory.hpp"
#include "../Audio.OpenAL/AudioEngineAL.hpp"
#include "../Graphics.Backends/GraphicsCommandQueueImmediate.hpp"
#include "../Graphics.GL4/GraphicsContextGL4.hpp"
#include "../Graphics.GL4/GraphicsDeviceGL4.hpp"
#include "../Input.Backends/NativeGamepad.hpp"
#include "Pomdog/Application/FileSystem.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/GraphicsCommandQueue.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Network/HTTPClient.hpp"
#include "Pomdog/Network/IOService.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include <chrono>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

using Pomdog::Detail::NativeGamepad;
using Pomdog::Detail::GL4::GraphicsContextGL4;
using Pomdog::Detail::GL4::GraphicsDeviceGL4;
using Pomdog::Detail::OpenAL::AudioEngineAL;

namespace Pomdog::Detail::X11 {
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
    const PresentationParameters& presentationParameters)
{
    if (!CheckFrameBufferConfigSupport(display)) {
        return std::make_tuple(nullptr, Errors::New("GLX of version lower than 1.3.2 is not supported."));
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
    case SurfaceFormat::R16G16B16A16_Float:
        attributes.add(GLX_RENDER_TYPE, GLX_RGBA_FLOAT_BIT_ARB);
        attributes.add(GLX_RED_SIZE, 16);
        attributes.add(GLX_GREEN_SIZE, 16);
        attributes.add(GLX_BLUE_SIZE, 16);
        attributes.add(GLX_ALPHA_SIZE, 16);
        break;
    case SurfaceFormat::R32G32B32A32_Float:
        attributes.add(GLX_RENDER_TYPE, GLX_RGBA_FLOAT_BIT_ARB);
        attributes.add(GLX_RED_SIZE, 32);
        attributes.add(GLX_GREEN_SIZE, 32);
        attributes.add(GLX_BLUE_SIZE, 32);
        attributes.add(GLX_ALPHA_SIZE, 32);
        break;
    case SurfaceFormat::R8G8B8A8_UNorm:
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
    case SurfaceFormat::Depth16:
        attributes.add(GLX_DEPTH_SIZE, 16);
        break;
    case SurfaceFormat::Depth24Stencil8:
        attributes.add(GLX_DEPTH_SIZE, 24);
        attributes.add(GLX_STENCIL_SIZE, 8);
        break;
    case SurfaceFormat::Depth32:
        attributes.add(GLX_DEPTH_SIZE, 32);
        break;
    case SurfaceFormat::Depth32_Float_Stencil8_Uint:
        attributes.add(GLX_DEPTH_SIZE, 32);
        attributes.add(GLX_STENCIL_SIZE, 8);
        break;
    case SurfaceFormat::Invalid:
        break;
    default:
        return std::make_tuple(nullptr, Errors::New("invalid depth stencil format"));
    }

    attributes.add(None, None);

    int framebufferConfigCount = 0;
    auto framebufferConfigs = glXChooseFBConfig(
        display,
        DefaultScreen(display),
        attributes.data(),
        &framebufferConfigCount);

    if ((framebufferConfigs == nullptr) || (framebufferConfigCount <= 0)) {
        return std::make_tuple(nullptr, Errors::New("failed to retrieve FBConfig"));
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
        return std::make_tuple(nullptr, Errors::New("cannot find any matching FBConfig"));
    }

    return std::make_tuple(bestConfig, nullptr);
}

} // namespace

// MARK: - GameHostX11

class GameHostX11::Impl final {
public:
    ~Impl();

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const PresentationParameters& presentationParameters);

    void Run(Game& game);

    void Exit();

private:
    void MessagePump();
    void ProcessEvent(::XEvent& event);

    void RenderFrame(Game& game);

public:
    GameClock clock;
    std::shared_ptr<X11Context> x11Context;
    std::shared_ptr<GameWindowX11> window;
    std::shared_ptr<OpenGLContextX11> openGLContext;
    std::shared_ptr<GraphicsDeviceGL4> graphicsDevice;
    std::shared_ptr<GraphicsContextGL4> graphicsContext;
    std::shared_ptr<GraphicsCommandQueue> graphicsCommandQueue;
    std::shared_ptr<AudioEngineAL> audioEngine;
    std::unique_ptr<AssetManager> assetManager;
    std::unique_ptr<KeyboardX11> keyboard;
    MouseX11 mouse;
    std::unique_ptr<NativeGamepad> gamepad;
    std::unique_ptr<IOService> ioService;
    std::unique_ptr<HTTPClient> httpClient;
    Duration presentationInterval;
    SurfaceFormat backBufferSurfaceFormat;
    SurfaceFormat backBufferDepthStencilFormat;
    bool exitRequest = false;
};

std::unique_ptr<Error>
GameHostX11::Impl::Initialize(const PresentationParameters& presentationParameters)
{
    backBufferSurfaceFormat = presentationParameters.BackBufferFormat;
    backBufferDepthStencilFormat = presentationParameters.DepthStencilFormat;
    exitRequest = false;

    POMDOG_ASSERT(presentationParameters.PresentationInterval > 0);
    presentationInterval = Duration(1.0) / presentationParameters.PresentationInterval;

    // NOTE: Create X11 context.
    x11Context = std::make_shared<X11Context>();
    if (auto err = x11Context->Initialize(); err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to initialize X11Context");
    }

    auto [framebufferConfig, framebufferConfigErr] =
        ChooseFramebufferConfig(x11Context->Display, presentationParameters);
    if (framebufferConfigErr != nullptr) {
        return Errors::Wrap(std::move(framebufferConfigErr), "ChooseFramebufferConfig() failed");
    }

    // NOTE: Create a game window.
    window = std::make_shared<GameWindowX11>();
    if (auto err = window->Initialize(
            x11Context,
            framebufferConfig,
            presentationParameters.BackBufferWidth,
            presentationParameters.BackBufferHeight);
        err != nullptr) {
        return Errors::Wrap(std::move(framebufferConfigErr), "failed to initialize GameWindowX11");
    }

    // NOTE: Create an OpenGL context.
    openGLContext = std::make_shared<OpenGLContextX11>();
    if (auto err = openGLContext->Initialize(window, framebufferConfig); err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to initialize OpenGLContextX11");
    }
    if (!openGLContext->IsOpenGL3Supported()) {
        return Errors::New("Pomdog doesn't support versions of OpenGL lower than 3.3/4.0.");
    }

    openGLContext->MakeCurrent();

    auto const errorCode = glewInit();
    if (GLEW_OK != errorCode) {
        std::string description = reinterpret_cast<const char*>(glewGetErrorString(errorCode));
        return Errors::New("glewInit() failed: " + description);
    }

    // NOTE: Create a graphics device.
    graphicsDevice = std::make_shared<GraphicsDeviceGL4>();
    if (auto err = graphicsDevice->Initialize(presentationParameters); err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to initialize GraphicsDeviceGL4");
    }

    // NOTE: Create a graphics context.
    graphicsContext = std::make_shared<GraphicsContextGL4>();
    if (auto err = graphicsContext->Initialize(openGLContext, graphicsDevice); err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to initialize GraphicsContextGL4");
    }

    graphicsCommandQueue = std::make_shared<GraphicsCommandQueueImmediate>(graphicsContext);

    // NOTE: Create audio engine.
    audioEngine = std::make_shared<AudioEngineAL>();
    if (auto err = audioEngine->Initialize(); err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to initialize AudioEngineAL");
    }

    keyboard = std::make_unique<KeyboardX11>(x11Context->Display);
    gamepad = Detail::X11::CreateGamepad();

    auto [resourceDir, resourceDirErr] = FileSystem::GetResourceDirectoryPath();
    if (resourceDirErr != nullptr) {
        return Errors::Wrap(std::move(resourceDirErr), "FileSystem::GetResourceDirectoryPath() failed.");
    }
    auto contentDirectory = PathHelper::Join(resourceDir, "Content");

    // NOTE: Create asset manager.
    assetManager = std::make_unique<AssetManager>(
        std::move(contentDirectory),
        audioEngine,
        graphicsDevice);

    ioService = std::make_unique<IOService>(&clock);
    if (auto err = ioService->Initialize(); err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to initialize IOService");
    }
    httpClient = std::make_unique<HTTPClient>(ioService.get());

    return nullptr;
}

GameHostX11::Impl::~Impl()
{
    httpClient.reset();
    if (auto err = ioService->Shutdown(); err != nullptr) {
        Log::Warning("Pomdog", err->ToString());
    }
    ioService.reset();
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

void GameHostX11::Impl::MessagePump()
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

void GameHostX11::Impl::ProcessEvent(::XEvent& event)
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

void GameHostX11::Impl::Run(Game& game)
{
    while (!exitRequest) {
        clock.Tick();
        MessagePump();
        constexpr int64_t gamepadDetectionInterval = 240;
        if (((clock.GetFrameNumber() % gamepadDetectionInterval) == 1) && (clock.GetFrameRate() >= 30.0f)) {
            gamepad->EnumerateDevices();
        }
        gamepad->PollEvents();
        ioService->Step();

        game.Update();
        RenderFrame(game);

        auto elapsedTime = clock.GetElapsedTime();

        if (elapsedTime < presentationInterval) {
            auto sleepTime = (presentationInterval - elapsedTime);
            std::this_thread::sleep_for(sleepTime);
        }
    }
}

void GameHostX11::Impl::Exit()
{
    exitRequest = true;
}

void GameHostX11::Impl::RenderFrame(Game& game)
{
    if (!window || window->IsMinimized()) {
        // skip rendering
        return;
    }

    game.Draw();
}

// MARK: - GameHostX11

GameHostX11::GameHostX11() noexcept
    : impl(std::make_unique<Impl>())
{
}

GameHostX11::~GameHostX11() = default;

std::unique_ptr<Error>
GameHostX11::Initialize(const PresentationParameters& presentationParameters)
{
    POMDOG_ASSERT(impl != nullptr);
    return impl->Initialize(presentationParameters);
}

void GameHostX11::Run(Game& game)
{
    POMDOG_ASSERT(impl);
    impl->Run(game);
}

void GameHostX11::Exit()
{
    POMDOG_ASSERT(impl);
    impl->Exit();
}

std::shared_ptr<GameWindow> GameHostX11::GetWindow() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->window;
}

std::shared_ptr<GameClock> GameHostX11::GetClock() noexcept
{
    POMDOG_ASSERT(impl);
    auto gameHost = shared_from_this();
    std::shared_ptr<GameClock> sharedClock(gameHost, &impl->clock);
    return sharedClock;
}

std::shared_ptr<GraphicsDevice> GameHostX11::GetGraphicsDevice() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->graphicsDevice;
}

std::shared_ptr<GraphicsCommandQueue> GameHostX11::GetGraphicsCommandQueue() noexcept
{
    POMDOG_ASSERT(impl);
    return impl->graphicsCommandQueue;
}

std::shared_ptr<AudioEngine> GameHostX11::GetAudioEngine() noexcept
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->audioEngine);
    return impl->audioEngine;
}

std::shared_ptr<AssetManager> GameHostX11::GetAssetManager() noexcept
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->assetManager);
    auto gameHost = shared_from_this();
    std::shared_ptr<AssetManager> shared{gameHost, impl->assetManager.get()};
    return shared;
}

std::shared_ptr<Keyboard> GameHostX11::GetKeyboard() noexcept
{
    POMDOG_ASSERT(impl);
    auto gameHost = shared_from_this();
    std::shared_ptr<Keyboard> shared{gameHost, impl->keyboard.get()};
    return shared;
}

std::shared_ptr<Mouse> GameHostX11::GetMouse() noexcept
{
    POMDOG_ASSERT(impl);
    auto gameHost = shared_from_this();
    std::shared_ptr<Mouse> shared{gameHost, &impl->mouse};
    return shared;
}

std::shared_ptr<Gamepad> GameHostX11::GetGamepad() noexcept
{
    POMDOG_ASSERT(impl);
    auto gameHost = shared_from_this();
    std::shared_ptr<Gamepad> shared{gameHost, impl->gamepad.get()};
    return shared;
}

std::shared_ptr<IOService> GameHostX11::GetIOService() noexcept
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->ioService);
    auto gameHost = shared_from_this();
    std::shared_ptr<IOService> shared{gameHost, impl->ioService.get()};
    return shared;
}

std::shared_ptr<HTTPClient> GameHostX11::GetHTTPClient() noexcept
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->httpClient);
    auto gameHost = shared_from_this();
    std::shared_ptr<HTTPClient> shared{gameHost, impl->httpClient.get()};
    return shared;
}

} // namespace Pomdog::Detail::X11
