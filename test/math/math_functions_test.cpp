// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/degree.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/radian.h"
#include <catch_amalgamated.hpp>

namespace math = pomdog::math;
using pomdog::Degree;
using pomdog::f32;
using pomdog::f64;
using pomdog::Radian;

TEST_CASE("MathFunctions", "[MathFunctions]")
{
    SECTION("Clamp")
    {
        REQUIRE(math::clamp(std::numeric_limits<f64>::lowest(), 0.0, 1.0) == 0.0);
        REQUIRE(math::clamp(0.0, 0.0, 1.0) == 0.0);
        REQUIRE(math::clamp(0.5, 0.0, 1.0) == 0.5);
        REQUIRE(math::clamp(1.0, 0.0, 1.0) == 1.0);
        REQUIRE(math::clamp(std::numeric_limits<f64>::max(), 0.0, 1.0) == 1.0);
        REQUIRE(math::clamp(std::numeric_limits<f32>::lowest(), 0.0f, 1.0f) == 0.0f);
        REQUIRE(math::clamp(0.0f, 0.0f, 1.0f) == 0.0f);
        REQUIRE(math::clamp(0.5f, 0.0f, 1.0f) == 0.5f);
        REQUIRE(math::clamp(1.0f, 0.0f, 1.0f) == 1.0f);
        REQUIRE(math::clamp(std::numeric_limits<f32>::max(), 0.0f, 1.0f) == 1.0f);
        REQUIRE(math::clamp(-4.3f, -4.0f, 5.0f) == -4.0f);
        REQUIRE(math::clamp(-2.5f, -4.0f, 5.0f) == -2.5f);
        REQUIRE(math::clamp(0.0f, -4.0f, 5.0f) == 0.0f);
        REQUIRE(math::clamp(3.5f, -4.0f, 5.0f) == 3.5f);
        REQUIRE(math::clamp(5.7f, -4.0f, 5.0f) == 5.0f);
    }
    SECTION("Saturate")
    {
        REQUIRE(math::saturate(std::numeric_limits<f64>::lowest()) == 0.0);
        REQUIRE(math::saturate(-0.1) == 0.0);
        REQUIRE(math::saturate(0.0) == 0.0);
        REQUIRE(math::saturate(0.1) == 0.1);
        REQUIRE(math::saturate(0.5) == 0.5);
        REQUIRE(math::saturate(0.9) == 0.9);
        REQUIRE(math::saturate(1.0) == 1.0);
        REQUIRE(math::saturate(1.1) == 1.0);
        REQUIRE(math::saturate(std::numeric_limits<f64>::max()) == 1.0);

        REQUIRE(math::saturate(std::numeric_limits<f32>::lowest()) == 0.0f);
        REQUIRE(math::saturate(-0.1f) == 0.0f);
        REQUIRE(math::saturate(0.0f) == 0.0f);
        REQUIRE(math::saturate(0.1f) == 0.1f);
        REQUIRE(math::saturate(0.5f) == 0.5f);
        REQUIRE(math::saturate(0.9f) == 0.9f);
        REQUIRE(math::saturate(1.0f) == 1.0f);
        REQUIRE(math::saturate(1.1f) == 1.0f);
        REQUIRE(math::saturate(std::numeric_limits<f32>::max()) == 1.0f);
    }
    SECTION("Lerp")
    {
        constexpr auto epsilon = std::numeric_limits<f32>::epsilon();

        REQUIRE(std::abs(0.0f - math::lerp(0.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.2f - math::lerp(0.0f, 1.0f, 0.2f)) < epsilon);
        REQUIRE(std::abs(0.5f - math::lerp(0.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.8f - math::lerp(0.0f, 1.0f, 0.8f)) < epsilon);
        REQUIRE(std::abs(1.0f - math::lerp(0.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-1.0f - math::lerp(-1.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(-0.6f - math::lerp(-1.0f, 1.0f, 0.2f)) < epsilon);
        REQUIRE(std::abs(0.0f - math::lerp(-1.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.6f - math::lerp(-1.0f, 1.0f, 0.8f)) < epsilon);
        REQUIRE(std::abs(1.0f - math::lerp(-1.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-0.5f - math::lerp(-0.5f, 0.5f, 0.0f)) < epsilon);
        REQUIRE(std::abs(-0.3f - math::lerp(-0.5f, 0.5f, 0.2f)) < epsilon);
        REQUIRE(std::abs(0.0f - math::lerp(-0.5f, 0.5f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.3f - math::lerp(-0.5f, 0.5f, 0.8f)) < epsilon);
        REQUIRE(std::abs(0.5f - math::lerp(-0.5f, 0.5f, 1.0f)) < epsilon);

        REQUIRE(math::lerp(0.0f, 1.0f, 0.0f) < math::lerp(0.0f, 1.0f, 0.1f));
        REQUIRE(math::lerp(0.0f, 1.0f, 0.1f) < math::lerp(0.0f, 1.0f, 0.2f));
        REQUIRE(math::lerp(0.0f, 1.0f, 0.2f) < math::lerp(0.0f, 1.0f, 0.3f));
        REQUIRE(math::lerp(0.0f, 1.0f, 0.3f) < math::lerp(0.0f, 1.0f, 0.4f));
        REQUIRE(math::lerp(0.0f, 1.0f, 0.4f) < math::lerp(0.0f, 1.0f, 0.5f));
        REQUIRE(math::lerp(0.0f, 1.0f, 0.5f) < math::lerp(0.0f, 1.0f, 0.6f));
        REQUIRE(math::lerp(0.0f, 1.0f, 0.6f) < math::lerp(0.0f, 1.0f, 0.7f));
        REQUIRE(math::lerp(0.0f, 1.0f, 0.7f) < math::lerp(0.0f, 1.0f, 0.8f));
        REQUIRE(math::lerp(0.0f, 1.0f, 0.8f) < math::lerp(0.0f, 1.0f, 0.9f));
        REQUIRE(math::lerp(0.0f, 1.0f, 0.9f) < math::lerp(0.0f, 1.0f, 1.0f));
    }
    SECTION("SmoothStep")
    {
        constexpr auto epsilon = std::numeric_limits<f32>::epsilon();

        REQUIRE(std::abs(0.0f - math::smoothstep(0.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.5f - math::smoothstep(0.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(1.0f - math::smoothstep(0.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-1.0f - math::smoothstep(-1.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.0f - math::smoothstep(-1.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(1.0f - math::smoothstep(-1.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-0.5f - math::smoothstep(-0.5f, 0.5f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.0f - math::smoothstep(-0.5f, 0.5f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.5f - math::smoothstep(-0.5f, 0.5f, 1.0f)) < epsilon);

        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.0f) < math::smoothstep(0.0f, 1.0f, 0.1f));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.1f) < math::smoothstep(0.0f, 1.0f, 0.2f));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.2f) < math::smoothstep(0.0f, 1.0f, 0.3f));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.3f) < math::smoothstep(0.0f, 1.0f, 0.4f));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.4f) < math::smoothstep(0.0f, 1.0f, 0.5f));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.5f) < math::smoothstep(0.0f, 1.0f, 0.6f));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.6f) < math::smoothstep(0.0f, 1.0f, 0.7f));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.7f) < math::smoothstep(0.0f, 1.0f, 0.8f));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.8f) < math::smoothstep(0.0f, 1.0f, 0.9f));
        REQUIRE(math::smoothstep(0.0f, 1.0f, 0.9f) < math::smoothstep(0.0f, 1.0f, 1.0f));
    }
    SECTION("ToDegrees")
    {
        constexpr auto epsilon = 0.0000001f;
        REQUIRE(std::abs(0.0f - math::toDegree(0.0f).get()) < epsilon);
        REQUIRE(std::abs(45.0f - math::toDegree(0.7853981633f).get()) < epsilon);
        REQUIRE(std::abs(90.0f - math::toDegree(1.5707963267f).get()) < epsilon);
        REQUIRE(std::abs(180.0f - math::toDegree(3.1415926535f).get()) < epsilon);
        REQUIRE(std::abs(360.0f - math::toDegree(6.2831853071f).get()) < epsilon);

        REQUIRE(std::abs(0.0f - math::toDegree(Radian<f32>(0.0f)).get()) < epsilon);
        REQUIRE(std::abs(45.0f - math::toDegree(Radian<f32>(0.7853981633f)).get()) < epsilon);
        REQUIRE(std::abs(90.0f - math::toDegree(Radian<f32>(1.5707963267f)).get()) < epsilon);
        REQUIRE(std::abs(180.0f - math::toDegree(Radian<f32>(3.1415926535f)).get()) < epsilon);
        REQUIRE(std::abs(360.0f - math::toDegree(Radian<f32>(6.2831853071f)).get()) < epsilon);
    }
    SECTION("ToRadians")
    {
        constexpr auto epsilon = 0.0000005f;
        REQUIRE(std::abs(0.0f - math::toRadian(0.0f).get()) < epsilon);
        REQUIRE(std::abs(0.7853981633f - math::toRadian(45.0f).get()) < epsilon);
        REQUIRE(std::abs(1.5707963267f - math::toRadian(90.0f).get()) < epsilon);
        REQUIRE(std::abs(3.1415926535f - math::toRadian(180.0f).get()) < epsilon);
        REQUIRE(std::abs(6.2831853071f - math::toRadian(360.0f).get()) < epsilon);

        REQUIRE(std::abs(0.0f - math::toRadian(Degree<f32>(0.0f)).get()) < epsilon);
        REQUIRE(std::abs(0.7853981633f - math::toRadian(Degree<f32>(45.0f)).get()) < epsilon);
        REQUIRE(std::abs(1.5707963267f - math::toRadian(Degree<f32>(90.0f)).get()) < epsilon);
        REQUIRE(std::abs(3.1415926535f - math::toRadian(Degree<f32>(180.0f)).get()) < epsilon);
        REQUIRE(std::abs(6.2831853071f - math::toRadian(Degree<f32>(360.0f)).get()) < epsilon);
    }
}
