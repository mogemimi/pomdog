//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAMEHOST_FDAA974D_DBD8_44E8_84CD_F6FAA243957B_HPP
#define POMDOG_GAMEHOST_FDAA974D_DBD8_44E8_84CD_F6FAA243957B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include "../Config/Export.hpp"
#include "../Config/FundamentalTypes.hpp"
#include "../Utility/Noncopyable.hpp"

namespace Pomdog {

class Game;
class GameWindow;
class GraphicsContext;
class GraphicsDevice;

/// @addtogroup Framework
/// @{
/// @addtogroup Application
/// @{

class POMDOG_EXPORT GameHost: Noncopyable, public std::enable_shared_from_this<GameHost>
{
public:
	virtual ~GameHost() = default;
	
	///@~Japanese
	/// @brief アプリケーションを実行します。
	virtual void Run(std::weak_ptr<Game> game) = 0;
	
	///@~Japanese
	/// @brief 実行中のゲームおよびアプリケーションを終了します。
	virtual void Exit() = 0;
	
	///@~Japanese
	/// @brief ウィンドウを取得します。
	virtual std::shared_ptr<GameWindow> GetWindow() = 0;
	
	///@~Japanese
	/// @brief グラフィックスコンテキストを取得します。
	virtual std::shared_ptr<GraphicsContext> GetGraphicsContext() = 0;
	
	///@~Japanese
	/// @brief グラフィックスデバイスを取得します。
	virtual std::shared_ptr<GraphicsDevice> GetGraphicsDevice() = 0;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEHOST_FDAA974D_DBD8_44E8_84CD_F6FAA243957B_HPP)
