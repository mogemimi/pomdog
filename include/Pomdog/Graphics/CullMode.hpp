// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

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
