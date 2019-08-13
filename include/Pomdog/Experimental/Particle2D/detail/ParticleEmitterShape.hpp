// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include <random>

namespace Pomdog::Detail::Particles {

class ParticleEmitterShape {
public:
    virtual ~ParticleEmitterShape() = default;

    virtual void Compute(std::mt19937& random, Vector3& emitPosition, Radian<float>& emitAngle) const = 0;
};

} // namespace Pomdog::Detail::Particles
