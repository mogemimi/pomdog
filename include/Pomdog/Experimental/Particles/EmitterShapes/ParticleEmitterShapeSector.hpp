// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShape.hpp"
#include "Pomdog/Experimental/Random/Xoroshiro128StarStar.hpp"
#include "Pomdog/Math/Radian.hpp"
#include <cmath>
#include <random>

namespace Pomdog::Detail::Particles {

class ParticleEmitterShapeSector final : public ParticleEmitterShape {
private:
    ///@note range: [0, 2*Pi]
    Radian<float> centralAngle;

public:
    explicit ParticleEmitterShapeSector(Radian<float>&& centralAngleIn)
        : centralAngle(std::move(centralAngleIn))
    {
    }

    std::tuple<Vector3, Vector3>
    Compute(Random::Xoroshiro128StarStar& random) const override
    {
        auto emitPosition = Vector3::Zero;

        std::uniform_real_distribution<float> distribution{-0.5f, 0.5f};
        auto emitAngle = centralAngle * distribution(random);

        auto emitDirection = Vector3{std::cos(emitAngle.value), std::sin(emitAngle.value), 0.0f};
        return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
    }
};

} // namespace Pomdog::Detail::Particles
