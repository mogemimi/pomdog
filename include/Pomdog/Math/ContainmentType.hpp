// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CONTAINMENTTYPE_21BFFFDA_HPP
#define POMDOG_CONTAINMENTTYPE_21BFFFDA_HPP

#include <cstdint>

namespace Pomdog {

enum class ContainmentType: std::uint8_t {
    Contains,

    Disjoint,

    Intersects
};

} // namespace Pomdog

#endif // POMDOG_CONTAINMENTTYPE_21BFFFDA_HPP
