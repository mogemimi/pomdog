// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/button_state.h"

namespace pomdog {

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
    ButtonState a = ButtonState::Up;
    ButtonState b = ButtonState::Up;
    ButtonState x = ButtonState::Up;
    ButtonState y = ButtonState::Up;
    ButtonState leftShoulder = ButtonState::Up;
    ButtonState rightShoulder = ButtonState::Up;
    ButtonState leftTrigger = ButtonState::Up;
    ButtonState rightTrigger = ButtonState::Up;
    ButtonState leftMenu = ButtonState::Up;
    ButtonState rightMenu = ButtonState::Up;
    ButtonState leftStick = ButtonState::Up;
    ButtonState rightStick = ButtonState::Up;
    ButtonState guide = ButtonState::Up;
    ButtonState extra1 = ButtonState::Up;
    ButtonState extra2 = ButtonState::Up;
};

} // namespace pomdog
