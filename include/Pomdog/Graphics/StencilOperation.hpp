// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_STENCILOPERATION_6EB1EB6D_HPP
#define POMDOG_STENCILOPERATION_6EB1EB6D_HPP

#include <cstdint>

namespace Pomdog {

enum class StencilOperation: std::uint8_t {
    Keep,
    Zero,
    Replace,
    IncrementSaturation,
    DecrementSaturation,
    Invert,
    Increment,
    Decrement,
};

} // namespace Pomdog

#endif // POMDOG_STENCILOPERATION_6EB1EB6D_HPP
