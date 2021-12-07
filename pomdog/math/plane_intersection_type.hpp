// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

/// PlaneIntersectionType indicates whether a geometry object intersects a plane.
enum class PlaneIntersectionType : std::uint8_t {
    Front,
    Back,
    Intersecting,
};

} // namespace Pomdog
