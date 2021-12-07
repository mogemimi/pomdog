// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

/// Indicates whether to cull primitives for hidden surface removal.
enum class CullMode : std::uint8_t {
    /// Does not cull any back faces.
    None,

    /// Culls back faces with clockwise vertices.
    ClockwiseFace,

    /// Culls back faces with counterclockwise vertices.
    CounterClockwiseFace,
};

} // namespace Pomdog
