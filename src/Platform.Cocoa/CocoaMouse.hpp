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

class CocoaMouse final: public Pomdog::Mouse
{
public:
	///@copydoc Pomdog::Mouse
	MouseState const& State() const override;
	
	void SetPosition(Point2D const& position);
	void SetScrollWheel(std::int32_t scrollWheel);
	void SetLeftButton(ButtonState buttonState);
	void SetRightButton(ButtonState buttonState);
	void SetMiddleButton(ButtonState buttonState);
	void SetXButton1(ButtonState buttonState);
	void SetXButton2(ButtonState buttonState);
	
private:
	MouseState mouseState;
};

}// namespace Mouse
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_COCOA_MOUSE_E737D799_3080_4B3E_A2A8_E3886D4800D1_HPP)
