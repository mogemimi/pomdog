// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShape.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
