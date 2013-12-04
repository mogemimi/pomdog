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

#include <OpenGL/OpenGL.h>
#include <Pomdog/Graphics/GraphicsContext.hpp>
#include "CocoaOpenGLContext.hpp"
#include "../RenderSystem.GL4/GraphicsContextGL4.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {

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
CocoaGameHost::CocoaGameHost(std::shared_ptr<CocoaGameWindow> window)
	: exitRequest(false)
	, gameWindow(window)
	, openGLContext(CreateOpenGLContext())
{
	auto nativeContext = std::unique_ptr<GL4::GraphicsContextGL4>(new GL4::GraphicsContextGL4(openGLContext));
	graphicsContext = std::make_shared<GraphicsContext>(std::move(nativeContext));
	
	window->ResetGLContext(openGLContext);
}
//-----------------------------------------------------------------------
CocoaGameHost::~CocoaGameHost()
{
}
//-----------------------------------------------------------------------
void CocoaGameHost::Run(std::weak_ptr<Game> weakGame)
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
		game->Update();
		RenderFrame(*game);
	}
}
//-----------------------------------------------------------------------
void CocoaGameHost::Exit()
{
	exitRequest = true;
}
//-----------------------------------------------------------------------
std::shared_ptr<GameWindow> CocoaGameHost::GetWindow()
{
	return gameWindow;
}
//-----------------------------------------------------------------------
std::shared_ptr<GraphicsContext> CocoaGameHost::GetGraphicsContext()
{
	return graphicsContext;
}
//-----------------------------------------------------------------------
void CocoaGameHost::RenderFrame(Game & game)
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

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
