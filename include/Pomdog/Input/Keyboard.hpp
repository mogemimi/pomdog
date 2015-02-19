//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_KEYBOARD_B6F4D4B4_A74A_4EFF_88D9_828C7D9FC5F7_HPP
#define POMDOG_KEYBOARD_B6F4D4B4_A74A_4EFF_88D9_828C7D9FC5F7_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class KeyboardState;

///@~Japanese
/// @brief キーボードの入力を取得するために使います。
class POMDOG_EXPORT Keyboard {
public:
	Keyboard() = default;
	Keyboard(Keyboard const&) = delete;
	Keyboard & operator=(Keyboard const&) = delete;

	virtual ~Keyboard() = default;

	///@~Japanese
	/// @brief キーボードの現在の状態を取得します。
	virtual KeyboardState GetState() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_KEYBOARD_B6F4D4B4_A74A_4EFF_88D9_828C7D9FC5F7_HPP)
