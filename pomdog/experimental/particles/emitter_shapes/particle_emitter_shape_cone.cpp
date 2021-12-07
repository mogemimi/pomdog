// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_cone.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/random/xoroshiro128_star_star.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Particles {

ParticleEmitterShapeCone::ParticleEmitterShapeCone(float radiusIn, float heightIn)
    : radius(radiusIn)
    , height(heightIn)
{
    // TODO: Not implemented yet.
    if (height <= 0.0f) {
        constexpr float epsilon = 0.000001f;
        this->height = epsilon;
    }
}

std::tuple<Vector3, Vector3>
ParticleEmitterShapeCone::Compute(Random::Xoroshiro128StarStar& random) const
{
    // TODO: Not implemented yet.
    POMDOG_ASSERT(radius >= 0.0f);
    POMDOG_ASSERT(height > 0.0f);
    std::uniform_real_distribution<float> distHeight(0.0f, height);

    const auto y = distHeight(random);
    const auto r = (y / height) * radius;

    std::uniform_real_distribution<float> dist(-r, r);

    auto emitPosition = Vector3{dist(random), y, dist(random)};
    auto emitDirection = Vector3::Normalize(emitPosition);
    return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
}

} // namespace Pomdog::Detail::Particles
