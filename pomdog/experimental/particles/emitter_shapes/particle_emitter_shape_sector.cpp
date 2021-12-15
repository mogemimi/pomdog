// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_sector.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/random/xoroshiro128_star_star.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

ParticleEmitterShapeSector::ParticleEmitterShapeSector(Radian<float>&& centralAngleIn)
    : centralAngle(std::move(centralAngleIn))
{
}

std::tuple<Vector3, Vector3>
ParticleEmitterShapeSector::Compute(random::Xoroshiro128StarStar& random) const
{
    auto emitPosition = Vector3::Zero();

    std::uniform_real_distribution<float> distribution{-0.5f, 0.5f};
    auto emitAngle = centralAngle * distribution(random);

    auto emitDirection = Vector3{std::cos(emitAngle.value), std::sin(emitAngle.value), 0.0f};
    return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
}

} // namespace pomdog::detail::particles
