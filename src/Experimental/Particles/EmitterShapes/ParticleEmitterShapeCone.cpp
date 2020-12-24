// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShapeCone.hpp"
#include "Pomdog/Experimental/Random/Xoroshiro128StarStar.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <random>

namespace Pomdog::Detail::Particles {

ParticleEmitterShapeCone::ParticleEmitterShapeCone(float radiusIn, float heightIn)
    : radius(radiusIn)
    , height(heightIn)
{
    // TODO: Not implemented yet.
    if (height <= 0.0f) {
        constexpr float epsilon = 0.000001f;
        this->height = epsilon;
    }
}

std::tuple<Vector3, Vector3>
ParticleEmitterShapeCone::Compute(Random::Xoroshiro128StarStar& random) const
{
    // TODO: Not implemented yet.
    POMDOG_ASSERT(radius >= 0.0f);
    POMDOG_ASSERT(height > 0.0f);
    std::uniform_real_distribution<float> distHeight(0.0f, height);

    const auto y = distHeight(random);
    const auto r = (y / height) * radius;

    std::uniform_real_distribution<float> dist(-r, r);

    auto emitPosition = Vector3{dist(random), y, dist(random)};
    auto emitDirection = Vector3::Normalize(emitPosition);
    return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
}

} // namespace Pomdog::Detail::Particles
