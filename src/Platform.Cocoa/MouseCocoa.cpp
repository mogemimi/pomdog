// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "MouseCocoa.hpp"
#include <type_traits>

namespace Pomdog {
namespace Detail {
namespace Cocoa {
//-----------------------------------------------------------------------
MouseCocoa::MouseCocoa()
: scrollWheel(0)
{}
//-----------------------------------------------------------------------
MouseState MouseCocoa::GetState() const
{
    return state;
}
//-----------------------------------------------------------------------
void MouseCocoa::Position(Point2D const& position)
{
    state.Position = position;
}
//-----------------------------------------------------------------------
void MouseCocoa::WheelDelta(double wheelDelta)
{
    scrollWheel += wheelDelta;
    static_assert(std::is_same<double, decltype(scrollWheel)>::value, "");
    static_assert(std::is_same<std::int32_t, decltype(state.ScrollWheel)>::value, "");
    state.ScrollWheel = this->scrollWheel;
}
//-----------------------------------------------------------------------
void MouseCocoa::LeftButton(ButtonState buttonState)
{
    state.LeftButton = buttonState;
}
//-----------------------------------------------------------------------
void MouseCocoa::RightButton(ButtonState buttonState)
{
    state.RightButton = buttonState;
}
//-----------------------------------------------------------------------
void MouseCocoa::MiddleButton(ButtonState buttonState)
{
    state.MiddleButton = buttonState;
}
//-----------------------------------------------------------------------
void MouseCocoa::XButton1(ButtonState buttonState)
{
    state.XButton1 = buttonState;
}
//-----------------------------------------------------------------------
void MouseCocoa::XButton2(ButtonState buttonState)
{
    state.XButton2 = buttonState;
}
//-----------------------------------------------------------------------
} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
