//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MOUSE_8BFC478A_C7F2_4D4E_A4FE_C756500D5E33_HPP
#define POMDOG_MOUSE_8BFC478A_C7F2_4D4E_A4FE_C756500D5E33_HPP

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

#endif // !defined(POMDOG_MOUSE_8BFC478A_C7F2_4D4E_A4FE_C756500D5E33_HPP)
