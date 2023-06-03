// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/input/button_state.h"
#include "pomdog/math/point2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT MouseState final {
public:
    /// Position of the mouse cursor
    Point2D position{0, 0};

    std::int32_t scrollWheel = 0;

    /// Left mouse button
    ButtonState leftButton = ButtonState::Released;

    /// Middle mouse button
    ButtonState middleButton = ButtonState::Released;

    /// Right mouse button
    ButtonState rightButton = ButtonState::Released;

    /// First extended mouse button
    ButtonState xButton1 = ButtonState::Released;

    /// Second extended mouse button
    ButtonState xButton2 = ButtonState::Released;
};

} // namespace pomdog
