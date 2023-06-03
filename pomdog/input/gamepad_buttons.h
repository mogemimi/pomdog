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
    ButtonState a = ButtonState::Released;
    ButtonState b = ButtonState::Released;
    ButtonState x = ButtonState::Released;
    ButtonState y = ButtonState::Released;
    ButtonState leftShoulder = ButtonState::Released;
    ButtonState rightShoulder = ButtonState::Released;
    ButtonState leftTrigger = ButtonState::Released;
    ButtonState rightTrigger = ButtonState::Released;
    ButtonState leftMenu = ButtonState::Released;
    ButtonState rightMenu = ButtonState::Released;
    ButtonState leftStick = ButtonState::Released;
    ButtonState rightStick = ButtonState::Released;
    ButtonState guide = ButtonState::Released;
    ButtonState extra1 = ButtonState::Released;
    ButtonState extra2 = ButtonState::Released;
};

} // namespace pomdog
