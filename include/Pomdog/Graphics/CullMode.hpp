// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class CullMode : std::uint8_t {
    None,
    ClockwiseFace,
    CounterClockwiseFace,
};

} // namespace Pomdog
