// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShape.hpp"
#include "Pomdog/Experimental/Random/Xoroshiro128StarStar.hpp"
#include <random>

namespace Pomdog::Detail::Particles {

class ParticleEmitterShapeHemisphere final : public ParticleEmitterShape {
private:
    float radius;

public:
    explicit ParticleEmitterShapeHemisphere(float radiusIn)
        : radius(radiusIn)
    {
    }

    std::tuple<Vector3, Vector3>
    Compute(Random::Xoroshiro128StarStar& random) const override
    {
        POMDOG_ASSERT(radius >= 0.0f);
        std::uniform_real_distribution<float> dist(-radius, radius);
        std::uniform_real_distribution<float> distY(0.0f, radius);

        auto emitPosition = Vector3{dist(random), distY(random), dist(random)};
        auto emitDirection = Vector3::Normalize(emitPosition);
        return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
    }
};

} // namespace Pomdog::Detail::Particles
