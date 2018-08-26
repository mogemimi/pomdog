// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Input/ButtonState.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include <cstdint>

namespace Pomdog {

class POMDOG_EXPORT MouseState final {
public:
    ///@brief Position of the mouse cursor
    Point2D Position {0, 0};

    std::int32_t ScrollWheel = 0;

    ///@brief Left mouse button
    ButtonState LeftButton = ButtonState::Released;

    ///@brief Middle mouse button
    ButtonState MiddleButton = ButtonState::Released;

    ///@brief Right mouse button
    ButtonState RightButton = ButtonState::Released;

    ///@brief First extended mouse button
    ButtonState XButton1 = ButtonState::Released;

    ///@brief Second extended mouse button
    ButtonState XButton2 = ButtonState::Released;
};

} // namespace Pomdog
