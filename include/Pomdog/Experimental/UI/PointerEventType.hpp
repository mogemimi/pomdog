// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog::UI {

enum class PointerEventType : std::uint8_t {
    Canceled,
    CaptureLost,
    Entered,
    Exited,
    Moved,
    Pressed,
    Released,
    WheelChanged,
};

} // namespace Pomdog::UI
