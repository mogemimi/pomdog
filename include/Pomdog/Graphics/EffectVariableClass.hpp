// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class EffectVariableClass : std::uint8_t {
    Scalar,
    Vector,
    Matrix,
    Object,
    Struct,
};

} // namespace Pomdog
