// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/gamepad_buttons.h"
#include "pomdog/input/gamepad_dpad.h"
#include "pomdog/input/gamepad_thumbsticks.h"

namespace pomdog {

struct GamepadState final {
    GamepadButtons buttons;
    GamepadThumbSticks thumbSticks;
    GamepadDPad dpad;
    bool isConnected = false;
};

} // namespace pomdog
