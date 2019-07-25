// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <random>

namespace Pomdog {
namespace Detail {
namespace Particles {

template <typename T>
class ParticleParameter {
public:
    virtual ~ParticleParameter() = default;

    virtual T Compute(float normalizedScale, std::mt19937& random) const = 0;

    virtual T Compute(float normalizedScale, float amount) const = 0;

    virtual float GenerateVariance(std::mt19937& random) const = 0;
};

} // namespace Particles
} // namespace Detail
} // namespace Pomdog
