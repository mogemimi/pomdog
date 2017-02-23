// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {
namespace UI {

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

} // namespace UI
} // namespace Pomdog
