// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/win32/game_host_win32.h"
#include "pomdog/application/game_host_options.h"
#include "pomdog/application/win32/game_window_win32.h"
#include "pomdog/application/win32/precise_sleeper_win32.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/input/backends/keyboard_impl.h"
#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/directinput/gamepad_directinput.h"
#include "pomdog/input/gamepad_service.h"
#include "pomdog/input/player_index.h"
#include "pomdog/input/win32/keyboard_win32.h"
#include "pomdog/input/win32/mouse_win32.h"
#if defined(POMDOG_USE_GL4)
#include "pomdog/application/win32/opengl_context_win32.h"
#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/gl4/graphics_context_gl4.h"
#include "pomdog/gpu/gl4/graphics_device_gl4.h"
#endif
#if defined(POMDOG_USE_DIRECT3D11)
#include "pomdog/gpu/backends/command_queue_immediate.h"
#include "pomdog/gpu/direct3d11/graphics_context_direct3d11.h"
#include "pomdog/gpu/direct3d11/graphics_device_direct3d11.h"
#endif
#if defined(POMDOG_USE_VULKAN)
#include "pomdog/gpu/vulkan/command_queue_vulkan.h"
#include "pomdog/gpu/vulkan/graphics_device_vulkan.h"
#include "pomdog/gpu/vulkan/swap_chain_vulkan.h"
#include <vulkan/vulkan_win32.h>
#endif
#include "pomdog/application/backends/subsystem_scheduler.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/application/game.h"
#include "pomdog/audio/xaudio2/audio_engine_xaudio2.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/chrono/win32/time_source_win32.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/gpu/command_queue.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/logging/log.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/network/http_client.h"
#include "pomdog/network/io_service.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::win32::GameWindowWin32;
using pomdog::detail::xaudio2::AudioEngineXAudio2;
#if defined(POMDOG_USE_GL4)
using pomdog::gpu::detail::gl4::GraphicsContextGL4;
using pomdog::gpu::detail::gl4::GraphicsDeviceGL4;
#endif
#if defined(POMDOG_USE_DIRECT3D11)
using pomdog::gpu::detail::direct3d11::GraphicsContextDirect3D11;
using pomdog::gpu::detail::direct3d11::GraphicsDeviceDirect3D11;
#endif
#if defined(POMDOG_USE_VULKAN)
using pomdog::gpu::detail::vulkan::CommandQueueVulkan;
using pomdog::gpu::detail::vulkan::GraphicsDeviceVulkan;
using pomdog::gpu::detail::vulkan::SwapChainVulkan;
#endif

namespace pomdog::detail::win32 {
namespace {

void messagePump()
{
    MSG msg;
    while (0 != ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

class GraphicsBridgeWin32 {
public:
    virtual ~GraphicsBridgeWin32() = default;

    virtual void
    onClientSizeChanged(int width, int height) = 0;

    virtual void
    shutdown() = 0;

    [[nodiscard]] virtual bool
    getDisplaySyncEnabled() const noexcept = 0;

    virtual void
    setDisplaySyncEnabled(bool enabled) noexcept = 0;
};

using CreateGraphicsDeviceResult = std::tuple<
    std::shared_ptr<gpu::GraphicsDevice>,
    std::shared_ptr<gpu::CommandQueue>,
    std::unique_ptr<GraphicsBridgeWin32>,
    std::unique_ptr<Error>>;

#if defined(POMDOG_USE_GL4)

class GraphicsBridgeWin32GL4 final : public GraphicsBridgeWin32 {
private:
    std::shared_ptr<win32::OpenGLContextWin32> openGLContext_;
    std::shared_ptr<GraphicsDeviceGL4> graphicsDevice_;
    std::shared_ptr<gpu::detail::CommandQueueImmediate> commandQueue_;

public:
    GraphicsBridgeWin32GL4(
        const std::shared_ptr<win32::OpenGLContextWin32>& openGLContextIn,
        const std::shared_ptr<GraphicsDeviceGL4>& graphicsDeviceIn,
        const std::shared_ptr<gpu::detail::CommandQueueImmediate>& commandQueueIn)
        : openGLContext_(openGLContextIn)
        , graphicsDevice_(graphicsDeviceIn)
        , commandQueue_(commandQueueIn)
    {
    }

    void onClientSizeChanged(int width, int height) override
    {
        POMDOG_ASSERT(graphicsDevice_);
        graphicsDevice_->clientSizeChanged(width, height);
    }

    void shutdown() override
    {
        if (commandQueue_ != nullptr) {
            commandQueue_->reset();
        }

        commandQueue_.reset();
        graphicsDevice_.reset();
        openGLContext_.reset();
    }

    [[nodiscard]] bool
    getDisplaySyncEnabled() const noexcept override
    {
        POMDOG_ASSERT(openGLContext_ != nullptr);
        return openGLContext_->getSwapInterval() != 0;
    }

    void
    setDisplaySyncEnabled(bool enabled) noexcept override
    {
        POMDOG_ASSERT(openGLContext_ != nullptr);
        openGLContext_->setSwapInterval(enabled ? 1 : 0);
    }
};

[[nodiscard]] CreateGraphicsDeviceResult
CreateGraphicsDeviceGL4(
    const std::shared_ptr<GameWindowWin32>& window,
    const gpu::PresentationParameters& presentationParameters)
{
    // NOTE: Create an OpenGL context.
    auto [openGLContext, glErr] = win32::OpenGLContextWin32::create(
        window->getNativeWindowHandle(),
        presentationParameters);
    if (glErr != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            errors::wrap(std::move(glErr), "OpenGLContextWin32::create() failed."));
    }

    if (::glewInit() != GLEW_OK) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            errors::make("glewInit() failed."));
    }

    openGLContext->makeCurrent();

    // NOTE: Create a graphics device.
    auto graphicsDevice = std::make_shared<GraphicsDeviceGL4>();
    if (auto err = graphicsDevice->initialize(presentationParameters); err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            errors::wrap(std::move(err), "GraphicsDeviceGL4::Initialize() failed."));
    }

    // NOTE: Create a graphics context.
    auto graphicsContext = std::make_shared<GraphicsContextGL4>();
    if (auto err = graphicsContext->initialize(openGLContext, graphicsDevice); err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            errors::wrap(std::move(err), "GraphicsContextGL4::Initialize() failed."));
    }

    auto graphicsCommandQueue = std::make_shared<gpu::detail::CommandQueueImmediate>(graphicsContext);

    POMDOG_ASSERT(graphicsDevice);
    POMDOG_ASSERT(graphicsContext);
    POMDOG_ASSERT(graphicsCommandQueue);

    auto bridge = std::make_unique<GraphicsBridgeWin32GL4>(
        openGLContext,
        graphicsDevice,
        graphicsCommandQueue);

    return std::make_tuple(
        std::move(graphicsDevice),
        std::move(graphicsCommandQueue),
        std::move(bridge),
        nullptr);
}
#endif

#if defined(POMDOG_USE_DIRECT3D11)

class GraphicsBridgeWin32Direct3D11 final : public GraphicsBridgeWin32 {
private:
    std::shared_ptr<GraphicsDeviceDirect3D11> graphicsDevice_;
    std::shared_ptr<GraphicsContextDirect3D11> graphicsContext_;
    std::shared_ptr<gpu::detail::CommandQueueImmediate> commandQueue_;

public:
    GraphicsBridgeWin32Direct3D11(
        const std::shared_ptr<GraphicsDeviceDirect3D11>& graphicsDeviceIn,
        const std::shared_ptr<GraphicsContextDirect3D11>& graphicsContextIn,
        const std::shared_ptr<gpu::detail::CommandQueueImmediate>& commandQueueIn)
        : graphicsDevice_(graphicsDeviceIn)
        , graphicsContext_(graphicsContextIn)
        , commandQueue_(commandQueueIn)
    {
    }

    void onClientSizeChanged(int width, int height) override
    {
        POMDOG_ASSERT(graphicsDevice_);
        POMDOG_ASSERT(graphicsContext_);
        auto device = graphicsDevice_->getDevice();
        if (auto err = graphicsContext_->resizeBackBuffers(device.Get(), width, height); err != nullptr) {
            // FIXME: Add error handling
            Log::Critical("pomdog", "error: ResizeBackBuffers() failed: " + err->toString());
        }
        graphicsDevice_->clientSizeChanged(width, height);
    }

    void shutdown() override
    {
        if (commandQueue_ != nullptr) {
            commandQueue_->reset();
        }

        commandQueue_.reset();
        graphicsContext_.reset();
        graphicsDevice_.reset();
    }

    [[nodiscard]] bool
    getDisplaySyncEnabled() const noexcept override
    {
        POMDOG_ASSERT(graphicsContext_ != nullptr);
        return graphicsContext_->getDisplaySyncEnabled();
    }

    void
    setDisplaySyncEnabled(bool enabled) noexcept override
    {
        POMDOG_ASSERT(graphicsContext_ != nullptr);
        graphicsContext_->setDisplaySyncEnabled(enabled);
    }
};

[[nodiscard]] CreateGraphicsDeviceResult
CreateGraphicsDeviceDirect3D11(
    const std::shared_ptr<GameWindowWin32>& window,
    const gpu::PresentationParameters& presentationParameters)
{
    auto graphicsDevice = std::make_shared<GraphicsDeviceDirect3D11>();
    if (auto err = graphicsDevice->initialize(presentationParameters); err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            errors::wrap(std::move(err), "failed to initialize GraphicsDeviceDirect3D11"));
    }

    auto device = graphicsDevice->getDevice();
    auto dxgiFactory = std::get<0>(graphicsDevice->getDXGIFactory());

    auto graphicsContext = std::make_shared<GraphicsContextDirect3D11>();
    if (auto err = graphicsContext->initialize(window->getNativeWindowHandle(),
            dxgiFactory,
            device,
            presentationParameters);
        err != nullptr) {
        return std::make_tuple(
            nullptr,
            nullptr,
            nullptr,
            errors::wrap(std::move(err), "failed to initialize GraphicsContextDirect3D11"));
    }

    auto graphicsCommandQueue = std::make_shared<gpu::detail::CommandQueueImmediate>(graphicsContext);

    POMDOG_ASSERT(graphicsDevice);
    POMDOG_ASSERT(graphicsContext);
    POMDOG_ASSERT(graphicsCommandQueue);

    auto bridge = std::make_unique<GraphicsBridgeWin32Direct3D11>(
        graphicsDevice,
        std::move(graphicsContext),
        graphicsCommandQueue);

    return std::make_tuple(
        std::move(graphicsDevice),
        std::move(graphicsCommandQueue),
        std::move(bridge),
        nullptr);
}
#endif

#if defined(POMDOG_USE_VULKAN)

class GraphicsBridgeWin32Vulkan final : public GraphicsBridgeWin32 {
private:
    std::shared_ptr<GraphicsDeviceVulkan> graphicsDevice_;
    std::shared_ptr<CommandQueueVulkan> commandQueue_;
    std::unique_ptr<SwapChainVulkan> swapChain_;

public:
    GraphicsBridgeWin32Vulkan(
        const std::shared_ptr<GraphicsDeviceVulkan>& graphicsDeviceIn,
        const std::shared_ptr<CommandQueueVulkan>& commandQueueIn,
        std::unique_ptr<SwapChainVulkan>&& swapChainIn)
        : graphicsDevice_(graphicsDeviceIn)
        , commandQueue_(commandQueueIn)
        , swapChain_(std::move(swapChainIn))
    {
    }

    void onClientSizeChanged(int width, int height) override
    {
        if (swapChain_ != nullptr) {
            if (auto err = swapChain_->recreate(width, height); err != nullptr) {
                Log::Critical("pomdog", "error: SwapChain recreate failed: " + err->toString());
            }
        }
        if (graphicsDevice_ != nullptr) {
            auto params = graphicsDevice_->getPresentationParameters();
            params.backBufferWidth = width;
            params.backBufferHeight = height;
            graphicsDevice_->setPresentationParameters(params);
        }
    }

    void shutdown() override
    {
        if (commandQueue_ != nullptr) {
            commandQueue_->reset();
        }

        if (swapChain_ != nullptr) {
            swapChain_->shutdown();
        }

        commandQueue_.reset();
        swapChain_.reset();
        graphicsDevice_.reset();
    }

    [[nodiscard]] bool
    getDisplaySyncEnabled() const noexcept override
    {
        // NOTE: The Vulkan backend determines the present mode at swap chain creation
        // time (VK_PRESENT_MODE_MAILBOX_KHR or VK_PRESENT_MODE_FIFO_KHR).
        // Changing the present mode at runtime requires destroying and recreating the
        // swap chain, which is not yet supported.  V-Sync control is therefore not
        // available for the Vulkan backend.
        return false;
    }

    void
    setDisplaySyncEnabled([[maybe_unused]] bool enabled) noexcept override
    {
        // NOTE: Changing V-Sync for the Vulkan backend requires recreating the swap
        // chain (changing VkPresentModeKHR).  This is not yet supported at runtime.
    }
};

[[nodiscard]] CreateGraphicsDeviceResult
CreateGraphicsDeviceVulkan(
    const std::shared_ptr<GameWindowWin32>& window,
    const gpu::PresentationParameters& presentationParameters)
{
    auto [graphicsDevice, deviceErr] = GraphicsDeviceVulkan::create();
    if (deviceErr != nullptr) {
        return std::make_tuple(
            nullptr, nullptr, nullptr,
            errors::wrap(std::move(deviceErr), "failed to create GraphicsDeviceVulkan"));
    }

    // Store presentation parameters
    graphicsDevice->setPresentationParameters(presentationParameters);

    // Create Win32 Vulkan surface
    VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.hwnd = window->getNativeWindowHandle();
    surfaceInfo.hinstance = ::GetModuleHandle(nullptr);

    VkSurfaceKHR surface = nullptr;
    auto result = vkCreateWin32SurfaceKHR(
        graphicsDevice->getInstance(), &surfaceInfo, nullptr, &surface);
    if (result != VK_SUCCESS) {
        return std::make_tuple(
            nullptr, nullptr, nullptr,
            errors::make("failed to create Win32 Vulkan surface"));
    }

    // Create swap chain
    auto swapChain = std::make_unique<SwapChainVulkan>();
    if (auto err = swapChain->initialize(
            graphicsDevice->getInstance(),
            graphicsDevice->getPhysicalDevice(),
            graphicsDevice->getDevice(),
            graphicsDevice->getGraphicsQueueFamilyIndex(),
            surface,
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            presentationParameters.depthStencilFormat);
        err != nullptr) {
        // Surface ownership is not transferred yet, clean up
        vkDestroySurfaceKHR(graphicsDevice->getInstance(), surface, nullptr);
        return std::make_tuple(
            nullptr, nullptr, nullptr,
            errors::wrap(std::move(err), "failed to initialize SwapChainVulkan"));
    }

    // Set swap chain on graphics device so createCommandList() can inject it
    graphicsDevice->setSwapChain(swapChain.get());

    // Update presentation parameters to match actual swap chain format
    {
        auto params = graphicsDevice->getPresentationParameters();
        auto vkFormat = swapChain->getSurfaceFormat();
        if (vkFormat == VK_FORMAT_B8G8R8A8_UNORM) {
            params.backBufferFormat = gpu::PixelFormat::B8G8R8A8_UNorm;
        }
        else if (vkFormat == VK_FORMAT_R8G8B8A8_UNORM) {
            params.backBufferFormat = gpu::PixelFormat::R8G8B8A8_UNorm;
        }
        // NOTE: Keep the default depth/stencil format from bootstrap
        // (typically Depth24Stencil8) now that DepthStencilBufferVulkan is implemented.
        graphicsDevice->setPresentationParameters(params);
    }

    auto commandQueue = std::make_shared<CommandQueueVulkan>(
        graphicsDevice->getDevice(),
        graphicsDevice->getGraphicsQueue());
    commandQueue->setSwapChain(swapChain.get());

    auto bridge = std::make_unique<GraphicsBridgeWin32Vulkan>(
        graphicsDevice,
        commandQueue,
        std::move(swapChain));

    return std::make_tuple(
        std::move(graphicsDevice),
        std::move(commandQueue),
        std::move(bridge),
        nullptr);
}
#endif

class GameHostWin32Impl final : public GameHostWin32 {
private:
    std::shared_ptr<detail::win32::TimeSourceWin32> timeSource_;
    std::shared_ptr<GameClockImpl> clock_;
    detail::SubsystemScheduler subsystemScheduler_;

    std::shared_ptr<SystemEventQueue> eventQueue_;
    std::shared_ptr<GameWindowWin32> window_;

    std::unique_ptr<GraphicsBridgeWin32> graphicsBridge_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> graphicsCommandQueue_;
    std::shared_ptr<AudioEngineXAudio2> audioEngine_;

    std::shared_ptr<KeyboardImpl> keyboardImpl_;
    std::unique_ptr<KeyboardWin32> keyboard_;
    std::shared_ptr<MouseImpl> mouseImpl_;
    std::unique_ptr<MouseWin32> mouse_;
    std::shared_ptr<directinput::GamepadServiceDirectInput> gamepad_;

    std::unique_ptr<IOService> ioService_;
    std::unique_ptr<HTTPClient> httpClient_;

    std::thread gamepadThread_;

    std::optional<i32> maxFramesPerSecond_;
    std::optional<Duration> targetFrameDuration_;
    std::optional<std::optional<i32>> pendingMaxFPS_;
    std::optional<bool> pendingDisplaySync_;
    PreciseSleeper preciseSleeper_;
    gpu::PixelFormat backBufferSurfaceFormat_;
    gpu::PixelFormat backBufferDepthStencilFormat_;
    Rect2D lastReportedBounds_ = {0, 0, 0, 0};
    std::atomic<bool> exitRequest_ = false;
    bool displaySyncEnabled_ = false;

public:
    ~GameHostWin32Impl()
    {
        if (gamepadThread_.joinable()) {
            gamepadThread_.join();
        }

        eventQueue_.reset();
        httpClient_.reset();
        if (ioService_ != nullptr) {
            if (auto err = ioService_->shutdown(); err != nullptr) {
                // FIXME: Log the error instead of ignoring it.
            }
        }
        ioService_.reset();
        gamepad_.reset();
        keyboard_.reset();
        keyboardImpl_.reset();
        mouse_.reset();
        mouseImpl_.reset();
        audioEngine_.reset();

        if (graphicsBridge_ != nullptr) {
            graphicsBridge_->shutdown();
        }
        graphicsBridge_.reset();
        graphicsCommandQueue_.reset();
        graphicsDevice_.reset();

        window_.reset();
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<GameWindowWin32>& windowIn,
        HINSTANCE hInstance,
        const std::shared_ptr<SystemEventQueue>& eventQueueIn,
        const gpu::PresentationParameters& presentationParameters,
        const GameHostOptions& options) noexcept
    {
        eventQueue_ = eventQueueIn;
        window_ = windowIn;
        backBufferSurfaceFormat_ = presentationParameters.backBufferFormat;
        backBufferDepthStencilFormat_ = presentationParameters.depthStencilFormat;
        exitRequest_ = false;

        timeSource_ = std::make_shared<detail::win32::TimeSourceWin32>();
        clock_ = std::make_shared<GameClockImpl>();
        if (auto err = clock_->initialize(options.maxFramesPerSecond.value_or(60), timeSource_); err != nullptr) {
            return errors::wrap(std::move(err), "GameClockImpl::Initialize() failed.");
        }

        const auto graphicsBackend = options.graphicsBackend;
#if defined(POMDOG_USE_GL4)
        if (graphicsBackend == gpu::GraphicsBackend::OpenGL4) {
            auto result = CreateGraphicsDeviceGL4(window_, presentationParameters);
            graphicsDevice_ = std::move(std::get<0>(result));
            graphicsCommandQueue_ = std::move(std::get<1>(result));
            graphicsBridge_ = std::move(std::get<2>(result));

            if (auto deviceErr = std::move(std::get<3>(result)); deviceErr != nullptr) {
                return errors::wrap(std::move(deviceErr), "CreateGraphicsDeviceGL4() failed");
            }
        }
#endif
#if defined(POMDOG_USE_DIRECT3D11)
        if (graphicsBackend == gpu::GraphicsBackend::Direct3D11) {
            auto result = CreateGraphicsDeviceDirect3D11(window_, presentationParameters);
            graphicsDevice_ = std::move(std::get<0>(result));
            graphicsCommandQueue_ = std::move(std::get<1>(result));
            graphicsBridge_ = std::move(std::get<2>(result));

            if (auto deviceErr = std::move(std::get<3>(result)); deviceErr != nullptr) {
                return errors::wrap(std::move(deviceErr), "CreateGraphicsDeviceDirect3D11() failed");
            }
        }
#endif
#if defined(POMDOG_USE_VULKAN)
        if (graphicsBackend == gpu::GraphicsBackend::Vulkan) {
            auto result = CreateGraphicsDeviceVulkan(window_, presentationParameters);
            graphicsDevice_ = std::move(std::get<0>(result));
            graphicsCommandQueue_ = std::move(std::get<1>(result));
            graphicsBridge_ = std::move(std::get<2>(result));

            if (auto deviceErr = std::move(std::get<3>(result)); deviceErr != nullptr) {
                return errors::wrap(std::move(deviceErr), "CreateGraphicsDeviceVulkan() failed");
            }
        }
#endif
        if (graphicsDevice_ == nullptr || graphicsCommandQueue_ == nullptr || graphicsBridge_ == nullptr) {
            return errors::make("unsupported graphics backend");
        }

        // NOTE: Read the initial V-Sync state from the graphics bridge.
        displaySyncEnabled_ = graphicsBridge_->getDisplaySyncEnabled();

        if (options.displaySyncEnabled.has_value()) {
            pendingDisplaySync_ = *options.displaySyncEnabled;
        }
        if (options.maxFramesPerSecond != std::nullopt) {
            if (*options.maxFramesPerSecond <= 0) {
                return errors::make("maxFramesPerSecond must be > 0");
            }
            maxFramesPerSecond_ = *options.maxFramesPerSecond;
            targetFrameDuration_ = Duration(1.0) / *options.maxFramesPerSecond;
        }

        POMDOG_ASSERT(eventQueue_ != nullptr);

        // NOTE: Create audio engine (conditional).
        if (options.enableAudio) {
            audioEngine_ = std::make_shared<AudioEngineXAudio2>();
            if (auto err = audioEngine_->initialize(); err != nullptr) {
                return errors::wrap(std::move(err), "AudioEngineXAudio2::Initialize() failed");
            }
        }

        // NOTE: Create keyboard and mouse (conditional).
        if (!options.headless) {
            keyboardImpl_ = std::make_shared<KeyboardImpl>();
            keyboard_ = std::make_unique<KeyboardWin32>(keyboardImpl_);
            mouseImpl_ = std::make_shared<MouseImpl>();
            mouse_ = std::make_unique<MouseWin32>(window_->getNativeWindowHandle(), mouseImpl_);
        }

        // NOTE: Create gamepad (conditional).
        if (options.enableGamepad && !options.headless) {
            gamepad_ = std::make_shared<directinput::GamepadServiceDirectInput>();
            if (auto err = gamepad_->initialize(hInstance, window_->getNativeWindowHandle(), options.gameControllerDB); err != nullptr) {
                return errors::wrap(std::move(err), "GamepadServiceDirectInput::Initialize() failed");
            }

            gamepadThread_ = std::thread([this] {
                while (!exitRequest_) {
                    gamepad_->enumerateDevices();
                    std::this_thread::sleep_for(std::chrono::milliseconds(800));
                }
            });
        }

        // NOTE: Create IO service and HTTP client (conditional).
        if (options.enableNetwork) {
            ioService_ = std::make_unique<IOService>();
            if (auto err = ioService_->initialize(clock_); err != nullptr) {
                return errors::wrap(std::move(err), "IOService::Initialize() failed");
            }
            httpClient_ = std::make_unique<HTTPClient>(ioService_.get());
        }

        return nullptr;
    }

    void run(Game& game) override
    {
        while (!exitRequest_) {
            // NOTE: Apply pending V-Sync and frame rate changes at the start of each
            // frame boundary so that update/draw within a frame see consistent settings.
            applyPendingDisplaySettings();

            clock_->tick();
            if (keyboardImpl_) {
                keyboardImpl_->clearTextInput();
            }
            if (mouseImpl_) {
                mouseImpl_->clearScrollDelta();
            }
            messagePump();
            window_->applyPendingWindowRequests();
            doEvents();
            if (gamepad_) {
                gamepad_->pollEvents();
            }
            if (ioService_) {
                ioService_->step();
            }
            subsystemScheduler_.onUpdate();
            game.update();
            renderFrame(game);

            // NOTE: Frame rate limiting.
            // If an explicit maximum FPS is set, sleep until the target frame duration is reached.
            // If V-Sync is enabled without an explicit FPS cap, no additional sleep is needed
            // because the display hardware already synchronizes the frame rate.
            // If neither V-Sync nor an FPS cap is used, the game runs as fast as possible,
            // which may cause excessive CPU/GPU usage and increased heat generation.
            if (targetFrameDuration_ != std::nullopt) {
                POMDOG_ASSERT(*targetFrameDuration_ >= Duration::zero());

                const auto elapsedTime = clock_->getElapsedTime();
                const auto sleepTime = *targetFrameDuration_ - elapsedTime;

                preciseSleeper_.sleep(sleepTime);
            }
        }

        window_->close();
        messagePump();
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

    std::shared_ptr<gpu::CommandQueue>
    getCommandQueue() noexcept override
    {
        POMDOG_ASSERT(graphicsCommandQueue_ != nullptr);
        return graphicsCommandQueue_;
    }

    std::shared_ptr<gpu::GraphicsDevice>
    getGraphicsDevice() noexcept override
    {
        POMDOG_ASSERT(graphicsDevice_ != nullptr);
        return graphicsDevice_;
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
        if (gamepad_ == nullptr) {
            return nullptr;
        }
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
            POMDOG_ASSERT(graphicsBridge_ != nullptr);
            graphicsBridge_->setDisplaySyncEnabled(enabled);
            displaySyncEnabled_ = enabled;
            pendingDisplaySync_ = std::nullopt;
        }

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

    void renderFrame(Game& game)
    {
        POMDOG_ASSERT(window_ != nullptr);

        if (!window_ || window_->isMinimized()) {
            // skip rendering
            return;
        }

        game.draw();
    }

    void doEvents()
    {
        bool surfaceResizeRequest = false;

        eventQueue_->emit([this, &surfaceResizeRequest](SystemEvent event) {
            processSystemEvents(std::move(event), surfaceResizeRequest);
        });

        if (surfaceResizeRequest) {
            POMDOG_ASSERT(window_ != nullptr);
            const auto bounds = window_->getClientBounds();

            // NOTE: Only notify when the client area size actually changed since the
            // last report. This prevents redundant framebuffer reallocations and UI
            // layout updates when, for example, only the window mode changes without
            // affecting the size (e.g. moving between Windowed and `BorderlessWindowed`
            // at the same dimensions).
            if (bounds.width != lastReportedBounds_.width ||
                bounds.height != lastReportedBounds_.height) {
                lastReportedBounds_ = bounds;

                POMDOG_ASSERT(graphicsBridge_ != nullptr);
                graphicsBridge_->onClientSizeChanged(bounds.width, bounds.height);
                window_->clientSizeChanged(bounds.width, bounds.height);
            }
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
            surfaceResizeRequest = true;
            if (auto* e = std::get_if<WindowModeChangedEvent>(&event.data); e != nullptr) {
                window_->windowModeChanged(e->windowMode);
            }
            break;
        }
        case SystemEventKind::ViewDidEndLiveResizeEvent: {
            surfaceResizeRequest = true;
            break;
        }
        default:
            if (mouse_) {
                mouse_->handleMessage(event);
            }
            if (keyboard_) {
                keyboard_->handleMessage(event);
            }
            break;
        }
    }
};

} // namespace

GameHostWin32::GameHostWin32() = default;

GameHostWin32::~GameHostWin32() = default;

std::tuple<std::shared_ptr<GameHostWin32>, std::unique_ptr<Error>>
GameHostWin32::create(
    const std::shared_ptr<GameWindowWin32>& window,
    HINSTANCE hInstance,
    const std::shared_ptr<SystemEventQueue>& eventQueue,
    const gpu::PresentationParameters& presentationParameters,
    const GameHostOptions& options) noexcept
{
    auto host = std::make_shared<GameHostWin32Impl>();
    if (auto err = host->initialize(
            window,
            hInstance,
            eventQueue,
            presentationParameters,
            options);
        err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(host), nullptr);
}

} // namespace pomdog::detail::win32
