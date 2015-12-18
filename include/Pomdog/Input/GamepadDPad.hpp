// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_GAMEPADDPAD_986E4F79_HPP
#define POMDOG_GAMEPADDPAD_986E4F79_HPP

#include "ButtonState.hpp"

namespace Pomdog {

class GamepadDPad {
public:
    ButtonState Down = ButtonState::Released;
    ButtonState Up = ButtonState::Released;
    ButtonState Left = ButtonState::Released;
    ButtonState Right = ButtonState::Released;
};

} // namespace Pomdog

#endif // POMDOG_GAMEPADDPAD_986E4F79_HPP
