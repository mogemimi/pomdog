// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape.h"
#include "pomdog/math/turn.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

class ParticleEmitterShapeSector final : public ParticleEmitterShape {
private:
    /// The angle in turns within [0, 1] that the emission directions span,
    /// centered on the +X axis.
    Turn32 centralAngle_;

public:
    explicit ParticleEmitterShapeSector(Turn32&& centralAngleIn);

    [[nodiscard]] std::tuple<Vector3, Vector3>
    compute(random::Xoroshiro128StarStar& random) const override;
};

} // namespace pomdog::detail::particles
