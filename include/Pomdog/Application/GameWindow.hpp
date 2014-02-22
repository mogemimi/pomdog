//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAMEWINDOW_22D747BC_F865_4F1E_B8BB_609D297F662F_HPP
#define POMDOG_GAMEWINDOW_22D747BC_F865_4F1E_B8BB_609D297F662F_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <string>
#include "../Config/Export.hpp"
#include "../Utility/Noncopyable.hpp"

namespace Pomdog {

class Rectangle;

/// @addtogroup Framework
/// @{
/// @addtogroup Application
/// @{

///@~Japanese
/// @brief ゲームウィンドウです。
class POMDOG_EXPORT GameWindow: Noncopyable
{
public:	
	virtual ~GameWindow() = default;

	///@~Japanese
	/// @brief プレイヤーによるウィンドウサイズの変更を許可するかどうかを取得します。
	/// @return プレイヤーによるウィンドウサイズの変更を許可する場合は true を返却します。
	virtual bool AllowPlayerResizing() const = 0;

	///@~Japanese
	/// @brief プレイヤーによるウィンドウサイズの変更を許可するかどうかを設定します。
	/// @param allowResizing プレイヤーによるウィンドウサイズの変更を許可する場合は true を指定します。 
	virtual void AllowPlayerResizing(bool allowResizing) = 0;

	///@~Japanese
	/// @brief 現在のキャプション（ウィンドウタイトル）を取得します。
	/// @return 現在のキャプションを文字列として返却します。
	virtual std::string Caption() const = 0;

	///@~Japanese
	/// @brief キャプション（ウィンドウタイトル）を設定します。
	/// @param caption キャプションを指定します。
	virtual void Caption(std::string const& caption) = 0;

	///@~Japanese
	/// @brief 現在のクライアント領域のサイズを取得します。
	/// @return クライアント領域を表すサイズを返却します。
	virtual Rectangle ClientBounds() const = 0;

	///@~Japanese
	/// @brief ゲームウィンドウのクライアント領域のサイズを設定します。
	/// @param clientBounds ゲームウィンドウのクライアント領域のサイズを指定します。
	virtual void ClientBounds(Rectangle const& clientBounds) = 0;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEWINDOW_22D747BC_F865_4F1E_B8BB_609D297F662F_HPP)
