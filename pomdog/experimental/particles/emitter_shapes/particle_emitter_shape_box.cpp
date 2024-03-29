// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_box.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/random/xoroshiro128_star_star.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::particles {

ParticleEmitterShapeBox::ParticleEmitterShapeBox(const Vector3& size)
    : halfSize(size * 0.5f)
{
}

std::tuple<Vector3, Vector3>
ParticleEmitterShapeBox::Compute(random::Xoroshiro128StarStar& random) const
{
    POMDOG_ASSERT(halfSize.x >= 0.0f);
    POMDOG_ASSERT(halfSize.y >= 0.0f);
    POMDOG_ASSERT(halfSize.z >= 0.0f);

    std::uniform_real_distribution<float> distX(-halfSize.x, halfSize.x);
    std::uniform_real_distribution<float> distY(-halfSize.y, halfSize.y);
    std::uniform_real_distribution<float> distZ(-halfSize.z, halfSize.z);

    auto emitPosition = Vector3{distX(random), distY(random), distZ(random)};
    auto emitDirection = Vector3{1.0f, 0.0f, 0.0f};
    return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
}

} // namespace pomdog::detail::particles
