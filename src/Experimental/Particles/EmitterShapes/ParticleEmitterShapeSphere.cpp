// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShapeSphere.hpp"
#include "Pomdog/Experimental/Random/Xoroshiro128StarStar.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <random>

namespace Pomdog::Detail::Particles {

ParticleEmitterShapeSphere::ParticleEmitterShapeSphere(float radiusIn)
    : radius(radiusIn)
{
}

std::tuple<Vector3, Vector3>
ParticleEmitterShapeSphere::Compute(Random::Xoroshiro128StarStar& random) const
{
    POMDOG_ASSERT(radius >= 0.0f);
    std::uniform_real_distribution<float> dist(-radius, radius);

    auto emitPosition = Vector3{dist(random), dist(random), dist(random)};
    auto emitDirection = Vector3::Normalize(emitPosition);
    return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
}

} // namespace Pomdog::Detail::Particles
