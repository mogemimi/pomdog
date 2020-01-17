// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShape.hpp"
#include "Pomdog/Experimental/Random/Xoroshiro128StarStar.hpp"
#include <random>

namespace Pomdog::Detail::Particles {

// TODO: not implemented yet
class ParticleEmitterShapeCone final : public ParticleEmitterShape {
private:
    float radius;
    float height;

public:
    ParticleEmitterShapeCone(float radiusIn, float heightIn)
        : radius(radiusIn)
        , height(heightIn)
    {
        if (height <= 0.0f) {
            constexpr float epsilon = 0.000001f;
            this->height = epsilon;
        }
    }

    std::tuple<Vector3, Vector3>
    Compute(Random::Xoroshiro128StarStar& random) const override
    {
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
};

} // namespace Pomdog::Detail::Particles
