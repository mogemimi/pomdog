// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TOUCHLOCATIONSTATE_C896CADD_HPP
#define POMDOG_TOUCHLOCATIONSTATE_C896CADD_HPP

#include <cstdint>

namespace Pomdog {

enum class TouchLocationState: std::uint8_t {
    Invalid,
    Moved,
    Pressed,
    Released,
};

}// namespace Pomdog

#endif // POMDOG_TOUCHLOCATIONSTATE_C896CADD_HPP
