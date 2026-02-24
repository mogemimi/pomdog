// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// Determines the fill mode to use when rendering primitives.
enum class FillMode : u8 {
    /// Rasterizes triangle primitives as filled triangles.
    Solid,

    /// Draws polygon edges as line segments.
    Wireframe,
};

} // namespace pomdog::gpu
