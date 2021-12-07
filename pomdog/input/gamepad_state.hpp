// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/gamepad_buttons.hpp"
#include "pomdog/input/gamepad_dpad.hpp"
#include "pomdog/input/gamepad_thumbsticks.hpp"

namespace Pomdog {

struct GamepadState final {
    GamepadButtons Buttons;
    GamepadThumbSticks ThumbSticks;
    GamepadDPad DPad;
    bool IsConnected = false;
};

} // namespace Pomdog
