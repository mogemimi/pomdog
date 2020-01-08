// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShape.hpp"
#include "Pomdog/Experimental/Random/Xoroshiro128StarStar.hpp"
#include <random>

namespace Pomdog::Detail::Particles {

class ParticleEmitterShapeBox final : public ParticleEmitterShape {
private:
    Vector3 halfSize;

public:
    explicit ParticleEmitterShapeBox(const Vector3& size)
        : halfSize(size * 0.5f)
    {
    }

    std::tuple<Vector3, Vector3>
    Compute(Random::Xoroshiro128StarStar& random) const override
    {
        POMDOG_ASSERT(halfSize.X >= 0.0f);
        POMDOG_ASSERT(halfSize.Y >= 0.0f);
        POMDOG_ASSERT(halfSize.Z >= 0.0f);

        std::uniform_real_distribution<float> distX(-halfSize.X, halfSize.X);
        std::uniform_real_distribution<float> distY(-halfSize.Y, halfSize.Y);
        std::uniform_real_distribution<float> distZ(-halfSize.Z, halfSize.Z);

        auto emitPosition = Vector3{distX(random), distY(random), distZ(random)};
        auto emitDirection = Vector3{1.0f, 0.0f, 0.0f};
        return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
    }
};

} // namespace Pomdog::Detail::Particles
