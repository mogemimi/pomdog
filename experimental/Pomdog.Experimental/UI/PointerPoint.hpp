// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/PointerEventType.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <cstdint>

namespace Pomdog {
namespace UI {

enum class PointerMouseEvent : std::uint8_t {
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

} // namespace UI
} // namespace Pomdog
