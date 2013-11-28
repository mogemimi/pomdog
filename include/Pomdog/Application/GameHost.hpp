//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAMEHOST_HPP
#define POMDOG_GAMEHOST_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "../Utility/Noncopyable.hpp"
#include <memory>

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Application
/// @{

class GameHost: Noncopyable, public std::enable_shared_from_this<GameHost>
{
public:
	virtual ~GameHost() = default;
	
	///@~Japanese
	/// @brief アプリケーションを実行します。
	virtual void Run() = 0;
	//virtual void Run(std::shared_ptr<Game> game) = 0;
	
	///@~Japanese
	/// @brief 実行中のゲームおよびアプリケーションを終了します。
	virtual void Exit() = 0;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEHOST_HPP)
