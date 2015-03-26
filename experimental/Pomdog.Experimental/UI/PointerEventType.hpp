// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_POINTEREVENTTYPE_2ABE882F_HPP
#define POMDOG_POINTEREVENTTYPE_2ABE882F_HPP

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

#endif // POMDOG_POINTEREVENTTYPE_2ABE882F_HPP
