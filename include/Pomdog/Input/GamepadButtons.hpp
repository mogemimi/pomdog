// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/ButtonState.hpp"

namespace Pomdog {

// Gamepad's layout:
//  ----------------------------------------
//　　　    (L2)                  (R2)    Triggers
//　　　    (L1)                  (R1)    Shoulders
//  ---------------------------------------
//　　　          (LM)     (RM)           Menu Buttons
//　　　     +       (Guide)      (Y)
//  DPad  +-+-+                (X) (B)   Buttons
//　　　     +                    (A)
//  -----------------------------------------
//　　　         (LS)       (RS)
//　  　　 Left Stick       Right Stick
//  ------------------------------------------
struct GamepadButtons final {
    ButtonState A = ButtonState::Released;
    ButtonState B = ButtonState::Released;
    ButtonState X = ButtonState::Released;
    ButtonState Y = ButtonState::Released;
    ButtonState LeftShoulder = ButtonState::Released;
    ButtonState RightShoulder = ButtonState::Released;
    ButtonState LeftTrigger = ButtonState::Released;
    ButtonState RightTrigger = ButtonState::Released;
    ButtonState LeftMenu = ButtonState::Released;
    ButtonState RightMenu = ButtonState::Released;
    ButtonState LeftStick = ButtonState::Released;
    ButtonState RightStick = ButtonState::Released;
    ButtonState Guide = ButtonState::Released;
    ButtonState Extra1 = ButtonState::Released;
    ButtonState Extra2 = ButtonState::Released;
};

} // namespace Pomdog
