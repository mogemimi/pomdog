//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_GAMEHOSTCOCOA_29C3B75B_9D10_4DA7_BD1A_3AD8511B7D4F_HPP
#define POMDOG_GAMEHOSTCOCOA_29C3B75B_9D10_4DA7_BD1A_3AD8511B7D4F_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "../Application/SystemEventDispatcher.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include <memory>

namespace Pomdog {

class PresentationParameters;

namespace Details {
namespace Cocoa {

class GameWindowCocoa;

class GameHostCocoa final: public GameHost {
public:
	GameHostCocoa(std::shared_ptr<GameWindowCocoa> const& window,
		std::shared_ptr<SystemEventDispatcher> const& dispatcher,
		PresentationParameters const& presentationParameters);

	~GameHostCocoa();

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
	std::shared_ptr<Pomdog::AudioEngine> AudioEngine() override;

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

#endif // !defined(POMDOG_GAMEHOSTCOCOA_29C3B75B_9D10_4DA7_BD1A_3AD8511B7D4F_HPP)
