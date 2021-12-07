// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_sphere.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/random/xoroshiro128_star_star.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Particles {

ParticleEmitterShapeSphere::ParticleEmitterShapeSphere(float radiusIn)
    : radius(radiusIn)
{
}

std::tuple<Vector3, Vector3>
ParticleEmitterShapeSphere::Compute(Random::Xoroshiro128StarStar& random) const
{
    POMDOG_ASSERT(radius >= 0.0f);
    std::uniform_real_distribution<float> dist(-radius, radius);

    auto emitPosition = Vector3{dist(random), dist(random), dist(random)};
    auto emitDirection = Vector3::Normalize(emitPosition);
    return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
}

} // namespace Pomdog::Detail::Particles
