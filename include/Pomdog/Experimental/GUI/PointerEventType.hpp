// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
