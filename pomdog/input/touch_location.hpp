// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/input/touch_location_state.hpp"
#include "pomdog/math/point2d.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct TouchLocation final {
    Point2D Position;
    std::uint16_t Id;
    TouchLocationState State = TouchLocationState::Invalid;
};

} // namespace pomdog
