//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_KEYSTATE_0C5E11FF_3B98_436F_9243_679933DA89E8_HPP
#define POMDOG_KEYSTATE_0C5E11FF_3B98_436F_9243_679933DA89E8_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Input
/// @{

///@~Japanese
/// @brief キーが押されているかどうかの状態を表します。
enum class KeyState: bool
{
	///@~Japanese
	/// @brief キーが離されている状態です。
	Released = false,

	///@~Japanese
	/// @brief キーが押されている状態です。
	Pressed = true,
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_KEYSTATE_0C5E11FF_3B98_436F_9243_679933DA89E8_HPP)
