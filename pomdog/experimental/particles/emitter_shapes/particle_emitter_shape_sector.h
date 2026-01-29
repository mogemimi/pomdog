// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape.h"
#include "pomdog/math/radian.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

class ParticleEmitterShapeSector final : public ParticleEmitterShape {
private:
    ///@note range: [0, 2*Pi]
    Radian<f32> centralAngle;

public:
    explicit ParticleEmitterShapeSector(Radian<f32>&& centralAngleIn);

    std::tuple<Vector3, Vector3>
    Compute(random::Xoroshiro128StarStar& random) const override;
};

} // namespace pomdog::detail::particles
