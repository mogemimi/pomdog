// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/particles/beam_system.h"
#include "pomdog/math/turn.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::BeamSystem;
using pomdog::Turn32;
using pomdog::Vector2;

TEST_CASE("BeamSystem::createTarget")
{
    SUBCASE("quarter turns are exact")
    {
        const auto origin = Vector2{1.0f, 2.0f};
        REQUIRE_EQ(BeamSystem::createTarget(origin, Turn32{0.0f}, 10.0f), Vector2{11.0f, 2.0f});
        REQUIRE_EQ(BeamSystem::createTarget(origin, Turn32{0.25f}, 10.0f), Vector2{1.0f, 12.0f});
        REQUIRE_EQ(BeamSystem::createTarget(origin, Turn32{0.5f}, 10.0f), Vector2{-9.0f, 2.0f});
        REQUIRE_EQ(BeamSystem::createTarget(origin, Turn32{-0.25f}, 10.0f), Vector2{1.0f, -8.0f});
    }
    SUBCASE("eighth turn")
    {
        const auto target = BeamSystem::createTarget(Vector2{0.0f, 0.0f}, Turn32{0.125f}, 2.0f);
        REQUIRE(target.x == doctest::Approx(1.4142135));
        REQUIRE(target.y == doctest::Approx(1.4142135));
    }
}
