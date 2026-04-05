// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gui {

enum class PointerEventType : u8 {
    Canceled,
    CaptureLost,
    Entered,
    Exited,
    Moved,
    Pressed,
    Released,
    WheelChanged,
};

} // namespace pomdog::gui
