// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "ParticleEmitterShape.hpp"
#include <random>

namespace Pomdog {
namespace Detail {
namespace Particles {

class ParticleEmitterShapeSector final: public ParticleEmitterShape {
private:
    ///@note range: [0, 2*Pi]
    Radian<float> centralAngle;

public:
    explicit ParticleEmitterShapeSector(Radian<float> && centralAngleIn)
        : centralAngle(std::move(centralAngleIn))
    {}

    void Compute(std::mt19937 & random, Vector2 & emitPosition, Radian<float> & emitAngle) const override
    {
        emitPosition = {0, 0};

        std::uniform_real_distribution<float> distribution(-0.5f, 0.5f);
        emitAngle = centralAngle * distribution(random);
    }
};

} // namespace Particles
} // namespace Detail
} // namespace Pomdog
