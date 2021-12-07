// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape.hpp"
#include "pomdog/math/radian.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Particles {

class ParticleEmitterShapeSector final : public ParticleEmitterShape {
private:
    ///@note range: [0, 2*Pi]
    Radian<float> centralAngle;

public:
    explicit ParticleEmitterShapeSector(Radian<float>&& centralAngleIn);

    std::tuple<Vector3, Vector3>
    Compute(Random::Xoroshiro128StarStar& random) const override;
};

} // namespace Pomdog::Detail::Particles
