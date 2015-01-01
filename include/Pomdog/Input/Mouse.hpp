//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_MOUSE_8BFC478A_C7F2_4D4E_A4FE_C756500D5E33_HPP
#define POMDOG_MOUSE_8BFC478A_C7F2_4D4E_A4FE_C756500D5E33_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class MouseState;

///@~Japanese
/// @brief マウスによる入力を取得するのに使います。
class POMDOG_EXPORT Mouse {
public:
	Mouse() = default;
	Mouse(Mouse const&) = delete;
	Mouse & operator=(Mouse const&) = delete;

	virtual ~Mouse() = default;

	///@~Japanese
	/// @brief マウスの現在の状態を取得します。
	virtual MouseState GetState() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_MOUSE_8BFC478A_C7F2_4D4E_A4FE_C756500D5E33_HPP)
