// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShape.hpp"
#include "Pomdog/Experimental/Random/Xoroshiro128StarStar.hpp"
#include <random>

namespace Pomdog::Detail::Particles {

class ParticleEmitterShapeSphere final : public ParticleEmitterShape {
private:
    float radius;

public:
    explicit ParticleEmitterShapeSphere(float radiusIn)
        : radius(radiusIn)
    {
    }

    std::tuple<Vector3, Vector3>
    Compute(Random::Xoroshiro128StarStar& random) const override
    {
        POMDOG_ASSERT(radius >= 0.0f);
        std::uniform_real_distribution<float> dist(-radius, radius);

        auto emitPosition = Vector3{dist(random), dist(random), dist(random)};
        auto emitDirection = Vector3::Normalize(emitPosition);
        return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
    }
};

} // namespace Pomdog::Detail::Particles
