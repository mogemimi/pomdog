// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// PlaneIntersectionType indicates whether a geometry object intersects a plane.
enum class PlaneIntersectionType : std::uint8_t {
    Front,
    Back,
    Intersecting,
};

} // namespace pomdog
