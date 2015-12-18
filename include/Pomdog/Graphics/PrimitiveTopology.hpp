// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_PRIMITIVETOPOLOGY_46F3C71C_HPP
#define POMDOG_PRIMITIVETOPOLOGY_46F3C71C_HPP

#include <cstdint>

namespace Pomdog {

enum class PrimitiveTopology: std::uint8_t {
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

#endif // POMDOG_PRIMITIVETOPOLOGY_46F3C71C_HPP
