// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog::Random {
class Xoroshiro128StarStar;
} // namespace Pomdog::Random

namespace Pomdog::Detail::Particles {

template <typename T>
class ParticleParameter {
public:
    virtual ~ParticleParameter() = default;

    virtual T Compute(float normalizedScale, Random::Xoroshiro128StarStar& random) const = 0;

    virtual T Compute(float normalizedScale, float amount) const = 0;

    virtual float GenerateVariance(Random::Xoroshiro128StarStar& random) const = 0;
};

} // namespace Pomdog::Detail::Particles
