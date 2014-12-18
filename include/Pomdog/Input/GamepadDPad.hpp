//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAMEPADDPAD_986E4F79_8941_46F3_9ED3_FD90A5E560AE_HPP
#define POMDOG_GAMEPADDPAD_986E4F79_8941_46F3_9ED3_FD90A5E560AE_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "ButtonState.hpp"

namespace Pomdog {

///@~Japanese
/// @brief ゲームパッドの方向キー (DPad) の状態を示します。
class GamepadDPad {
public:
	ButtonState Down = ButtonState::Released;
	ButtonState Up = ButtonState::Released;
	ButtonState Left = ButtonState::Released;
	ButtonState Right = ButtonState::Released;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEPADDPAD_986E4F79_8941_46F3_9ED3_FD90A5E560AE_HPP)
