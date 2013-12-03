//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_COCOA_GAMEHOST_HPP
#define POMDOG_COCOA_GAMEHOST_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Application/GameHost.hpp>
#include <memory>

namespace Pomdog {
namespace Details {
namespace Cocoa {

class CocoaGameWindow;

class CocoaGameHost final: public GameHost
{
public:
	explicit CocoaGameHost(std::shared_ptr<CocoaGameWindow> window);

	~CocoaGameHost();

	void Run(std::weak_ptr<Game> game) override;
	
	void Exit() override;
	
private:
	void RenderFrame(Game & game);
	
private:
	std::weak_ptr<Game> game;
	std::shared_ptr<CocoaGameWindow> gameWindow;
	bool exitRequest;
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_COCOA_GAMEHOST_HPP)
