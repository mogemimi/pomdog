// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_hemisphere.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/random/xoroshiro128_star_star.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

ParticleEmitterShapeHemisphere::ParticleEmitterShapeHemisphere(f32 radiusIn)
    : radius_(radiusIn)
{
}

std::tuple<Vector3, Vector3>
ParticleEmitterShapeHemisphere::compute(random::Xoroshiro128StarStar& random) const
{
    POMDOG_ASSERT(radius_ >= 0.0f);
    std::uniform_real_distribution<f32> dist(-radius_, radius_);
    std::uniform_real_distribution<f32> distY(0.0f, radius_);

    const auto emitPosition = Vector3{dist(random), distY(random), dist(random)};
    const auto emitDirection = math::normalize(emitPosition);
    return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
}

} // namespace pomdog::detail::particles
