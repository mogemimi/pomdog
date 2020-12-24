// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShape.hpp"
#include <tuple>

namespace Pomdog::Detail::Particles {

class ParticleEmitterShapeHemisphere final : public ParticleEmitterShape {
private:
    float radius;

public:
    explicit ParticleEmitterShapeHemisphere(float radiusIn);

    std::tuple<Vector3, Vector3>
    Compute(Random::Xoroshiro128StarStar& random) const override;
};

} // namespace Pomdog::Detail::Particles
