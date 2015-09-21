// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLEPARAMETER_FCCEE8BC_HPP
#define POMDOG_PARTICLEPARAMETER_FCCEE8BC_HPP

#include <random>

namespace Pomdog {
namespace Detail {
namespace Particles {

template <typename T>
class ParticleParameter {
public:
    virtual ~ParticleParameter() = default;

    virtual T Compute(float normalizedScale, std::mt19937 & random) const = 0;

    virtual T Compute(float normalizedScale, float amount) const = 0;

    virtual float GenerateVariance(std::mt19937 & random) const = 0;
};

} // namespace Particles
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_PARTICLEPARAMETER_FCCEE8BC_HPP
