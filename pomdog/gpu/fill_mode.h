// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

/// Determines the fill mode to use when rendering primitives.
enum class FillMode : std::uint8_t {
    /// Rasterizes triangle primitives as filled triangles.
    Solid,

    /// Draws polygon edges as line segments.
    Wireframe,
};

} // namespace pomdog::gpu
