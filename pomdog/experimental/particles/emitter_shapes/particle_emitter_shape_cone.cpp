// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_cone.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/random/xoroshiro128_star_star.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

ParticleEmitterShapeCone::ParticleEmitterShapeCone(f32 radiusIn, f32 heightIn)
    : radius_(radiusIn)
    , height_(heightIn)
{
    // TODO: Not implemented yet.
    if (height_ <= 0.0f) {
        constexpr f32 epsilon = 0.000001f;
        height_ = epsilon;
    }
}

std::tuple<Vector3, Vector3>
ParticleEmitterShapeCone::compute(random::Xoroshiro128StarStar& random) const
{
    // TODO: Not implemented yet.
    POMDOG_ASSERT(radius_ >= 0.0f);
    POMDOG_ASSERT(height_ > 0.0f);
    std::uniform_real_distribution<f32> distHeight(0.0f, height_);

    const auto y = distHeight(random);
    const auto r = (y / height_) * radius_;

    std::uniform_real_distribution<f32> dist(-r, r);

    const auto emitPosition = Vector3{dist(random), y, dist(random)};
    const auto emitDirection = math::normalize(emitPosition);
    return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
}

} // namespace pomdog::detail::particles
