// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog::GUI {

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

} // namespace Pomdog::GUI
