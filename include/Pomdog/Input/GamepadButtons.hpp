// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/ButtonState.hpp"
#include <cstdint>

namespace Pomdog {

struct GamepadButtons {
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
