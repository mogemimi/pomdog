//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaMouse.hpp"
#include <algorithm>
//#include <Pomdog/Logging/Log.hpp>

namespace Pomdog {
namespace Details {
namespace Cocoa {
//-----------------------------------------------------------------------
CocoaMouse::CocoaMouse()
: scrollWheel(0.0)
{}
//-----------------------------------------------------------------------
MouseState const& CocoaMouse::State() const
{
	return mouseState;
}
//-----------------------------------------------------------------------
void CocoaMouse::Position(Point2D const& position)
{
	mouseState.Position = position;
}
//-----------------------------------------------------------------------
void CocoaMouse::WheelDelta(double wheelDelta)
{
	scrollWheel += wheelDelta;
	static_assert(std::is_same<double, decltype(scrollWheel)>::value, "");
	static_assert(std::is_same<std::int32_t, decltype(mouseState.ScrollWheel)>::value, "");
	mouseState.ScrollWheel = this->scrollWheel;
}
//-----------------------------------------------------------------------
void CocoaMouse::LeftButton(ButtonState buttonState)
{
	//Log::Stream() << "LeftButton" << ": "
	//	<< (buttonState == ButtonState::Released ? "Released": "Pressed");
	mouseState.LeftButton = buttonState;
}
//-----------------------------------------------------------------------
void CocoaMouse::RightButton(ButtonState buttonState)
{
	//Log::Stream() << "RightButton" << ": "
	//	<< (buttonState == ButtonState::Released ? "Released": "Pressed");
	mouseState.RightButton = buttonState;
}
//-----------------------------------------------------------------------
void CocoaMouse::MiddleButton(ButtonState buttonState)
{
	//Log::Stream() << "MiddleButton" << ": "
	//	<< (buttonState == ButtonState::Released ? "Released": "Pressed");
	mouseState.MiddleButton = buttonState;
}
//-----------------------------------------------------------------------
void CocoaMouse::XButton1(ButtonState buttonState)
{
	mouseState.XButton1 = buttonState;
}
//-----------------------------------------------------------------------
void CocoaMouse::XButton2(ButtonState buttonState)
{
	mouseState.XButton2 = buttonState;
}
//-----------------------------------------------------------------------
}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
