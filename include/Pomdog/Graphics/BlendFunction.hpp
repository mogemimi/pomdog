// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class BlendFunction: std::uint8_t {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
};

} // namespace Pomdog
