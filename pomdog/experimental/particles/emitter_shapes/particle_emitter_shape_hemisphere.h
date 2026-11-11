// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

class ParticleEmitterShapeHemisphere final : public ParticleEmitterShape {
private:
    f32 radius_;

public:
    explicit ParticleEmitterShapeHemisphere(f32 radiusIn);

    [[nodiscard]] std::tuple<Vector3, Vector3>
    compute(random::Xoroshiro128StarStar& random) const override;
};

} // namespace pomdog::detail::particles
