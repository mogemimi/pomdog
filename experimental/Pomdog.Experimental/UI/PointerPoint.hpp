// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/UI/PointerEventType.hpp"
#include <cstdint>
#include <optional>

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
    std::optional<PointerMouseEvent> MouseEvent;
    std::uint32_t Id = 0;
};

} // namespace UI
} // namespace Pomdog
