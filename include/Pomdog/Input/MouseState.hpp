//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MOUSESTATE_0CEDE70B_5845_49FD_AA5F_2572AE32B73E_HPP
#define POMDOG_MOUSESTATE_0CEDE70B_5845_49FD_AA5F_2572AE32B73E_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "ButtonState.hpp"
#include "Pomdog/Config/Export.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief マウスの状態を表します。
class POMDOG_EXPORT MouseState {
public:
	///@~English
	/// @brief Position of the mouse cursor
	///@~Japanese
	/// @brief マウスカーソルの位置
	/// @remarks
	/// ゲームウィンドウの左上隅を基準とした位置を示します。
	/// マウスカーソルがクライアント領域外である左端、上端を越えた場合、
	/// 負の値をとることがあります。
	Point2D Position {0, 0};

	///@~Japanese
	/// @brief マウススクロールホイールの移動量
	std::int32_t ScrollWheel = 0;

	///@~English
	/// @brief Left mouse button
	///@~Japanese
	/// @brief マウス左ボタン
	ButtonState LeftButton = ButtonState::Released;

	///@~English
	/// @brief Middle mouse button
	///@~Japanese
	/// @brief マウス中央ボタン
	ButtonState MiddleButton = ButtonState::Released;

	///@~English
	/// @brief Right mouse button
	///@~Japanese
	/// @brief マウス右ボタン
	ButtonState RightButton = ButtonState::Released;

	///@~English
	/// @brief First extended mouse button
	///@~Japanese
	/// @brief マウス拡張ボタン1
	ButtonState XButton1 = ButtonState::Released;

	///@~English
	/// @brief Second extended mouse button
	///@~Japanese
	/// @brief マウス拡張ボタン2
	ButtonState XButton2 = ButtonState::Released;
};

}// namespace Pomdog

#endif // !defined(POMDOG_MOUSESTATE_0CEDE70B_5845_49FD_AA5F_2572AE32B73E_HPP)
