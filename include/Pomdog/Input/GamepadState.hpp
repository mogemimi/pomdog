// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEPADSTATE_02293326_EB1D_4D79_A4DB_15F7F0BA8FBF_HPP
#define POMDOG_GAMEPADSTATE_02293326_EB1D_4D79_A4DB_15F7F0BA8FBF_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEPADSTATE_02293326_EB1D_4D79_A4DB_15F7F0BA8FBF_HPP)
