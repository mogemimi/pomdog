// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/GUI/PointerEventType.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include <cstdint>
#include <optional>

namespace Pomdog::GUI {

enum class PointerMouseEvent : std::uint8_t {
    LeftButtonPressed,
    MiddleButtonPressed,
    RightButtonPressed,
    XButton1Pressed,
    XButton2Pressed,
    ScrollWheel,
};

class PointerPoint final {
public:
    Point2D Position{0, 0};
    std::int32_t MouseWheelDelta = 0;
    PointerEventType Event = PointerEventType::CaptureLost;
    std::optional<PointerMouseEvent> MouseEvent;
    std::uint32_t ID = 0;
    std::int32_t ClickCount = 0;
};

} // namespace Pomdog::GUI
