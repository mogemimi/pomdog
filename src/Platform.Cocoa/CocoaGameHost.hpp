//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_COCOA_GAMEHOST_HPP
#define POMDOG_SRC_COCOA_GAMEHOST_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Application/GameHost.hpp>
#include <memory>
#include "../RenderSystem.GL4/GraphicsContextGL4.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {

class CocoaOpenGLContext;
class CocoaGameWindow;

class CocoaGameHost final: public GameHost
{
public:
	explicit CocoaGameHost(std::shared_ptr<CocoaGameWindow> window);

	~CocoaGameHost();

	///@copydoc GameHost
	void Run(std::weak_ptr<Game> game) override;
	
	///@copydoc GameHost
	void Exit() override;

	///@copydoc GameHost
	std::shared_ptr<GameWindow> GetWindow() override;
	
private:
	void RenderFrame(Game & game);
	
	void TestDraw();
	
private:
	std::weak_ptr<Game> game;
	std::shared_ptr<CocoaGameWindow> gameWindow;
	
	std::shared_ptr<CocoaOpenGLContext> openGLContext;
	std::shared_ptr<GL4::GraphicsContextGL4> graphicsContext;
	
	bool exitRequest;
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_COCOA_GAMEHOST_HPP)
