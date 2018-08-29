// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GameHostX11.hpp"
#include "../InputSystem/GamepadFactory.hpp"
#include "../InputSystem/NativeGamepad.hpp"
#include "../RenderSystem.GL4/GraphicsContextGL4.hpp"
#include "../RenderSystem.GL4/GraphicsDeviceGL4.hpp"
#include "../RenderSystem/GraphicsCommandQueueImmediate.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Audio/AudioEngine.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Graphics/GraphicsCommandQueue.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include <chrono>
#include <string>
#include <thread>
#include <vector>

using Pomdog::Detail::GL4::GraphicsContextGL4;
using Pomdog::Detail::GL4::GraphicsDeviceGL4;
using Pomdog::Detail::InputSystem::NativeGamepad;

namespace Pomdog {
namespace Detail {
namespace X11 {
namespace {

bool CheckFrameBufferConfigSupport(::Display* display)
{
    int majorVer = 0;
    int minorVer = 0;

    if (glXQueryVersion(display, &majorVer, &minorVer) == False) {
        return false;
    }

    return (((majorVer == 1) && (minorVer >= 3)) || (majorVer >= 2));
}

GLXFBConfig ChooseFramebufferConfig(
    Display* display,
    const PresentationParameters& presentationParameters)
{
    if (!CheckFrameBufferConfigSupport(display)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "GLX of version lower than 1.3.2 is not supported.");
    }

    std::vector<int> visualAttributes = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_DOUBLEBUFFER, True,
        //GLX_SAMPLE_BUFFERS, 1,
        //GLX_SAMPLES, 4,
    };

    switch (presentationParameters.BackBufferFormat) {
    case SurfaceFormat::R16G16B16A16_Float:
        visualAttributes.push_back(GLX_RENDER_TYPE);
        visualAttributes.push_back(GLX_RGBA_FLOAT_BIT_ARB);
        visualAttributes.push_back(GLX_RED_SIZE);
        visualAttributes.push_back(16);
        visualAttributes.push_back(GLX_GREEN_SIZE);
        visualAttributes.push_back(16);
        visualAttributes.push_back(GLX_BLUE_SIZE);
        visualAttributes.push_back(16);
        visualAttributes.push_back(GLX_ALPHA_SIZE);
        visualAttributes.push_back(16);
        break;
    case SurfaceFormat::R32G32B32A32_Float:
        visualAttributes.push_back(GLX_RENDER_TYPE);
        visualAttributes.push_back(GLX_RGBA_FLOAT_BIT_ARB);
        visualAttributes.push_back(GLX_RED_SIZE);
        visualAttributes.push_back(32);
        visualAttributes.push_back(GLX_GREEN_SIZE);
        visualAttributes.push_back(32);
        visualAttributes.push_back(GLX_BLUE_SIZE);
        visualAttributes.push_back(32);
        visualAttributes.push_back(GLX_ALPHA_SIZE);
        visualAttributes.push_back(32);
        break;
    default:
        visualAttributes.push_back(GLX_RENDER_TYPE);
        visualAttributes.push_back(GLX_RGBA_BIT);
        visualAttributes.push_back(GLX_RED_SIZE);
        visualAttributes.push_back(8);
        visualAttributes.push_back(GLX_GREEN_SIZE);
        visualAttributes.push_back(8);
        visualAttributes.push_back(GLX_BLUE_SIZE);
        visualAttributes.push_back(8);
        visualAttributes.push_back(GLX_ALPHA_SIZE);
        visualAttributes.push_back(8);
        break;
    }

    switch (presentationParameters.DepthStencilFormat) {
    case DepthFormat::Depth16:
        visualAttributes.push_back(GLX_DEPTH_SIZE);
        visualAttributes.push_back(16);
        break;
    case DepthFormat::Depth24Stencil8:
        visualAttributes.push_back(GLX_DEPTH_SIZE);
        visualAttributes.push_back(24);
        visualAttributes.push_back(GLX_STENCIL_SIZE);
        visualAttributes.push_back(8);
        break;
    case DepthFormat::Depth32:
        visualAttributes.push_back(GLX_DEPTH_SIZE);
        visualAttributes.push_back(32);
        break;
    case DepthFormat::Depth32_Float_Stencil8_Uint:
        visualAttributes.push_back(GLX_DEPTH_SIZE);
        visualAttributes.push_back(32);
        visualAttributes.push_back(GLX_STENCIL_SIZE);
        visualAttributes.push_back(8);
        break;
    default:
        break;
    }

    visualAttributes.push_back(None);

    int framebufferConfigCount = 0;
    auto framebufferConfigs = glXChooseFBConfig(display, DefaultScreen(display),
        visualAttributes.data(), &framebufferConfigCount);

    if ((framebufferConfigs == nullptr) || (framebufferConfigCount <= 0)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to retrieve FBConfig");
    }

    std::optional<GLXFBConfig> bestConfig;
    int bestSampleCount = 0;

    for (int index = 0; index < framebufferConfigCount; ++index) {
        auto framebufferConfig = framebufferConfigs[index];
        auto visualInfo = glXGetVisualFromFBConfig(display, framebufferConfig);

        if (visualInfo != nullptr) {
            int sampleBuffers = 0;
            int sampleCount = 0;
            glXGetFBConfigAttrib(display, framebufferConfig, GLX_SAMPLE_BUFFERS, &sampleBuffers);
            glXGetFBConfigAttrib(display, framebufferConfig, GLX_SAMPLES, &sampleCount);

            if (!bestConfig || ((sampleBuffers != 0) && (sampleCount > bestSampleCount))) {
                bestConfig = framebufferConfig;
                bestSampleCount = sampleCount;
            }
        }

        XFree(visualInfo);
    }

    XFree(framebufferConfigs);

    if (!bestConfig) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot find any matching FBConfig");
    }

    return *bestConfig;
}

} // unnamed namespace

// MARK: - GameHostX11

class GameHostX11::Impl {
public:
    explicit Impl(const PresentationParameters& presentationParameters);

    ~Impl();

    void Run(Game & game);

    void Exit();

private:
    void MessagePump();
    void ProcessEvent(::XEvent & event);

    void RenderFrame(Game & game);

public:
    GameClock clock;
    std::shared_ptr<X11Context> x11Context;
    std::shared_ptr<GameWindowX11> window;
    std::shared_ptr<OpenGLContextX11> openGLContext;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<GraphicsContextGL4> graphicsContext;
    std::shared_ptr<GraphicsCommandQueue> graphicsCommandQueue;
    std::shared_ptr<AudioEngine> audioEngine;
    std::unique_ptr<Pomdog::AssetManager> assetManager;
    std::unique_ptr<KeyboardX11> keyboard;
    MouseX11 mouse;
    std::unique_ptr<NativeGamepad> gamepad;
    Duration presentationInterval;
    SurfaceFormat backBufferSurfaceFormat;
    DepthFormat backBufferDepthStencilFormat;
    bool exitRequest;
};

GameHostX11::Impl::Impl(const PresentationParameters& presentationParameters)
    : backBufferSurfaceFormat(presentationParameters.BackBufferFormat)
    , backBufferDepthStencilFormat(presentationParameters.DepthStencilFormat)
    , exitRequest(false)
{
    POMDOG_ASSERT(presentationParameters.PresentationInterval > 0);
    presentationInterval = Duration(1.0) / presentationParameters.PresentationInterval;

    x11Context = std::make_shared<X11Context>();

    auto framebufferConfig = ChooseFramebufferConfig(
        x11Context->Display, presentationParameters);

    window = std::make_shared<GameWindowX11>(
        x11Context,
        framebufferConfig,
        presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight);

    openGLContext = std::make_shared<OpenGLContextX11>(window, framebufferConfig);

    if (!openGLContext->IsOpenGL3Supported()) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Pomdog doesn't support versions of OpenGL lower than 3.3/4.0.");
    }

    openGLContext->MakeCurrent();

    auto const errorCode = glewInit();
    if (GLEW_OK != errorCode) {
        auto description = reinterpret_cast<const char*>(glewGetErrorString(errorCode));
        POMDOG_THROW_EXCEPTION(std::runtime_error, description);
    }

    graphicsDevice = std::make_shared<GraphicsDevice>(
        std::make_unique<GraphicsDeviceGL4>(presentationParameters));

    graphicsContext = std::make_shared<GraphicsContextGL4>(openGLContext, window);

    graphicsCommandQueue = std::make_shared<GraphicsCommandQueue>(
        std::make_unique<GraphicsCommandQueueImmediate>(graphicsContext));

    audioEngine = std::make_shared<AudioEngine>();

    keyboard = std::make_unique<KeyboardX11>(x11Context->Display);
    gamepad = Detail::InputSystem::CreateGamepad();

    Detail::AssetLoaderContext loaderContext;
    loaderContext.RootDirectory = PathHelper::Join(FileSystem::GetResourceDirectoryPath(), "Content");
    loaderContext.GraphicsDevice = graphicsDevice;
    assetManager = std::make_unique<AssetManager>(std::move(loaderContext));
}

GameHostX11::Impl::~Impl()
{
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
        ::XNextEvent(window->NativeDisplay(), &event);
        ::XUnlockDisplay(x11Context->Display);

        ProcessEvent(event);
    }
}

void GameHostX11::Impl::ProcessEvent(::XEvent & event)
{
    if (event.xany.window != window->NativeWindow()) {
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
        POMDOG_ASSERT(graphicsDevice);
        POMDOG_ASSERT(graphicsDevice->GetNativeGraphicsDevice());
        auto nativeDevice = static_cast<GraphicsDeviceGL4*>(graphicsDevice->GetNativeGraphicsDevice());
        auto presentationParameters = nativeDevice->GetPresentationParameters();
        if ((presentationParameters.BackBufferWidth != event.xconfigure.width) ||
            (presentationParameters.BackBufferHeight != event.xconfigure.height)) {
            nativeDevice->ClientSizeChanged(event.xconfigure.width, event.xconfigure.height);
        }
        break;
    }
    case KeyPress:
    case KeyRelease: {
        keyboard->HandleEvent(event);
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

void GameHostX11::Impl::Run(Game & game)
{
    game.Initialize();

    while (!exitRequest) {
        clock.Tick();
        MessagePump();
        constexpr int64_t gamepadDetectionInterval = 240;
        if (((clock.GetFrameNumber() % gamepadDetectionInterval) == 1) && (clock.GetFrameRate() >= 30.0f)) {
            gamepad->EnumerateDevices();
        }
        gamepad->PollEvents();

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

void GameHostX11::Impl::RenderFrame(Game & game)
{
    if (!window || window->IsMinimized()) {
        // skip rendering
        return;
    }

    game.Draw();
}

// MARK: - GameHostX11

GameHostX11::GameHostX11(const PresentationParameters& presentationParameters)
    : impl(std::make_unique<Impl>(presentationParameters))
{
}

GameHostX11::~GameHostX11() = default;

void GameHostX11::Run(Game & game)
{
    POMDOG_ASSERT(impl);
    impl->Run(game);
}

void GameHostX11::Exit()
{
    POMDOG_ASSERT(impl);
    impl->Exit();
}

std::shared_ptr<GameWindow> GameHostX11::GetWindow()
{
    POMDOG_ASSERT(impl);
    return impl->window;
}

std::shared_ptr<GameClock> GameHostX11::GetClock()
{
    POMDOG_ASSERT(impl);
    auto gameHost = shared_from_this();
    std::shared_ptr<GameClock> sharedClock(gameHost, &impl->clock);
    return sharedClock;
}

std::shared_ptr<GraphicsDevice> GameHostX11::GetGraphicsDevice()
{
    POMDOG_ASSERT(impl);
    return impl->graphicsDevice;
}

std::shared_ptr<GraphicsCommandQueue> GameHostX11::GetGraphicsCommandQueue()
{
    POMDOG_ASSERT(impl);
    return impl->graphicsCommandQueue;
}

std::shared_ptr<AudioEngine> GameHostX11::GetAudioEngine()
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->audioEngine);
    return impl->audioEngine;
}

std::shared_ptr<AssetManager> GameHostX11::GetAssetManager()
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->assetManager);
    auto gameHost = shared_from_this();
    std::shared_ptr<AssetManager> sharedAssetManager(
        gameHost, impl->assetManager.get());
    return sharedAssetManager;
}

std::shared_ptr<Keyboard> GameHostX11::GetKeyboard()
{
    POMDOG_ASSERT(impl);
    auto gameHost = shared_from_this();
    std::shared_ptr<Keyboard> sharedKeyboard(
        gameHost, impl->keyboard.get());
    return sharedKeyboard;
}

std::shared_ptr<Mouse> GameHostX11::GetMouse()
{
    POMDOG_ASSERT(impl);
    auto gameHost = shared_from_this();
    std::shared_ptr<Mouse> sharedMouse(gameHost, &impl->mouse);
    return sharedMouse;
}

std::shared_ptr<Gamepad> GameHostX11::GetGamepad()
{
    POMDOG_ASSERT(impl);
    auto gameHost = shared_from_this();
    std::shared_ptr<Gamepad> sharedGamepad(gameHost, impl->gamepad.get());
    return sharedGamepad;
}

} // namespace X11
} // namespace Detail
} // namespace Pomdog
