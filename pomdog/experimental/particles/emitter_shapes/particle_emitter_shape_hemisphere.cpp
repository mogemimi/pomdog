// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_hemisphere.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/random/xoroshiro128_star_star.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

ParticleEmitterShapeHemisphere::ParticleEmitterShapeHemisphere(float radiusIn)
    : radius(radiusIn)
{
}

std::tuple<Vector3, Vector3>
ParticleEmitterShapeHemisphere::Compute(random::Xoroshiro128StarStar& random) const
{
    POMDOG_ASSERT(radius >= 0.0f);
    std::uniform_real_distribution<float> dist(-radius, radius);
    std::uniform_real_distribution<float> distY(0.0f, radius);

    const auto emitPosition = Vector3{dist(random), distY(random), dist(random)};
    const auto emitDirection = math::normalize(emitPosition);
    return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
}

} // namespace pomdog::detail::particles
