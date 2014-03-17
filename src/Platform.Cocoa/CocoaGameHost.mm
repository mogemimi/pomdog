//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaGameHost.hpp"
#include <utility>
#include <vector>
#include <OpenGL/OpenGL.h>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Event/Event.hpp>
#include <Pomdog/Event/ScopedConnection.hpp>
#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Graphics/GraphicsContext.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Application/Game.hpp>
#include <Pomdog/Content/AssetManager.hpp>

#include "CocoaGameWindow.hpp"
#include "CocoaOpenGLContext.hpp"
#include "../RenderSystem/PresentationParameters.hpp"
#include "../RenderSystem.GL4/GraphicsContextGL4.hpp"
#include "../RenderSystem.GL4/GraphicsDeviceGL4.hpp"
#include "../Utility/MakeUnique.hpp"
#include "CocoaMouse.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {
namespace {

//-----------------------------------------------------------------------
#pragma mark - OpenGL Helper Functions
//-----------------------------------------------------------------------
static NSOpenGLPixelFormat* CreatePixelFormat(DepthFormat depthFormat)
{
	std::vector<NSOpenGLPixelFormatAttribute> attributes =
	{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFANoRecovery,
		NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
	};
	
	switch (depthFormat) {
	case DepthFormat::Depth16:
		attributes.push_back(NSOpenGLPFADepthSize);
		attributes.push_back(16);
		break;
	case DepthFormat::Depth24:
		attributes.push_back(NSOpenGLPFADepthSize);
		attributes.push_back(24);
		break;
	case DepthFormat::Depth24Stencil8:
		attributes.push_back(NSOpenGLPFADepthSize);
		attributes.push_back(24);
		attributes.push_back(NSOpenGLPFAStencilSize);
		attributes.push_back(8);
		break;
	case DepthFormat::None:
		break;
	}
	
	attributes.push_back(0);
	return [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes.data()];
}
//-----------------------------------------------------------------------
static std::shared_ptr<CocoaOpenGLContext> CreateOpenGLContext(DepthFormat depthFormat)
{
	NSOpenGLPixelFormat* pixelFormat = CreatePixelFormat(depthFormat);
	return std::make_shared<CocoaOpenGLContext>(pixelFormat);
}
//-----------------------------------------------------------------------
static std::shared_ptr<GraphicsContext> CreateGraphicsContext(
	std::shared_ptr<CocoaOpenGLContext> openGLContext, std::weak_ptr<GameWindow> gameWindow,
	PresentationParameters const& presentationParameters,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	POMDOG_ASSERT(openGLContext);
	using RenderSystem::GL4::GraphicsContextGL4;

	auto nativeContext = std::unique_ptr<GraphicsContextGL4>(new GraphicsContextGL4(
		std::move(openGLContext), std::move(gameWindow)));
			
	return std::make_shared<GraphicsContext>(std::move(nativeContext), presentationParameters, graphicsDevice);
}

}// unnamed namespace
//-----------------------------------------------------------------------
#pragma mark - CocoaGameHost::Impl
//-----------------------------------------------------------------------
class CocoaGameHost::Impl final {
public:
	Impl(std::shared_ptr<CocoaGameWindow> const& window,
		std::shared_ptr<SystemEventDispatcher> const& dipatcher,
		PresentationParameters const& presentationParameters);

	~Impl() = default;

	///@copydoc GameHost
	void Run(std::weak_ptr<Game> game);
	
	///@copydoc GameHost
	void Exit();

	///@copydoc GameHost
	std::shared_ptr<Pomdog::GameWindow> Window();
	
	///@copydoc GameHost
	std::shared_ptr<Pomdog::GraphicsContext> GraphicsContext();
	
	///@copydoc GameHost
	std::shared_ptr<Pomdog::GraphicsDevice> GraphicsDevice();
	
	///@copydoc GameHost
	std::shared_ptr<Pomdog::AssetManager> AssetManager(std::shared_ptr<GameHost> && gameHost);
	
	///@copydoc GameHost
	std::shared_ptr<Pomdog::Mouse> Mouse();

public:
	void RenderFrame(Game & game);
	
	void DoEvents();
	
	void ProcessSystemEvents(Event const& event);

private:
	//std::weak_ptr<Game> game;
	std::shared_ptr<CocoaGameWindow> gameWindow;

	std::shared_ptr<SystemEventDispatcher> systemEventDispatcher;
	ScopedConnection systemEventConnection;
	
	std::shared_ptr<CocoaOpenGLContext> openGLContext;
	std::shared_ptr<Pomdog::GraphicsContext> graphicsContext;
	std::shared_ptr<Pomdog::GraphicsDevice> graphicsDevice;
	std::unique_ptr<Pomdog::AssetManager> assetManager;
	
	std::shared_ptr<CocoaMouse> mouse;
	
	bool exitRequest;
};
//-----------------------------------------------------------------------
CocoaGameHost::Impl::Impl(std::shared_ptr<CocoaGameWindow> const& window,
	std::shared_ptr<SystemEventDispatcher> const& eventDispatcher,
	PresentationParameters const& presentationParameters)
	: gameWindow(window)
	, systemEventDispatcher(eventDispatcher)
	, exitRequest(false)
{
	using Details::RenderSystem::GL4::GraphicsDeviceGL4;

	openGLContext = CreateOpenGLContext(presentationParameters.DepthFormat);
	{
		GLint const swapInterval = 1;
		[openGLContext->GetNSOpenGLContext() setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
	}
	
	graphicsDevice = std::make_shared<Pomdog::GraphicsDevice>(MakeUnique<GraphicsDeviceGL4>());

	graphicsContext = CreateGraphicsContext(openGLContext, gameWindow, presentationParameters, graphicsDevice);
	
	POMDOG_ASSERT(gameWindow);
	gameWindow->ResetGLContext(openGLContext);
	
	POMDOG_ASSERT(systemEventDispatcher);
	systemEventConnection = systemEventDispatcher->Connect([this](Event const& event){
		ProcessSystemEvents(event);
	});
	
	mouse = std::make_shared<CocoaMouse>();
	gameWindow->BindToDelegate(mouse);
	
	{
		NSString* path = [[NSBundle mainBundle] resourcePath];
		Details::AssetLoaderContext loaderContext{
			[[path stringByAppendingPathComponent: @"Content"] UTF8String],
			graphicsContext,
			graphicsDevice
		};
		assetManager = MakeUnique<Pomdog::AssetManager>(std::move(loaderContext));
	}
}
//-----------------------------------------------------------------------
void CocoaGameHost::Impl::Run(std::weak_ptr<Game> weakGame)
{
	auto game = weakGame.lock();
	
	if (!game) {
		return;
	}
	
	///@note make current context on this thread
	//openGLContext->BindCurrentContext();// badcode
	game->Initialize();

	if (!game->CompleteInitialize()) {
		return;
	}

	while (!exitRequest)
	{
		DoEvents();
		game->Update();
		RenderFrame(*game);
	}

	gameWindow->Close();
	//DoEvents();
}
//-----------------------------------------------------------------------
void CocoaGameHost::Impl::Exit()
{
	exitRequest = true;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameWindow> CocoaGameHost::Impl::Window()
{
	return gameWindow;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsContext> CocoaGameHost::Impl::GraphicsContext()
{
	return graphicsContext;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsDevice> CocoaGameHost::Impl::GraphicsDevice()
{
	return graphicsDevice;
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AssetManager> CocoaGameHost::Impl::AssetManager(std::shared_ptr<GameHost> && gameHost)
{
	std::shared_ptr<Pomdog::AssetManager> sharedAssetManager(gameHost, assetManager.get());
	return std::move(sharedAssetManager);
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Mouse> CocoaGameHost::Impl::Mouse()
{
	return mouse;
}
//-----------------------------------------------------------------------
void CocoaGameHost::Impl::RenderFrame(Game & game)
{
	POMDOG_ASSERT(gameWindow);

	if (!gameWindow || gameWindow->IsMinimized()) {
		// skip rendering
		return;
	}
	
	// RenderBegin:
	openGLContext->LockContext();
	openGLContext->BindCurrentContext();

	// Render:
	game.Draw();
	
	// RenderEnd:
	openGLContext->UnlockContext();
}
//-----------------------------------------------------------------------
void CocoaGameHost::Impl::DoEvents()
{
	systemEventDispatcher->Tick();
}
//-----------------------------------------------------------------------
void CocoaGameHost::Impl::ProcessSystemEvents(Event const& event)
{
	if (event.Is<WindowShouldCloseEvent>()) {
		Log::LogMessage("WindowShouldCloseEvent", LoggingLevel::Internal);
		this->Exit();
	}
	else if (event.Is<WindowWillCloseEvent>()) {
		Log::LogMessage("WindowWillCloseEvent", LoggingLevel::Internal);
		///@todo Not implemented
	}
}
//-----------------------------------------------------------------------
#pragma mark - CocoaGameHost
//-----------------------------------------------------------------------
CocoaGameHost::CocoaGameHost(std::shared_ptr<CocoaGameWindow> const& window,
	std::shared_ptr<SystemEventDispatcher> const& eventDispatcher,
	PresentationParameters const& presentationParameters)
	: impl(MakeUnique<Impl>(window, eventDispatcher, presentationParameters))
{}
//-----------------------------------------------------------------------
CocoaGameHost::~CocoaGameHost() = default;
//-----------------------------------------------------------------------
void CocoaGameHost::Run(std::weak_ptr<Game> weakGame)
{
	POMDOG_ASSERT(impl);
	impl->Run(weakGame);
}
//-----------------------------------------------------------------------
void CocoaGameHost::Exit()
{
	POMDOG_ASSERT(impl);
	impl->Exit();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GameWindow> CocoaGameHost::Window()
{
	POMDOG_ASSERT(impl);
	return impl->Window();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsContext> CocoaGameHost::GraphicsContext()
{
	POMDOG_ASSERT(impl);
	return impl->GraphicsContext();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::GraphicsDevice> CocoaGameHost::GraphicsDevice()
{
	POMDOG_ASSERT(impl);
	return impl->GraphicsDevice();
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::AssetManager> CocoaGameHost::AssetManager()
{
	POMDOG_ASSERT(impl);
	return impl->AssetManager(shared_from_this());
}
//-----------------------------------------------------------------------
std::shared_ptr<Pomdog::Mouse> CocoaGameHost::Mouse()
{
	POMDOG_ASSERT(impl);
	return impl->Mouse();
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
