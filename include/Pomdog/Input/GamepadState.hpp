// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_GAMEPADSTATE_02293326_HPP
#define POMDOG_GAMEPADSTATE_02293326_HPP

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

#endif // POMDOG_GAMEPADSTATE_02293326_HPP
