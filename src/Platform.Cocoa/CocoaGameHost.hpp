//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_COCOA_GAMEHOST_29C3B75B_9D10_4DA7_BD1A_3AD8511B7D4F_HPP
#define POMDOG_SRC_COCOA_GAMEHOST_29C3B75B_9D10_4DA7_BD1A_3AD8511B7D4F_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Application/GameHost.hpp>
#include <memory>
#include "../Application/SystemEventDispatcher.hpp"

namespace Pomdog {
namespace Details {

class PresentationParameters;

namespace Cocoa {

class CocoaGameWindow;

class CocoaGameHost final: public GameHost
{
public:
	CocoaGameHost(std::shared_ptr<CocoaGameWindow> const& window,
		std::shared_ptr<SystemEventDispatcher> const& dispatcher,
		PresentationParameters const& presentationParameters);

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
	
	///@copydoc GameHost
	std::shared_ptr<AssetManager> GetAssetManager() override;

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_COCOA_GAMEHOST_29C3B75B_9D10_4DA7_BD1A_3AD8511B7D4F_HPP)
