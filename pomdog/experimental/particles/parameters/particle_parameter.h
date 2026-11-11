// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::random {
class Xoroshiro128StarStar;
} // namespace pomdog::random

namespace pomdog::detail::particles {

template <typename T>
class ParticleParameter {
public:
    virtual ~ParticleParameter() = default;

    [[nodiscard]] virtual T
    compute(f32 normalizedScale, random::Xoroshiro128StarStar& random) const = 0;

    [[nodiscard]] virtual T
    compute(f32 normalizedScale, f32 amount) const = 0;

    [[nodiscard]] virtual f32
    generateVariance(random::Xoroshiro128StarStar& random) const = 0;
};

} // namespace pomdog::detail::particles
