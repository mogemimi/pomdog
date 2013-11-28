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
#include <Pomdog/Application/GameSystem.hpp>
#include <Pomdog/Application/detail/SystemHub.hpp>

namespace Pomdog {
namespace Details {

class SystemHub;

}// namespace Details

class GameSystem;

/// @addtogroup Framework
/// @{
/// @addtogroup Application
/// @{

///@~Japanese
///  @brief GameSystem を含むすべてのサブシステムをの起動、およびアプリケーションの実行を行います。
class Bootstrapper final
{
private:
	std::shared_ptr<Details::SystemHub> systemHub;
	std::shared_ptr<GameSystem> gameSystem;
	
public:
	Bootstrapper()
	{
		systemHub = std::make_shared<Details::SystemHub>();
		Details::ResetGlobalSystemHub(systemHub);

		gameSystem = std::make_shared<GameSystem>(systemHub);
		systemHub->GameSystem = gameSystem;
	}
	
	~Bootstrapper()
	{
		Details::ResetGlobalSystemHub();
	}
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_BOOTSTRAPPER_HPP)
