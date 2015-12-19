// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class IndexElementSize : std::uint8_t {
    ///@brief 16 bit.
    SixteenBits,

    ///@brief 32 bit.
    ThirtyTwoBits,
};

} // namespace Pomdog
