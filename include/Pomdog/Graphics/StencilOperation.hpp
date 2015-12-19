// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class StencilOperation : std::uint8_t {
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
