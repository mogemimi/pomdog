// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/gui/pointer_event_type.h"
#include "pomdog/math/point2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {

enum class PointerMouseEvent : u8 {
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
    i32 MouseWheelDelta = 0;
    PointerEventType Event = PointerEventType::CaptureLost;
    std::optional<PointerMouseEvent> MouseEvent;
    u32 ID = 0;
    i32 ClickCount = 0;
};

} // namespace pomdog::gui
