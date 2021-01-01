// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

/// Determines the fill mode to use when rendering primitives.
enum class FillMode : std::uint8_t {
    /// Draws polygon edges as line segments.
    WireFrame,

    /// Rasterizes triangle primitives as filled triangles.
    Solid,
};

} // namespace Pomdog
