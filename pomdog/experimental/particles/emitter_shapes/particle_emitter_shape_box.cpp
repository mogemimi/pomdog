// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_box.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/random/xoroshiro128_star_star.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

ParticleEmitterShapeBox::ParticleEmitterShapeBox(const Vector3& size)
    : halfSize_(size * 0.5f)
{
}

std::tuple<Vector3, Vector3>
ParticleEmitterShapeBox::compute(random::Xoroshiro128StarStar& random) const
{
    POMDOG_ASSERT(halfSize_.x >= 0.0f);
    POMDOG_ASSERT(halfSize_.y >= 0.0f);
    POMDOG_ASSERT(halfSize_.z >= 0.0f);

    std::uniform_real_distribution<f32> distX(-halfSize_.x, halfSize_.x);
    std::uniform_real_distribution<f32> distY(-halfSize_.y, halfSize_.y);
    std::uniform_real_distribution<f32> distZ(-halfSize_.z, halfSize_.z);

    auto emitPosition = Vector3{distX(random), distY(random), distZ(random)};
    auto emitDirection = Vector3{1.0f, 0.0f, 0.0f};
    return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
}

} // namespace pomdog::detail::particles
