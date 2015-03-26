// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BLENDFUNCTION_5A8D5F85_HPP
#define POMDOG_BLENDFUNCTION_5A8D5F85_HPP

#include <cstdint>

namespace Pomdog {

enum class BlendFunction: std::uint8_t {
    Add,

    Subtract,

    ReverseSubtract,

    Min,

    Max,
};

}// namespace Pomdog

#endif // POMDOG_BLENDFUNCTION_5A8D5F85_HPP
