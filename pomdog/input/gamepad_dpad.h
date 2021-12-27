// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/button_state.h"

namespace pomdog {

struct GamepadDPad final {
    ButtonState Down = ButtonState::Released;
    ButtonState Up = ButtonState::Released;
    ButtonState Left = ButtonState::Released;
    ButtonState Right = ButtonState::Released;
};

} // namespace pomdog
