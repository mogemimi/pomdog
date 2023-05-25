// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/degree.h"
#include "pomdog/math/math.h"
#include "pomdog/math/radian.h"
#include <catch_amalgamated.hpp>

namespace math = pomdog::math;
using pomdog::Degree;
using pomdog::Radian;

TEST_CASE("MathHelper", "[MathHelper]")
{
    SECTION("Constants")
    {
        // NOTE: float
        REQUIRE(math::Pi<float> == 3.1415926535f);
        REQUIRE(math::TwoPi<float> == 6.2831853071f);
        REQUIRE(math::OneOverPi<float> == 0.3183098861f);
        REQUIRE(math::OneOver2Pi<float> == 0.1591549430f);
        REQUIRE(math::PiOver2<float> == 1.5707963267f);
        REQUIRE(math::PiOver4<float> == 0.7853981633f);

        // NOTE: double
        REQUIRE(math::Pi<double> == 3.14159265358979323846);
        REQUIRE(math::TwoPi<double> == 6.28318530717958647692);
        REQUIRE(math::OneOverPi<double> == 0.31830988618379067154);
        REQUIRE(math::OneOver2Pi<double> == 0.15915494309189533576);
        REQUIRE(math::PiOver2<double> == 1.57079632679489661923);
        REQUIRE(math::PiOver4<double> == 0.78539816339744830962);
    }
    SECTION("Clamp")
    {
        REQUIRE(math::clamp(std::numeric_limits<double>::lowest(), 0.0, 1.0) == 0.0);
        REQUIRE(math::clamp(0.0, 0.0, 1.0) == 0.0);
        REQUIRE(math::clamp(0.5, 0.0, 1.0) == 0.5);
        REQUIRE(math::clamp(1.0, 0.0, 1.0) == 1.0);
        REQUIRE(math::clamp(std::numeric_limits<double>::max(), 0.0, 1.0) == 1.0);

        REQUIRE(math::clamp(std::numeric_limits<float>::lowest(), 0.0f, 1.0f) == 0.0f);
        REQUIRE(math::clamp(0.0f, 0.0f, 1.0f) == 0.0);
        REQUIRE(math::clamp(0.5f, 0.0f, 1.0f) == 0.5);
        REQUIRE(math::clamp(1.0f, 0.0f, 1.0f) == 1.0);
        REQUIRE(math::clamp(std::numeric_limits<float>::max(), 0.0f, 1.0f) == 1.0f);

        REQUIRE(math::clamp(-4.3f, -4.0f, 5.0f) == -4.0f);
        REQUIRE(math::clamp(-2.5f, -4.0f, 5.0f) == -2.5f);
        REQUIRE(math::clamp(0.0f, -4.0f, 5.0f) == 0.0f);
        REQUIRE(math::clamp(3.5f, -4.0f, 5.0f) == 3.5f);
        REQUIRE(math::clamp(5.7f, -4.0f, 5.0f) == 5.0f);
    }
    SECTION("Saturate")
    {
        REQUIRE(math::saturate(std::numeric_limits<double>::lowest()) == 0.0);
        REQUIRE(math::saturate(-0.1) == 0.0);
        REQUIRE(math::saturate(0.0) == 0.0);
        REQUIRE(math::saturate(0.1) == 0.1);
        REQUIRE(math::saturate(0.5) == 0.5);
        REQUIRE(math::saturate(0.9) == 0.9);
        REQUIRE(math::saturate(1.0) == 1.0);
        REQUIRE(math::saturate(1.1) == 1.0);
        REQUIRE(math::saturate(std::numeric_limits<double>::max()) == 1.0);

        REQUIRE(math::saturate(std::numeric_limits<float>::lowest()) == 0.0f);
        REQUIRE(math::saturate(-0.1f) == 0.0f);
        REQUIRE(math::saturate(0.0f) == 0.0f);
        REQUIRE(math::saturate(0.1f) == 0.1f);
        REQUIRE(math::saturate(0.5f) == 0.5f);
        REQUIRE(math::saturate(0.9f) == 0.9f);
        REQUIRE(math::saturate(1.0f) == 1.0f);
        REQUIRE(math::saturate(1.1f) == 1.0f);
        REQUIRE(math::saturate(std::numeric_limits<float>::max()) == 1.0f);
    }
    SECTION("Lerp")
    {
        constexpr auto epsilon = std::numeric_limits<float>::epsilon();

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
        constexpr auto epsilon = std::numeric_limits<float>::epsilon();

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
        REQUIRE(std::abs(0.0f - math::toDegrees(0.0f).value) < epsilon);
        REQUIRE(std::abs(45.0f - math::toDegrees(0.7853981633f).value) < epsilon);
        REQUIRE(std::abs(90.0f - math::toDegrees(1.5707963267f).value) < epsilon);
        REQUIRE(std::abs(180.0f - math::toDegrees(3.1415926535f).value) < epsilon);
        REQUIRE(std::abs(360.0f - math::toDegrees(6.2831853071f).value) < epsilon);

        REQUIRE(std::abs(0.0f - math::toDegrees(Radian<float>(0.0f)).value) < epsilon);
        REQUIRE(std::abs(45.0f - math::toDegrees(Radian<float>(0.7853981633f)).value) < epsilon);
        REQUIRE(std::abs(90.0f - math::toDegrees(Radian<float>(1.5707963267f)).value) < epsilon);
        REQUIRE(std::abs(180.0f - math::toDegrees(Radian<float>(3.1415926535f)).value) < epsilon);
        REQUIRE(std::abs(360.0f - math::toDegrees(Radian<float>(6.2831853071f)).value) < epsilon);
    }
    SECTION("ToRadians")
    {
        constexpr auto epsilon = 0.0000005f;
        REQUIRE(std::abs(0.0f - math::toRadians(0.0f).value) < epsilon);
        REQUIRE(std::abs(0.7853981633f - math::toRadians(45.0f).value) < epsilon);
        REQUIRE(std::abs(1.5707963267f - math::toRadians(90.0f).value) < epsilon);
        REQUIRE(std::abs(3.1415926535f - math::toRadians(180.0f).value) < epsilon);
        REQUIRE(std::abs(6.2831853071f - math::toRadians(360.0f).value) < epsilon);

        REQUIRE(std::abs(0.0f - math::toRadians(Degree<float>(0.0f)).value) < epsilon);
        REQUIRE(std::abs(0.7853981633f - math::toRadians(Degree<float>(45.0f)).value) < epsilon);
        REQUIRE(std::abs(1.5707963267f - math::toRadians(Degree<float>(90.0f)).value) < epsilon);
        REQUIRE(std::abs(3.1415926535f - math::toRadians(Degree<float>(180.0f)).value) < epsilon);
        REQUIRE(std::abs(6.2831853071f - math::toRadians(Degree<float>(360.0f)).value) < epsilon);
    }
}
