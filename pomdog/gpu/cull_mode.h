// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// Indicates whether to cull primitives for hidden surface removal.
enum class CullMode : u8 {
    /// Does not cull any back faces.
    None,

    /// Culls back faces with clockwise vertices.
    ClockwiseFace,

    /// Culls back faces with counterclockwise vertices.
    CounterClockwiseFace,
};

} // namespace pomdog::gpu
