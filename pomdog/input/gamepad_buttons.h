// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog {

/// Represents a logical gamepad button.
///
/// ```
/// # Gamepad layout
///        ┌────────────┐             ┌────────────┐
/// ┌──────┘            └─────────────┘            └──────┐
/// │         ┌──────┐                   ┌──────┐         │
/// │         │  LT  │    L/R Trigger    │  RT  │         │
/// │         ├──────┤                   ├──────┤         │
/// │         │  LB  │    L/R Bumper     │  RB  │         │
/// │         └──────┘                   └──────┘         │
/// ├─────────────────────────────────────────────────────┤
/// │              ┌──────┐          ┌──────┐             │
/// │     ┌───┐    │  LM  │ L/R Menu │  RM  │   ┌───┐     │
/// │     │ ▲ │    └──────┘          └──────┘   │ Y │     │
/// │ ┌───┘ │ └───┐        ┌───────┐       ┌───┐└───┘┌───┐│
/// │ │◄─ DPad ──►│        │ Guide │       │ X │     │ B ││
/// │ └───┐ │ ┌───┘        └───────┘       └───┘┌───┐└───┘│
/// │     │ ▼ │                                 │ A │     │
/// │     └───┘     ┌────┐            ┌────┐    └───┘     │
/// │               │ LS │ L/R Stick  │ RS │              │
/// │               └────┘            └────┘              │
/// │              ┌──────────────────────┐               │
/// │              │                      │               │
/// └──────────────┘                      └───────────────┘
/// ```
enum class GamepadButtons : i8 {
    /// The "A" button.
    ///
    /// Platform-specific mapping:
    /// - PS4 Controller    = Cross
    /// - Switch Controller = B
    /// - Xbox Controller   = A
    A,

    /// The "B" button.
    ///
    /// Platform-specific mapping:
    /// - PS4 Controller    = Circle
    /// - Switch Controller = A
    /// - Xbox Controller   = B
    B,

    /// The "X" button.
    ///
    /// Platform-specific mapping:
    /// - PS4 Controller    = Square
    /// - Switch Controller = Y
    /// - Xbox Controller   = X
    X,

    /// The "Y" button.
    ///
    /// Platform-specific mapping:
    /// - PS4 Controller    = Triangle
    /// - Switch Controller = X
    /// - Xbox Controller   = Y
    Y,

    /// The "LB" button.
    ///
    /// Platform-specific mapping:
    /// - PS4 Controller    = L1
    /// - Switch Controller = L
    /// - Xbox Controller   = LB
    LeftBumper,

    /// The "RB" button.
    ///
    /// Platform-specific mapping:
    /// - PS4 Controller    = R1
    /// - Switch Controller = R
    /// - Xbox Controller   = RB
    RightBumper,

    /// The "LT" button.
    ///
    /// Platform-specific mapping:
    /// - PS4 Controller    = L2
    /// - Switch Controller = ZL
    /// - Xbox Controller   = LT
    LeftTrigger,

    /// The "RT" button.
    ///
    /// Platform-specific mapping:
    /// - PS4 Controller    = R2
    /// - Switch Controller = ZR
    /// - Xbox Controller   = RT
    RightTrigger,

    /// The left menu button (e.g., Options / View / Minus).
    ///
    /// Platform-specific mapping:
    /// - PS4 Controller    = Options
    /// - Switch Controller = '-' Button
    /// - Xbox Controller   = View
    LeftMenu,

    /// The right menu button (e.g., Share / Plus).
    ///
    /// Platform-specific mapping:
    /// - PS4 Controller    = Share
    /// - Switch Controller = '+' Button
    /// - Xbox Controller   = Menu
    RightMenu,

    /// The "Left Stick" button.
    ///
    /// Platform-specific mapping:
    /// - PS4 Controller    = L3
    /// - Switch Controller = Left Stick
    /// - Xbox Controller   = Left Stick
    LeftStick,

    /// The "Right Stick" button.
    ///
    /// Platform-specific mapping:
    /// - PS4 Controller    = R3
    /// - Switch Controller = Right Stick
    /// - Xbox Controller   = Right Stick
    RightStick,

    /// Directional pad (D-pad) up.
    DPadUp,

    /// Directional pad (D-pad) down.
    DPadDown,

    /// Directional pad (D-pad) left.
    DPadLeft,

    /// Directional pad (D-pad) right.
    DPadRight,

    /// The "Guide" (system) button.
    ///
    /// Platform-specific mapping:
    /// - Switch Controller = Home
    /// - Xbox Controller   = Xbox
    Guide,

    /// The "Extra1" button. An additional, platform-specific button.
    Extra1,

    /// The "Extra2" button. An additional, platform-specific button.
    Extra2,

    /// Represents an invalid or uninitialized button.
    Invalid = -1,
};

} // namespace pomdog
