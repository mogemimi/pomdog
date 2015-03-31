// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_COMPARISONFUNCTION_01201CF2_HPP
#define POMDOG_COMPARISONFUNCTION_01201CF2_HPP

#include <cstdint>

namespace Pomdog {

enum class ComparisonFunction: std::uint8_t {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
};

} // namespace Pomdog

#endif // POMDOG_COMPARISONFUNCTION_01201CF2_HPP
