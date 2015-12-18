// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_INPUTELEMENTFORMAT_9DA1879C_HPP
#define POMDOG_INPUTELEMENTFORMAT_9DA1879C_HPP

#include <cstdint>

namespace Pomdog {

enum class InputElementFormat : std::uint8_t {
    ///@brief A single-component, 32-bit floating-point format.
    Float,

    ///@brief A two-component, 64-bit floating-point format.
    Float2,

    ///@brief A three-componet, 96-bit floating-point format.
    Float3,

    ///@brief A four-component, 128-bit floating-point format
    Float4,

    ///@brief A four-component, 128-bit integer format
    Int4,

    ///@brief A four-component, 128-bit unsigned integer format
    //UInt4,

    ///@brief A four-component, 32-bit unsigned integer format.
    Byte4,

    ///@deprecated Not supported
    ///@brief A two-component, 32-bit floating-point format.
    HalfFloat2,

    ///@deprecated Not supported
    ///@brief A four-component, 64-bit floating-point format.
    HalfFloat4,
};

} // namespace Pomdog

#endif // POMDOG_INPUTELEMENTFORMAT_9DA1879C_HPP
