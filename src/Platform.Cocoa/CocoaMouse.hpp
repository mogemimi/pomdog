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
	///@copydoc Pomdog::Mouse
	MouseState const& State() const override;
	
	void Update();
	void Position(Point2D const& position);
	void ScrollWheel(std::int32_t scrollWheel);
	void LeftButton(ButtonState buttonState);
	void RightButton(ButtonState buttonState);
	void MiddleButton(ButtonState buttonState);
	void XButton1(ButtonState buttonState);
	void XButton2(ButtonState buttonState);
	
private:
	MouseState mouseState;
};

}// namespace Mouse
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_COCOA_MOUSE_E737D799_3080_4B3E_A2A8_E3886D4800D1_HPP)
