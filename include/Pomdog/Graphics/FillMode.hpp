// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

/// Determines the fill mode to use when rendering primitives.
enum class FillMode : std::uint8_t {
    /// Draws polygon edges as line segments.
    WireFrame,

    /// Rasterizes triangle primitives as filled triangles.
    Solid,
};

} // namespace Pomdog
