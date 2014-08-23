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

#include "../Application/SystemEventDispatcher.hpp"
#include <Pomdog/Application/GameHost.hpp>
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class PresentationParameters;

}// namespace RenderSystem
namespace Cocoa {

class CocoaGameWindow;

class CocoaGameHost final: public GameHost {
public:
	CocoaGameHost(std::shared_ptr<CocoaGameWindow> const& window,
		std::shared_ptr<SystemEventDispatcher> const& dispatcher,
		Details::RenderSystem::PresentationParameters const& presentationParameters);

	~CocoaGameHost();

	///@copydoc GameHost
	void Run(Game & game) override;
	
	///@copydoc GameHost
	void Exit() override;

	///@copydoc GameHost
	std::shared_ptr<Pomdog::GameWindow> Window() override;
	
	///@copydoc GameHost
	std::shared_ptr<Pomdog::GameClock> Clock() override;
	
	///@copydoc GameHost
	std::shared_ptr<Pomdog::GraphicsContext> GraphicsContext() override;
	
	///@copydoc GameHost
	std::shared_ptr<Pomdog::GraphicsDevice> GraphicsDevice() override;
	
	///@copydoc GameHost
	std::shared_ptr<Pomdog::AssetManager> AssetManager() override;

	///@copydoc GameHost
	std::shared_ptr<Pomdog::Keyboard> Keyboard() override;

	///@copydoc GameHost
	std::shared_ptr<Pomdog::Mouse> Mouse() override;

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_COCOA_GAMEHOST_29C3B75B_9D10_4DA7_BD1A_3AD8511B7D4F_HPP)
