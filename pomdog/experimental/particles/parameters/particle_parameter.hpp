// Copyright mogemimi. Distributed under the MIT license.

#pragma once

namespace pomdog::random {
class Xoroshiro128StarStar;
} // namespace pomdog::random

namespace pomdog::detail::particles {

template <typename T>
class ParticleParameter {
public:
    virtual ~ParticleParameter() = default;

    virtual T Compute(float normalizedScale, random::Xoroshiro128StarStar& random) const = 0;

    virtual T Compute(float normalizedScale, float amount) const = 0;

    virtual float GenerateVariance(random::Xoroshiro128StarStar& random) const = 0;
};

} // namespace pomdog::detail::particles
