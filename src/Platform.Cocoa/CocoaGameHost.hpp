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
#include "../Application/SystemEventDispatcher.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {

class CocoaGameWindow;

class CocoaGameHost final: public GameHost
{
public:
	CocoaGameHost(std::shared_ptr<CocoaGameWindow> window, std::shared_ptr<SystemEventDispatcher> dispatcher);

	~CocoaGameHost();

	///@copydoc GameHost
	void Run(std::weak_ptr<Game> game) override;
	
	///@copydoc GameHost
	void Exit() override;

	///@copydoc GameHost
	std::shared_ptr<GameWindow> GetWindow() override;
	
	///@copydoc GameHost
	std::shared_ptr<GraphicsContext> GetGraphicsContext() override;
	
	///@copydoc GameHost
	std::shared_ptr<GraphicsDevice> GetGraphicsDevice() override;

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_COCOA_GAMEHOST_HPP)
