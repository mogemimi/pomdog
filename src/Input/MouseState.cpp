//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Input/MouseState.hpp>

namespace Pomdog {

MouseState::MouseState()
	: Position(0, 0)
	, ScrollWheel(0)
	, LeftButton(ButtonState::Released)
	, MiddleButton(ButtonState::Released)
	, RightButton(ButtonState::Released)
	, XButton1(ButtonState::Released)
	, XButton2(ButtonState::Released)
{}

}// namespace Pomdog
