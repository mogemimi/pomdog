//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_POINTERPOINT_DAEC8B7F_D25B_4146_BB15_965A4AF81FF3_HPP
#define POMDOG_POINTERPOINT_DAEC8B7F_D25B_4146_BB15_965A4AF81FF3_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "PointerEventType.hpp"
#include <Pomdog/Utility/Optional.hpp>
#include <cstdint>

namespace Pomdog {
namespace UI {

enum class PointerMouseEvent {
	LeftButtonPressed,
	MiddleButtonPressed,
	RightButtonPressed,
	XButton1Pressed,
	XButton2Pressed,
	ScrollWheel,
};

class PointerPoint {
public:
	Point2D Position {0, 0};
	std::int32_t MouseWheelDelta = 0;
	PointerEventType Event = PointerEventType::CaptureLost;
	Optional<PointerMouseEvent> MouseEvent;
	std::uint32_t Id = 0;
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_POINTERPOINT_DAEC8B7F_D25B_4146_BB15_965A4AF81FF3_HPP)
