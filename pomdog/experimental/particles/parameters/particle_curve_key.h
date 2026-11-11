// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::detail::particles {

template <typename T>
struct ParticleCurveKey {
    f32 timeSeconds;
    T value;
};

} // namespace pomdog::detail::particles
