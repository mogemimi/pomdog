// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_sector.h"
#include "pomdog/math/turn.h"
#include "pomdog/math/vector3.h"
#include "pomdog/random/xoroshiro128_star_star.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::i32;
using pomdog::Turn32;
using pomdog::Vector3;
using pomdog::detail::particles::ParticleEmitterShapeSector;
using pomdog::random::Xoroshiro128StarStar;
namespace math = pomdog::math;

TEST_CASE("ParticleEmitterShapeSector")
{
    Xoroshiro128StarStar random{42};

    SUBCASE("zero central angle emits along +X")
    {
        const ParticleEmitterShapeSector sector{Turn32{0.0f}};
        for (i32 i = 0; i < 16; ++i) {
            const auto [position, direction] = sector.compute(random);
            REQUIRE_EQ(position, Vector3{0.0f, 0.0f, 0.0f});
            REQUIRE_EQ(direction, Vector3{1.0f, 0.0f, 0.0f});
        }
    }
    SUBCASE("quarter turn spans 45 degrees on each side of +X")
    {
        const ParticleEmitterShapeSector sector{Turn32{0.25f}};
        for (i32 i = 0; i < 256; ++i) {
            const auto [position, direction] = sector.compute(random);
            CAPTURE(direction.x);
            CAPTURE(direction.y);
            REQUIRE(math::length(direction) == doctest::Approx(1.0f));
            REQUIRE(direction.x >= 0.70710678f - 1e-6f);
            REQUIRE(direction.z == 0.0f);
        }
    }
}
