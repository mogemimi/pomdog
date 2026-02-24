// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

enum class EffectVariableClass : u8 {
    Scalar,
    Vector,
    Matrix,
    Object,
    Struct,
};

} // namespace pomdog::gpu
