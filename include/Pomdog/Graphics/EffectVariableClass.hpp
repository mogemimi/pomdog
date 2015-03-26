// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTVARIABLECLASS_0B44E0FB_HPP
#define POMDOG_EFFECTVARIABLECLASS_0B44E0FB_HPP

#include <cstdint>

namespace Pomdog {

enum class EffectVariableClass: std::uint8_t {
    Scalar,

    Vector,

    Matrix,

    Object,

    Struct,
};

}// namespace Pomdog

#endif // POMDOG_EFFECTVARIABLECLASS_0B44E0FB_HPP
