//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_KEYBOARD_B6F4D4B4_A74A_4EFF_88D9_828C7D9FC5F7_HPP
#define POMDOG_KEYBOARD_B6F4D4B4_A74A_4EFF_88D9_828C7D9FC5F7_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/Export.hpp"
#include "../Utility/Noncopyable.hpp"

namespace Pomdog {

class KeyboardState;

///@~Japanese
/// @brief キーボードの入力を取得するために使います。
class Keyboard: Noncopyable {
public:
	virtual ~Keyboard() = default;

	///@~Japanese
	/// @brief キーボードの現在の状態を取得します。
	virtual KeyboardState const& State() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_KEYBOARD_B6F4D4B4_A74A_4EFF_88D9_828C7D9FC5F7_HPP)
