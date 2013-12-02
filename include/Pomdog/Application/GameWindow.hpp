//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAMEWINDOW_HPP
#define POMDOG_GAMEWINDOW_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "../Utility/Noncopyable.hpp"
#include <string>

namespace Pomdog {

class Rectangle;

/// @addtogroup Framework
/// @{
/// @addtogroup Application
/// @{

///@~Japanese
/// @brief ゲームウィンドウです。
class GameWindow: Noncopyable
{
public:	
	virtual	~GameWindow() = default;

	///@~Japanese
	/// @brief プレイヤーによるウィンドウサイズの変更を許可するかどうかを取得します。
	/// @return プレイヤーによるウィンドウサイズの変更を許可する場合は true を返却します。
	virtual bool GetAllowPlayerResizing() const = 0;

	///@~Japanese
	/// @brief プレイヤーによるウィンドウサイズの変更を許可するかどうかを設定します。
	/// @param allowResizing プレイヤーによるウィンドウサイズの変更を許可する場合は true を指定します。 
	virtual void SetAllowPlayerResizing(bool allowResizing) = 0;

	///@~Japanese
	/// @brief 現在のキャプション（ウィンドウタイトル）を取得します。
	/// @return 現在のキャプションを文字列として返却します。
	virtual std::string GetCaption() const = 0;

	///@~Japanese
	/// @brief キャプション（ウィンドウタイトル）を設定します。
	/// @param caption キャプションを指定します。
	virtual void SetCaption(std::string const& caption) = 0;

	///@~Japanese
	/// @brief 現在のクライアント領域のサイズを取得します。
	/// @return クライアント領域を表すサイズを返却します。
	virtual Rectangle GetClientBounds() const = 0;

	///@~Japanese
	/// @brief ゲームウィンドウのクライアント領域のサイズを設定します。
	/// @param clientBounds ゲームウィンドウのクライアント領域のサイズを指定します。
	virtual void SetClientBounds(Rectangle const& clientBounds) = 0;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEWINDOW_HPP)
