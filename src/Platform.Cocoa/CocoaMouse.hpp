//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_COCOA_MOUSE_E737D799_3080_4B3E_A2A8_E3886D4800D1_HPP
#define POMDOG_SRC_COCOA_MOUSE_E737D799_3080_4B3E_A2A8_E3886D4800D1_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Input/Mouse.hpp>
#include <Pomdog/Input/MouseState.hpp>

namespace Pomdog {
namespace Details {
namespace Cocoa {

class CocoaMouse final: public Mouse {
public:
	CocoaMouse();

	///@copydoc Pomdog::Mouse
	MouseState const& State() const override;
	
	void Position(Point2D const& position);
	void LeftButton(ButtonState buttonState);
	void RightButton(ButtonState buttonState);
	void MiddleButton(ButtonState buttonState);
	void XButton1(ButtonState buttonState);
	void XButton2(ButtonState buttonState);
	
	void WheelDelta(double wheelDelta);
	
private:
	MouseState mouseState;
	double scrollWheel;
};

}// namespace Mouse
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_COCOA_MOUSE_E737D799_3080_4B3E_A2A8_E3886D4800D1_HPP)
