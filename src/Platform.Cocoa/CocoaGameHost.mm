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

#include "CocoaGameWindow.hpp"
#include "CocoaOpenGLContext.hpp"
#include "../RenderSystem/PresentationParameters.hpp"
#include "../RenderSystem.GL4/GraphicsContextGL4.hpp"
#include "../RenderSystem.GL4/GraphicsDeviceGL4.hpp"
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
using RenderSystem::PresentationParameters;

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
class CocoaGameHost::Impl final: public GameHost
{
public:
	Impl(std::shared_ptr<CocoaGameWindow> const& window,
		std::shared_ptr<SystemEventDispatcher> const& dipatcher);

	~Impl() = default;

	///@copydoc GameHost
	void Run(std::weak_ptr<Game> game) override;
	
	///@copydoc GameHost
	void Exit() override;

	///@copydoc GameHost
	std::shared_ptr<GameWindow> GetWindow() override;
	
	///@copydoc GameHost
	std::shared_ptr<GraphicsContext> GetGraphicsContext();
	
	///@copydoc GameHost
	std::shared_ptr<GraphicsDevice> GetGraphicsDevice() override;

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
	std::shared_ptr<GraphicsContext> graphicsContext;
	std::shared_ptr<GraphicsDevice> graphicsDevice;
	
	std::shared_ptr<CocoaMouse> mouse;
	
	bool exitRequest;
};
//-----------------------------------------------------------------------
CocoaGameHost::Impl::Impl(std::shared_ptr<CocoaGameWindow> const& window,
	std::shared_ptr<SystemEventDispatcher> const& eventDispatcher)
	: gameWindow(window)
	, systemEventDispatcher(eventDispatcher)
	, exitRequest(false)
{
	using Details::RenderSystem::GL4::GraphicsDeviceGL4;

	PresentationParameters presentationParameters;
	presentationParameters.DepthFormat = DepthFormat::Depth24Stencil8;
	presentationParameters.BackBufferWidth = gameWindow->GetClientBounds().width;
	presentationParameters.BackBufferHeight = gameWindow->GetClientBounds().height;
	presentationParameters.IsFullScreen = false; ///@todo Not implemented.

	openGLContext = CreateOpenGLContext(presentationParameters.DepthFormat);
	{
		GLint const swapInterval = 1;
		[openGLContext->GetNSOpenGLContext() setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
	}
	
	graphicsDevice = std::make_shared<GraphicsDevice>(std::unique_ptr<GraphicsDeviceGL4>{new GraphicsDeviceGL4()});

	graphicsContext = CreateGraphicsContext(openGLContext, gameWindow, presentationParameters, graphicsDevice);
	
	POMDOG_ASSERT(gameWindow);
	gameWindow->ResetGLContext(openGLContext);
	
	POMDOG_ASSERT(systemEventDispatcher);
	systemEventConnection = systemEventDispatcher->Connect([this](Event const& event){
		ProcessSystemEvents(event);
	});
	
	mouse = std::make_shared<CocoaMouse>();
	gameWindow->BindToDelegate(mouse);
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
std::shared_ptr<GameWindow> CocoaGameHost::Impl::GetWindow()
{
	return gameWindow;
}
//-----------------------------------------------------------------------
std::shared_ptr<GraphicsContext> CocoaGameHost::Impl::GetGraphicsContext()
{
	return graphicsContext;
}
//-----------------------------------------------------------------------
std::shared_ptr<GraphicsDevice> CocoaGameHost::Impl::GetGraphicsDevice()
{
	return graphicsDevice;
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
	std::shared_ptr<SystemEventDispatcher> const& eventDispatcher)
	: impl(new Impl(window, eventDispatcher))
{}
//-----------------------------------------------------------------------
CocoaGameHost::~CocoaGameHost()
{}
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
std::shared_ptr<GameWindow> CocoaGameHost::GetWindow()
{
	POMDOG_ASSERT(impl);
	return impl->GetWindow();
}
//-----------------------------------------------------------------------
std::shared_ptr<GraphicsContext> CocoaGameHost::GetGraphicsContext()
{
	POMDOG_ASSERT(impl);
	return impl->GetGraphicsContext();
}
//-----------------------------------------------------------------------
std::shared_ptr<GraphicsDevice> CocoaGameHost::GetGraphicsDevice()
{
	POMDOG_ASSERT(impl);
	return impl->GetGraphicsDevice();
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
