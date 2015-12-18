// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_POINTERPOINT_DAEC8B7F_HPP
#define POMDOG_POINTERPOINT_DAEC8B7F_HPP

#include "PointerEventType.hpp"
#include "Pomdog/Utility/Optional.hpp"
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

#endif // POMDOG_POINTERPOINT_DAEC8B7F_HPP
