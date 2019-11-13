// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class PrimitiveTopology : std::uint8_t {
    /// A triangle list.
    TriangleList,

    /// A triangle strip.
    TriangleStrip,

    /// A line list.
    LineList,

    /// A line strip.
    LineStrip,
};

} // namespace Pomdog
