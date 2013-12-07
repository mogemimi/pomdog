//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaGameHost.hpp"
#include "Game.hpp"
#include <utility>
#include "CocoaGameWindow.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Event/Event.hpp>
#include <Pomdog/Event/EventCodeHelper.hpp>
#include <Pomdog/Event/ScopedConnection.hpp>
#include <Pomdog/Logging/Log.hpp>

#include <OpenGL/OpenGL.h>
#include <Pomdog/Graphics/GraphicsContext.hpp>
#include "CocoaOpenGLContext.hpp"
#include "../RenderSystem.GL4/GraphicsContextGL4.hpp"
#include "CocoaMouse.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {

static auto WindowWillCloseEvent = EventCodeHelper::CreateCode("WindowWillClose");
static auto WindowShouldCloseEvent = EventCodeHelper::CreateCode("WindowShouldClose");

//-----------------------------------------------------------------------
#pragma mark -
#pragma mark OpenGL Helper Functions
//-----------------------------------------------------------------------
static NSOpenGLPixelFormat* CreateDefaultPixelFormat()
{
	NSOpenGLPixelFormatAttribute attribute[] = {
		NSOpenGLPFAWindow,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFAStencilSize, 8,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFANoRecovery,
		NSOpenGLPFAAccelerated,
		0
	};
	return [[NSOpenGLPixelFormat alloc] initWithAttributes:attribute];
}
//-----------------------------------------------------------------------
static std::shared_ptr<CocoaOpenGLContext> CreateOpenGLContext()
{
	NSOpenGLPixelFormat* pixelFormat = CreateDefaultPixelFormat();
	return std::make_shared<CocoaOpenGLContext>(pixelFormat);
}
//-----------------------------------------------------------------------
static std::shared_ptr<GraphicsContext> CreateGraphicsContext(
	std::shared_ptr<CocoaOpenGLContext> openGLContext, std::weak_ptr<GameWindow> gameWindow)
{
	using RenderSystem::GL4::GraphicsContextGL4;
	auto nativeContext = std::unique_ptr<GraphicsContextGL4>(new GraphicsContextGL4(
		std::move(openGLContext), std::move(gameWindow)));
	
	return std::make_shared<GraphicsContext>(std::move(nativeContext));
}
//-----------------------------------------------------------------------
#pragma mark -
#pragma mark CocoaGameHost::Impl
//-----------------------------------------------------------------------
class CocoaGameHost::Impl final: public GameHost
{
public:
	Impl(std::shared_ptr<CocoaGameWindow> window, std::shared_ptr<SystemEventDispatcher> dipatcher);

	~Impl() = default;

	///@copydoc GameHost
	void Run(std::weak_ptr<Game> game) override;
	
	///@copydoc GameHost
	void Exit() override;

	///@copydoc GameHost
	std::shared_ptr<GameWindow> GetWindow() override;
	
	///@copydoc GameHost
	std::shared_ptr<GraphicsContext> GetGraphicsContext();

public:
	void RenderFrame(Game & game);
	
	void DoEvents();
	
	void ProcessSystemEvents(Event const& event);

private:
	std::weak_ptr<Game> game;
	std::shared_ptr<CocoaGameWindow> gameWindow;

	std::shared_ptr<SystemEventDispatcher> systemEventDispatcher;
	ScopedConnection systemEventConnection;
	
	std::shared_ptr<CocoaOpenGLContext> openGLContext;
	std::shared_ptr<GraphicsContext> graphicsContext;
	
	std::shared_ptr<CocoaMouse> mouse;
	
	bool exitRequest;
};
//-----------------------------------------------------------------------
CocoaGameHost::Impl::Impl(std::shared_ptr<CocoaGameWindow> window_, std::shared_ptr<SystemEventDispatcher> eventDispatcher)
	: exitRequest(false)
	, gameWindow(std::move(window_))
	, systemEventDispatcher(std::move(eventDispatcher))
	, openGLContext(CreateOpenGLContext())
{
	graphicsContext = CreateGraphicsContext(openGLContext, gameWindow);
	
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
void CocoaGameHost::Impl::RenderFrame(Game & game)
{
	POMDOG_ASSERT(gameWindow);

	if (!gameWindow || gameWindow->IsMinimized()) {
		// skip rendering
		return;
	}
	
	// RenderBegin:
	CGLContextObj coreOpenGLContext = static_cast<CGLContextObj>([openGLContext->GetNSOpenGLContext() CGLContextObj]);
	CGLLockContext(coreOpenGLContext);
	
	openGLContext->BindCurrentContext();

	// Render:
	game.Draw();
	
	// RenderEnd:
	CGLUnlockContext(coreOpenGLContext);
}
//-----------------------------------------------------------------------
void CocoaGameHost::Impl::DoEvents()
{
	systemEventDispatcher->Tick();
}
//-----------------------------------------------------------------------
void CocoaGameHost::Impl::ProcessSystemEvents(Event const& event)
{
	if (event.GetCode() == WindowShouldCloseEvent) {
		Log::LogMessage("WindowShouldCloseEvent", LoggingLevel::Internal);
		this->Exit();
	}
	else if (event.GetCode() == WindowWillCloseEvent) {
		Log::LogMessage("WindowWillCloseEvent", LoggingLevel::Internal);
		// Not implemented
	}
}
//-----------------------------------------------------------------------
#pragma mark -
#pragma mark CocoaGameHost
//-----------------------------------------------------------------------
CocoaGameHost::CocoaGameHost(std::shared_ptr<CocoaGameWindow> window, std::shared_ptr<SystemEventDispatcher> eventDispatcher)
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

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
