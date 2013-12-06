//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MOUSE_ED6727EE_5E88_11E3_98F8_A8206655A22B_HPP
#define POMDOG_MOUSE_ED6727EE_5E88_11E3_98F8_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Utility/Noncopyable.hpp"

namespace Pomdog {

class MouseState;

/// @addtogroup Framework
/// @{
/// @addtogroup Input
/// @{

///@~Japanese
/// @brief マウスによる入力を取得するのに使います。
class Mouse: Noncopyable
{
public:
	virtual ~Mouse() = default;

	///@~Japanese
	/// @brief マウスの現在の状態を取得します。
	virtual MouseState const& GetState() const = 0;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_MOUSE_ED6727EE_5E88_11E3_98F8_A8206655A22B_HPP)
