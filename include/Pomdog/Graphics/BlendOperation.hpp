// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class BlendOperation : std::uint8_t {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
};

} // namespace Pomdog
