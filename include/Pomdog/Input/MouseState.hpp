// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Input/ButtonState.hpp"
#include "Pomdog/Math/Point2D.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class POMDOG_EXPORT MouseState final {
public:
    /// Position of the mouse cursor
    Point2D Position{0, 0};

    std::int32_t ScrollWheel = 0;

    /// Left mouse button
    ButtonState LeftButton = ButtonState::Released;

    /// Middle mouse button
    ButtonState MiddleButton = ButtonState::Released;

    /// Right mouse button
    ButtonState RightButton = ButtonState::Released;

    /// First extended mouse button
    ButtonState XButton1 = ButtonState::Released;

    /// Second extended mouse button
    ButtonState XButton2 = ButtonState::Released;
};

} // namespace Pomdog
