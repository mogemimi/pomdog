// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/button_state.h"
#include "pomdog/math/point2d.h"

namespace pomdog {

class POMDOG_EXPORT MouseState final {
public:
    /// Position of the mouse cursor
    Point2D position{0, 0};

    i32 scrollWheel = 0;

    /// Left mouse button
    ButtonState leftButton = ButtonState::Up;

    /// Middle mouse button
    ButtonState middleButton = ButtonState::Up;

    /// Right mouse button
    ButtonState rightButton = ButtonState::Up;

    /// First extended mouse button
    ButtonState xButton1 = ButtonState::Up;

    /// Second extended mouse button
    ButtonState xButton2 = ButtonState::Up;
};

} // namespace pomdog
