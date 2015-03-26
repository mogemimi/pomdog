// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CULLMODE_DBEE9F5F_HPP
#define POMDOG_CULLMODE_DBEE9F5F_HPP

#include <cstdint>

namespace Pomdog {

enum class CullMode: std::uint8_t {
    None,

    ClockwiseFace,

    CounterClockwiseFace,
};

}// namespace Pomdog

#endif // POMDOG_CULLMODE_DBEE9F5F_HPP
