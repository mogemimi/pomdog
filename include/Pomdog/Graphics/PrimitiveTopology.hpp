// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class PrimitiveTopology : std::uint8_t {
    ///@brief A triangle list.
    TriangleList,

    ///@brief A triangle strip.
    TriangleStrip,

    ///@brief A line list.
    LineList,

    ///@brief A line strip.
    LineStrip,
};

} // namespace Pomdog
