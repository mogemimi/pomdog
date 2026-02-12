// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/touch_location_state.h"
#include "pomdog/math/point2d.h"

namespace pomdog {

struct TouchLocation final {
    Point2D position;
    std::uint16_t touchID;
    TouchLocationState state = TouchLocationState::Invalid;
};

} // namespace pomdog
