//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_BUTTONSTATE_5EBD2CE3_5E85_11E3_922C_A8206655A22B_HPP
#define POMDOG_BUTTONSTATE_5EBD2CE3_5E85_11E3_922C_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Input
/// @{

///@~Japanese
/// @brief ボタンが押されているかどうかの状態を表す列挙体です。
enum class ButtonState: bool
{
	///@~Japanese
	/// @brief ボタンが離されている状態を表します。
	Released = false,

	///@~Japanese
	/// @brief ボタンが押されている状態を表します。
	Pressed  = true
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_BUTTONSTATE_5EBD2CE3_5E85_11E3_922C_A8206655A22B_HPP)
