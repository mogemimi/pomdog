// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShape.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include <tuple>

namespace Pomdog::Detail::Particles {

class ParticleEmitterShapeCone final : public ParticleEmitterShape {
private:
    float radius;
    float height;

public:
    ParticleEmitterShapeCone(float radiusIn, float heightIn);

    std::tuple<Vector3, Vector3>
    Compute(Random::Xoroshiro128StarStar& random) const override;
};

} // namespace Pomdog::Detail::Particles
