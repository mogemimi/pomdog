// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/button_state.h"

namespace pomdog {

struct GamepadDPad final {
    ButtonState down = ButtonState::Released;
    ButtonState up = ButtonState::Released;
    ButtonState left = ButtonState::Released;
    ButtonState right = ButtonState::Released;
};

} // namespace pomdog
