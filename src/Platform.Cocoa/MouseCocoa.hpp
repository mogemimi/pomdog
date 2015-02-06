//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_MOUSECOCOA_E737D799_3080_4B3E_A2A8_E3886D4800D1_HPP
#define POMDOG_MOUSECOCOA_E737D799_3080_4B3E_A2A8_E3886D4800D1_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Input/Mouse.hpp"
#include "Pomdog/Input/MouseState.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {

class MouseCocoa final: public Mouse {
public:
	MouseCocoa();

	///@copydoc Pomdog::Mouse
	MouseState GetState() const override;

	void Position(Point2D const& position);
	void LeftButton(ButtonState buttonState);
	void RightButton(ButtonState buttonState);
	void MiddleButton(ButtonState buttonState);
	void XButton1(ButtonState buttonState);
	void XButton2(ButtonState buttonState);

	void WheelDelta(double wheelDelta);

private:
	MouseState state;
	double scrollWheel;
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MOUSECOCOA_E737D799_3080_4B3E_A2A8_E3886D4800D1_HPP)
