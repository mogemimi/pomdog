// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/gui/pointer_event_type.hpp"
#include "pomdog/math/point2d.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
