// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "GamepadButtons.hpp"
#include "GamepadDPad.hpp"
#include "GamepadThumbSticks.hpp"

namespace Pomdog {

class GamepadState {
public:
    GamepadButtons Buttons;
    GamepadThumbSticks ThumbSticks;
    GamepadDPad DPad;
    bool IsConnected = false;
};

} // namespace Pomdog
