//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_BOOTSTRAPPER_HPP
#define POMDOG_BOOTSTRAPPER_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include <memory>
#include <type_traits>
#include <array>
#include <functional>

// for implemetation
#include <Pomdog/Application/GameHost.hpp>
#include <Pomdog/Application/detail/PlatformSystem.hpp>

namespace Pomdog {

class Game;
class GameHost;

/// @addtogroup Framework
/// @{
/// @addtogroup Application
/// @{

///@~Japanese
/// @brief すべてのサブシステムをの起動、およびアプリケーションの実行を行います。
template <class GameClass>
class Bootstrapper final
{
private:
	static_assert(std::is_base_of<Game, GameClass>::value, "GameClass is base of Pomdog::Game.");
	static_assert(!std::is_abstract<GameClass>::value, "GameClass is not abstract.");
	
	std::shared_ptr<GameHost> gameHost;
	std::shared_ptr<GameClass> game;

public:
	Bootstrapper()
	{
		using Pomdog::Details::PlatformSystem;
		
		gameHost = PlatformSystem::CreateGameHost();
		game = std::make_shared<GameClass>(*gameHost);
	}
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_BOOTSTRAPPER_HPP)
