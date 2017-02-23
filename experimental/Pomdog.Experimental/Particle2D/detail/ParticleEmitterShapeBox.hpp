// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "ParticleEmitterShape.hpp"
#include <random>

namespace Pomdog {
namespace Detail {
namespace Particles {

class ParticleEmitterShapeBox final: public ParticleEmitterShape {
private:
    float width;
    float height;

public:
    ParticleEmitterShapeBox(float widthIn, float heightIn)
        : width(widthIn)
        , height(heightIn)
    {}

    void Compute(std::mt19937 & random, Vector2 & emitPosition, Radian<float> & emitAngle) const override
    {
        POMDOG_ASSERT(width >= 0.0f);
        POMDOG_ASSERT(height >= 0.0f);

        std::uniform_real_distribution<float> distribution(-0.5f, 0.5f);
        emitPosition.X = width * distribution(random);
        emitPosition.Y = height * distribution(random);
        emitAngle = 0.0f;
    }
};

} // namespace Particles
} // namespace Detail
} // namespace Pomdog
