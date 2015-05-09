// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEPADBUTTONS_7EF60FA0_HPP
#define POMDOG_GAMEPADBUTTONS_7EF60FA0_HPP

#include "ButtonState.hpp"
#include <cstdint>

namespace Pomdog {

class GamepadButtons {
public:
    ButtonState A = ButtonState::Released;
    ButtonState B = ButtonState::Released;
    ButtonState X = ButtonState::Released;
    ButtonState Y = ButtonState::Released;
    ButtonState LeftShoulder = ButtonState::Released;
    ButtonState RightShoulder = ButtonState::Released;
    ButtonState Start = ButtonState::Released;
    ButtonState LeftStick = ButtonState::Released;
    ButtonState RightStick = ButtonState::Released;
};

} // namespace Pomdog

#endif // POMDOG_GAMEPADBUTTONS_7EF60FA0_HPP
