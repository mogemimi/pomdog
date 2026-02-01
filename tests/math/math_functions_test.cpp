// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/radian.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace math = pomdog::math;
using pomdog::f32;
using pomdog::f64;

TEST_CASE("math_functions")
{
    SUBCASE("clamp(f32)")
    {
        REQUIRE(math::clamp(0.0f, 0.0f, 1.0f) == doctest::Approx(0.0).epsilon(0.00001));
        REQUIRE(math::clamp(1.0f, 0.0f, 1.0f) == doctest::Approx(1.0).epsilon(0.00001));
        REQUIRE(math::clamp(2.0f, 0.0f, 1.0f) == doctest::Approx(1.0).epsilon(0.00001));
        REQUIRE(math::clamp(-1.0f, 0.0f, 1.0f) == doctest::Approx(0.0).epsilon(0.00001));
        REQUIRE(math::clamp(0.42f, 0.0f, 1.0f) == doctest::Approx(0.42).epsilon(0.00001));
        REQUIRE(math::clamp(-0.42f, -1.0f, 1.0f) == doctest::Approx(-0.42).epsilon(0.00001));
    }
    SUBCASE("clamp(f64)")
    {
        REQUIRE(math::clamp(0.0, 0.0, 1.0) == doctest::Approx(0.0).epsilon(0.0000001));
        REQUIRE(math::clamp(1.0, 0.0, 1.0) == doctest::Approx(1.0).epsilon(0.0000001));
        REQUIRE(math::clamp(2.0, 0.0, 1.0) == doctest::Approx(1.0).epsilon(0.0000001));
        REQUIRE(math::clamp(-1.0, 0.0, 1.0) == doctest::Approx(0.0).epsilon(0.0000001));
        REQUIRE(math::clamp(0.42, 0.0, 1.0) == doctest::Approx(0.42).epsilon(0.0000001));
        REQUIRE(math::clamp(-0.42, -1.0, 1.0) == doctest::Approx(-0.42).epsilon(0.0000001));
    }
    SUBCASE("saturate(f32)")
    {
        REQUIRE(math::saturate(0.0f) == doctest::Approx(0.0).epsilon(0.00001));
        REQUIRE(math::saturate(1.0f) == doctest::Approx(1.0).epsilon(0.00001));
        REQUIRE(math::saturate(2.0f) == doctest::Approx(1.0).epsilon(0.00001));
        REQUIRE(math::saturate(-1.0f) == doctest::Approx(0.0).epsilon(0.00001));
        REQUIRE(math::saturate(0.42f) == doctest::Approx(0.42).epsilon(0.00001));
        REQUIRE(math::saturate(-0.42f) == doctest::Approx(0.0).epsilon(0.00001));
        REQUIRE(math::saturate(0.5f) == doctest::Approx(0.5).epsilon(0.00001));
    }
    SUBCASE("saturate(f64)")
    {
        REQUIRE(math::saturate(0.0) == doctest::Approx(0.0).epsilon(0.00000001));
        REQUIRE(math::saturate(1.0) == doctest::Approx(1.0).epsilon(0.00000001));
        REQUIRE(math::saturate(2.0) == doctest::Approx(1.0).epsilon(0.00000001));
        REQUIRE(math::saturate(-1.0) == doctest::Approx(0.0).epsilon(0.00000001));
        REQUIRE(math::saturate(0.42) == doctest::Approx(0.42).epsilon(0.00000001));
        REQUIRE(math::saturate(-0.42) == doctest::Approx(0.0).epsilon(0.00000001));
    }
    SUBCASE("lerp")
    {
        REQUIRE(math::lerp(0.0f, 1.0f, 0.0f) == doctest::Approx(0.0).epsilon(0.00001));
        REQUIRE(math::lerp(0.0f, 1.0f, 1.0f) == doctest::Approx(1.0).epsilon(0.00001));
        REQUIRE(math::lerp(0.0f, 1.0f, 0.5f) == doctest::Approx(0.5).epsilon(0.00001));
        REQUIRE(math::lerp(0.0f, 1.0f, 0.25f) == doctest::Approx(0.25).epsilon(0.00001));
        REQUIRE(math::lerp(0.0f, 1.0f, 0.75f) == doctest::Approx(0.75).epsilon(0.00001));
        REQUIRE(math::lerp(1.0f, 2.0f, 0.5f) == doctest::Approx(1.5).epsilon(0.00001));
        REQUIRE(math::lerp(-1.0f, 1.0f, 0.5f) == doctest::Approx(0.0).epsilon(0.00001));
    }
    SUBCASE("smoothstep")
    {
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.0f) == doctest::Approx(0.0).epsilon(0.00001));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 1.0f) == doctest::Approx(1.0).epsilon(0.00001));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.5f) == doctest::Approx(0.5).epsilon(0.00001));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.25f) == doctest::Approx(0.15625).epsilon(0.00001));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.75f) == doctest::Approx(0.84375).epsilon(0.00001));
    }
}
