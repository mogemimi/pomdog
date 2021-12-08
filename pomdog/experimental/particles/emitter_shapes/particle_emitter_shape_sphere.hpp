// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape.hpp"
#include "pomdog/math/vector3.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

class ParticleEmitterShapeSphere final : public ParticleEmitterShape {
private:
    float radius;

public:
    explicit ParticleEmitterShapeSphere(float radiusIn);

    std::tuple<Vector3, Vector3>
    Compute(random::Xoroshiro128StarStar& random) const override;
};

} // namespace pomdog::detail::particles
