//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_WIN32_GAMEHOSTWIN32_F8172FE6_1CB8_49D0_B6FE_B5448E094B9C_HPP
#define POMDOG_SRC_WIN32_GAMEHOSTWIN32_F8172FE6_1CB8_49D0_B6FE_B5448E094B9C_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "../Application/SystemEventDispatcher.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include <memory>

namespace Pomdog {

class PresentationParameters;

namespace Detail {
namespace InputSystem {

class InputDeviceFactory;

}// namespace InputSystem
namespace Win32 {

class GameWindowWin32;

class GameHostWin32 final: public GameHost {
public:
	GameHostWin32(std::shared_ptr<GameWindowWin32> const& window,
		std::shared_ptr<SystemEventDispatcher> const& dispatcher,
		PresentationParameters const& presentationParameters,
		std::unique_ptr<InputSystem::InputDeviceFactory> && inputDeviceFactory);

	~GameHostWin32();

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

}// namespace Win32
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_WIN32_GAMEHOSTWIN32_F8172FE6_1CB8_49D0_B6FE_B5448E094B9C_HPP)
