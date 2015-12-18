// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_MOUSESTATE_0CEDE70B_HPP
#define POMDOG_MOUSESTATE_0CEDE70B_HPP

#include "ButtonState.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include <cstdint>

namespace Pomdog {

class POMDOG_EXPORT MouseState {
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

#endif // POMDOG_MOUSESTATE_0CEDE70B_HPP
