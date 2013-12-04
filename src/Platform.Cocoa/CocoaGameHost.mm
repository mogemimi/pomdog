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

#include <Pomdog/Math/Color.hpp>
#include "CocoaOpenGLContext.hpp"
#include "../../src/RenderSystem.GL4/GraphicsContextGL4.hpp"
#include <OpenGL/OpenGL.h>

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
CocoaGameHost::CocoaGameHost(std::shared_ptr<CocoaGameWindow> window)
	: exitRequest(false)
	, gameWindow(window)
{
	NSOpenGLPixelFormat* pixelFormat = CreateDefaultPixelFormat();
	openGLContext = std::make_shared<CocoaOpenGLContext>(pixelFormat);
	graphicsContext = std::make_shared<GL4::GraphicsContextGL4>(openGLContext);
	
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
void CocoaGameHost::RenderFrame(Game & game)
{
	POMDOG_ASSERT(gameWindow);

	if (!gameWindow || gameWindow->IsMinimized()) {
		// skip rendering
		return;
	}
	TestDraw();
	game.Draw();
}
//-----------------------------------------------------------------------
void CocoaGameHost::TestDraw()
{
	CGLContextObj coreOpenGLContext = static_cast<CGLContextObj>([openGLContext->GetNSOpenGLContext() CGLContextObj]);
	CGLLockContext(coreOpenGLContext);
	
	openGLContext->BindCurrentContext();

	auto color = Pomdog::Color::CornflowerBlue;
	graphicsContext->Clear(color);

	graphicsContext->Present();
	
	CGLUnlockContext(coreOpenGLContext);
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
