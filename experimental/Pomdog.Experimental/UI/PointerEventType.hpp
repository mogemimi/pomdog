// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_POINTEREVENTTYPE_2ABE882F_69A8_4F0D_AE08_A712EE914EA0_HPP
#define POMDOG_POINTEREVENTTYPE_2ABE882F_69A8_4F0D_AE08_A712EE914EA0_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {
namespace UI {

enum class PointerEventType: std::uint8_t {
	Canceled,
	CaptureLost,
	Entered,
	Exited,
	Moved,
	Pressed,
	Released,
	WheelChanged,
};

}// namespace UI
}// namespace Pomdog

#endif // !defined(POMDOG_POINTEREVENTTYPE_2ABE882F_69A8_4F0D_AE08_A712EE914EA0_HPP)
