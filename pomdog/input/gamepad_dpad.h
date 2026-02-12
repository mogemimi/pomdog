// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/button_state.h"

namespace pomdog {

struct GamepadDPad final {
    ButtonState down = ButtonState::Up;
    ButtonState up = ButtonState::Up;
    ButtonState left = ButtonState::Up;
    ButtonState right = ButtonState::Up;
};

} // namespace pomdog
