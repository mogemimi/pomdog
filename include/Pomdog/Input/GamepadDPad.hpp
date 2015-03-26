// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEPADDPAD_986E4F79_8941_46F3_9ED3_FD90A5E560AE_HPP
#define POMDOG_GAMEPADDPAD_986E4F79_8941_46F3_9ED3_FD90A5E560AE_HPP

#include "ButtonState.hpp"

namespace Pomdog {

class GamepadDPad {
public:
    ButtonState Down = ButtonState::Released;
    ButtonState Up = ButtonState::Released;
    ButtonState Left = ButtonState::Released;
    ButtonState Right = ButtonState::Released;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEPADDPAD_986E4F79_8941_46F3_9ED3_FD90A5E560AE_HPP)
