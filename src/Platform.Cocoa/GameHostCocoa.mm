// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "GameHostCocoa.hpp"
#include "GameWindowCocoa.hpp"
#include "OpenGLContextCocoa.hpp"
#include "KeyboardCocoa.hpp"
#include "MouseCocoa.hpp"
#include "../RenderSystem.GL4/GraphicsContextGL4.hpp"
#include "../RenderSystem.GL4/GraphicsDeviceGL4.hpp"
#include "Pomdog/Application/Game.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Audio/AudioEngine.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Event/Event.hpp"
#include "Pomdog/Event/ScopedConnection.hpp"
#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Input/KeyState.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/StringFormat.hpp"
#include <OpenGL/OpenGL.h>
#include <utility>
#include <vector>
#include <mutex>
#include <thread>

namespace Pomdog {
namespace Detail {
namespace Cocoa {
namespace {

//-----------------------------------------------------------------------
#pragma mark - OpenGL Helper Functions
//-----------------------------------------------------------------------
static NSOpenGLPixelFormat* CreatePixelFormat(PresentationParameters const& presentationParameters)
{
	std::vector<NSOpenGLPixelFormatAttribute> attributes =
	{
		NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFANoRecovery,
		NSOpenGLPFAAllowOfflineRenderers,
	};

	///@todo Not implemented
//	if (presentationParameters.MultiSampleCount > 0) {
//		attributes.push_back(NSOpenGLPFAMultisample);
//		attributes.push_back(NSOpenGLPFASampleBuffers);
//		attributes.push_back(1);
//		attributes.push_back(NSOpenGLPFASamples);
//		attributes.push_back(presentationParameters.MultiSampleCount);
//	}

	switch (presentationParameters.SurfaceFormat) {
	case SurfaceFormat::R8G8B8A8_UNorm:
		attributes.push_back(NSOpenGLPFAColorSize);
		attributes.push_back(24);
		attributes.push_back(NSOpenGLPFAAlphaSize);
		attributes.push_back(8);
		break;
	case SurfaceFormat::R16G16B16A16_Float:
		attributes.push_back(NSOpenGLPFAColorSize);
		attributes.push_back(48);
		attributes.push_back(NSOpenGLPFAAlphaSize);
		attributes.push_back(16);
		break;
	case SurfaceFormat::R32G32B32A32_Float:
		attributes.push_back(NSOpenGLPFAColorSize);
		attributes.push_back(96);
		attributes.push_back(NSOpenGLPFAAlphaSize);
		attributes.push_back(32);
		break;
	default:
		attributes.push_back(NSOpenGLPFAColorSize);
		attributes.push_back(24);
		attributes.push_back(NSOpenGLPFAAlphaSize);
		attributes.push_back(8);
		break;
	}

	switch (presentationParameters.DepthFormat) {
	case DepthFormat::Depth16:
		attributes.push_back(NSOpenGLPFADepthSize);
		attributes.push_back(16);
		break;
	case DepthFormat::Depth24Stencil8:
		attributes.push_back(NSOpenGLPFADepthSize);
		attributes.push_back(24);
		attributes.push_back(NSOpenGLPFAStencilSize);
		attributes.push_back(8);
		break;
	case DepthFormat::Depth32:
		attributes.push_back(NSOpenGLPFADepthSize);
		attributes.push_back(32);
		break;
	case DepthFormat::None:
		break;
	}

	attributes.push_back(0);
	return [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes.data()];
}
//-----------------------------------------------------------------------
static std::shared_ptr<OpenGLContextCocoa> CreateOpenGLContext(
	PresentationParameters const& presentationParameters)
{
	auto pixelFormat = CreatePixelFormat(presentationParameters);
	return std::make_shared<OpenGLContextCocoa>(pixelFormat);
}
//-----------------------------------------------------------------------
static std::shared_ptr<GraphicsContext> CreateGraphicsContext(
	std::shared_ptr<OpenGLContextCocoa> const& openGLContext,
	std::weak_ptr<GameWindow> && gameWindow,
	PresentationParameters const& presentationParameters,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	POMDOG_ASSERT(openGLContext);
	POMDOG_ASSERT(!gameWindow.expired());
	using RenderSystem::GL4::GraphicsContextGL4;

	auto nativeContext = std::make_unique<GraphicsContextGL4>(openGLContext, std::move(gameWindow));
	return std::make_shared<GraphicsContext>(std::move(nativeContext), presentationParameters, graphicsDevice);
}

}// unnamed namespace
//-----------------------------------------------------------------------
#pragma mark - GameHostCocoa::Impl
//-----------------------------------------------------------------------
class GameHostCocoa::Impl final {
public:
	Impl(std::shared_ptr<GameWindowCocoa> const& window,
		std::shared_ptr<SystemEventDispatcher> const& dipatcher,
		PresentationParameters const& presentationParameters);

	~Impl();

	void Run(Game & game);

	void Exit();

	std::shared_ptr<Pomdog::GameWindow> GetWindow();

	std::shared_ptr<Pomdog::GameClock> GetClock(std::shared_ptr<GameHost> && gameHost);

	std::shared_ptr<Pomdog::GraphicsContext> GetGraphicsContext();

	std::shared_ptr<Pomdog::GraphicsDevice> GetGraphicsDevice();

	std::shared_ptr<Pomdog::AssetManager> GetAssetManager(std::shared_ptr<GameHost> && gameHost);

	std::shared_ptr<Pomdog::AudioEngine> GetAudioEngine();

	std::shared_ptr<Pomdog::Keyboard> GetKeyboard();

	std::shared_ptr<Pomdog::Mouse> GetMouse();

private:
	void RenderFrame(Game & game);

	void DoEvents();

	void ProcessSystemEvents(Event const& event);

	void ClientSizeChanged();

private:
	GameClock clock;
	ScopedConnection systemEventConnection;
	std::mutex renderMutex;
	std::atomic_bool viewLiveResizing;

	//std::weak_ptr<Game> game;
	std::shared_ptr<SystemEventDispatcher> systemEventDispatcher;
	std::shared_ptr<GameWindowCocoa> window;
	std::shared_ptr<OpenGLContextCocoa> openGLContext;
	std::shared_ptr<Pomdog::GraphicsDevice> graphicsDevice;
	std::shared_ptr<Pomdog::GraphicsContext> graphicsContext;
	std::shared_ptr<Pomdog::AudioEngine> audioEngine;
	std::unique_ptr<Pomdog::AssetManager> assetManager;
	std::shared_ptr<KeyboardCocoa> keyboard;
	std::shared_ptr<MouseCocoa> mouse;

	Duration presentationInterval;
	bool exitRequest;
};
//-----------------------------------------------------------------------
GameHostCocoa::Impl::Impl(std::shared_ptr<GameWindowCocoa> const& windowIn,
	std::shared_ptr<SystemEventDispatcher> const& eventDispatcher,
	PresentationParameters const& presentationParameters)
	: viewLiveResizing(false)
	, systemEventDispatcher(eventDispatcher)
	, window(windowIn)
	, presentationInterval(Duration(1) / 60)
	, exitRequest(false)
{
	openGLContext = CreateOpenGLContext(presentationParameters);

	using Detail::RenderSystem::GL4::GraphicsDeviceGL4;
	graphicsDevice = std::make_shared<Pomdog::GraphicsDevice>(std::make_unique<GraphicsDeviceGL4>());

	graphicsContext = CreateGraphicsContext(openGLContext, window, presentationParameters, graphicsDevice);

	audioEngine = std::make_shared<Pomdog::AudioEngine>();

	POMDOG_ASSERT(window);
	window->ResetGLContext(openGLContext);

	POMDOG_ASSERT(systemEventDispatcher);
	systemEventConnection = systemEventDispatcher->Connect([this](Event const& event){
		ProcessSystemEvents(event);
	});

	keyboard = std::make_shared<KeyboardCocoa>();
	mouse = std::make_shared<MouseCocoa>();
	window->BindToDelegate(mouse);

	{
		NSString* path = [[NSBundle mainBundle] resourcePath];
		std::string rootDirectory = [[path stringByAppendingPathComponent: @"Content"] UTF8String];
		Detail::AssetLoaderContext loaderContext{rootDirectory, graphicsDevice};

		assetManager = std::make_unique<Pomdog::AssetManager>(std::move(loaderContext));
	}

	POMDOG_ASSERT(presentationParameters.PresentationInterval > 0);
	presentationInterval = Duration(1) / presentationParameters.PresentationInterval;
}
//-----------------------------------------------------------------------
GameHostCocoa::Impl::~Impl()
{
	systemEventConnection.Disconnect();
	assetManager.reset();
	keyboard.reset();
	mouse.reset();
	audioEngine.reset();
	graphicsContext.reset();
	graphicsDevice.reset();
	openGLContext.reset();
	window.reset();
	systemEventDispatcher.reset();
}
//-----------------------------------------------------------------------
void GameHostCocoa::Impl::Run(Game & game)
{
	//openGLContext->Lock();
	openGLContext->MakeCurrent();
	game.Initialize();
	//openGLContext->Unlock();

	if (!game.CompleteInitialize()) {
		return;
	}

	window->SetRenderCallbackOnLiveResizing([&] {
		std::lock_guard<std::mutex> lock(renderMutex);
		ClientSizeChanged();
		RenderFrame(game);
	});

	while (!exitRequest)
	{
		std::lock_guard<std::mutex> lock(renderMutex);

		clock.Tick();
		DoEvents();
		game.Update();

		if (!viewLiveResizing.load()) {
			RenderFrame(game);
		}

		auto elapsedTime = clock.ElapsedTime();

		if (elapsedTime < presentationInterval) {
			lock.~lock_guard();
			auto sleepTime = (presentationInterval - elapsedTime);
			std::this_thread::sleep_for(sleepTime);
		}
	}

	window->SetRenderCallbackOnLiveResizing();

	window->Close();
}
//-----------------------------------------------------------------------
void GameHostCocoa::Impl::Exit()
{
	exitRequest = true;
}
//-----------------------------------------------------------------------
void GameHostCocoa::Impl::RenderFrame(Game & game)
{
	POMDOG_ASSERT(window);

	bool skipRender = (!window || window->IsMinimized()
		|| [NSApp isHidden] == YES);

	if (skipRender) {
		return;
	}

	openGLContext->Lock();
	openGLContext->MakeCurrent();

	game.Draw();

	openGLContext->Unlock();
}
//-----------------------------------------------------------------------
void GameHostCocoa::Impl::DoEvents()
{
	systemEventDispatcher->Tick();
}
//-----------------------------------------------------------------------
void GameHostCocoa::Impl::ProcessSystemEvents(Event const& event)
{
	if (event.Is<WindowShouldCloseEvent>())
	{
		Log::Internal("WindowShouldCloseEvent");
		this->Exit();
	}
	else if (event.Is<WindowWillCloseEvent>())
	{
		Log::Internal("WindowWillCloseEvent");
		///@todo Not implemented
	}
	else if (event.Is<ViewWillStartLiveResizeEvent>())
	{
		viewLiveResizing = true;

		auto rect = window->ClientBounds();
		Log::Internal(StringFormat("ViewWillStartLiveResizeEvent: {w: %d, h: %d}",
			rect.Width, rect.Height));
	}
	else if (event.Is<ViewDidEndLiveResizeEvent>())
	{
		viewLiveResizing = false;

		auto rect = window->ClientBounds();
		Log::Internal(StringFormat("ViewDidEndLiveResizeEvent: {w: %d, h: %d}",
			rect.Width, rect.Height));
	}
	else if (auto keyDownEvent = event.As<InputKeyDownEvent>())
	{
		//Log::Internal(StringFormat("KeyDown: %d", static_cast<int>(keyDownEvent->Key)));

		POMDOG_ASSERT(keyboard);
		keyboard->SetKey(keyDownEvent->Key, KeyState::Down);
	}
	else if (auto keyUpEvent = event.As<InputKeyUpEvent>())
	{
		//Log::Internal(StringFormat("KeyUp: %d", static_cast<int>(keyUpEvent->Key)));

		POMDOG_ASSERT(keyboard);
		keyboard->SetKey(keyUpEvent->Key, KeyState::Up);
	}
}
//-----------------------------------------------------------------------
void GameHostCocoa::Impl::ClientSizeChanged()
{
	openGLContext->Lock();
	openGLContext->MakeCurrent();
	{
		POMDOG_ASSERT(openGLContext->NativeOpenGLContext() != nil);
		[openGLContext->NativeOpenGLContext() update];

		auto bounds = window->ClientBounds();
		window->ClientSizeChanged(bounds.Width, bounds.Height);
	}
	openGLContext->Unlock();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameWindow> GameHostCocoa::Impl::GetWindow()
{
	return window;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameClock> GameHostCocoa::Impl::GetClock(std::shared_ptr<GameHost> && gameHost)
{
	std::shared_ptr<Pomdog::GameClock> sharedClock(gameHost, &clock);
	return std::move(sharedClock);
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsContext> GameHostCocoa::Impl::GetGraphicsContext()
{
	return graphicsContext;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsDevice> GameHostCocoa::Impl::GetGraphicsDevice()
{
	return graphicsDevice;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AudioEngine> GameHostCocoa::Impl::GetAudioEngine()
{
	return audioEngine;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AssetManager> GameHostCocoa::Impl::GetAssetManager(std::shared_ptr<GameHost> && gameHost)
{
	std::shared_ptr<Pomdog::AssetManager> sharedAssetManager(gameHost, assetManager.get());
	return std::move(sharedAssetManager);
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Keyboard> GameHostCocoa::Impl::GetKeyboard()
{
	return keyboard;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Mouse> GameHostCocoa::Impl::GetMouse()
{
	return mouse;
}
//-----------------------------------------------------------------------
#pragma mark - GameHostCocoa
//-----------------------------------------------------------------------
GameHostCocoa::GameHostCocoa(std::shared_ptr<GameWindowCocoa> const& window,
	std::shared_ptr<SystemEventDispatcher> const& eventDispatcher,
	PresentationParameters const& presentationParameters)
	: impl(std::make_unique<Impl>(window, eventDispatcher, presentationParameters))
{}
//-----------------------------------------------------------------------
GameHostCocoa::~GameHostCocoa() = default;
//-----------------------------------------------------------------------
void GameHostCocoa::Run(Game & game)
{
	POMDOG_ASSERT(impl);
	impl->Run(game);
}
//-----------------------------------------------------------------------
void GameHostCocoa::Exit()
{
	POMDOG_ASSERT(impl);
	impl->Exit();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameWindow> GameHostCocoa::Window()
{
	POMDOG_ASSERT(impl);
	return impl->GetWindow();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameClock> GameHostCocoa::Clock()
{
	POMDOG_ASSERT(impl);
	return impl->GetClock(shared_from_this());
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsContext> GameHostCocoa::GraphicsContext()
{
	POMDOG_ASSERT(impl);
	return impl->GetGraphicsContext();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsDevice> GameHostCocoa::GraphicsDevice()
{
	POMDOG_ASSERT(impl);
	return impl->GetGraphicsDevice();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AudioEngine> GameHostCocoa::AudioEngine()
{
	POMDOG_ASSERT(impl);
	return impl->GetAudioEngine();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AssetManager> GameHostCocoa::AssetManager()
{
	POMDOG_ASSERT(impl);
	return impl->GetAssetManager(shared_from_this());
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Keyboard> GameHostCocoa::Keyboard()
{
	POMDOG_ASSERT(impl);
	return impl->GetKeyboard();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Mouse> GameHostCocoa::Mouse()
{
	POMDOG_ASSERT(impl);
	return impl->GetMouse();
}
//-----------------------------------------------------------------------
}// namespace Cocoa
}// namespace Detail
}// namespace Pomdog
