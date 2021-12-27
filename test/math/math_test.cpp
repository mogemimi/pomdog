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
        REQUIRE(math::Clamp(std::numeric_limits<double>::lowest(), 0.0, 1.0) == 0.0);
        REQUIRE(math::Clamp(0.0, 0.0, 1.0) == 0.0);
        REQUIRE(math::Clamp(0.5, 0.0, 1.0) == 0.5);
        REQUIRE(math::Clamp(1.0, 0.0, 1.0) == 1.0);
        REQUIRE(math::Clamp(std::numeric_limits<double>::max(), 0.0, 1.0) == 1.0);

        REQUIRE(math::Clamp(std::numeric_limits<float>::lowest(), 0.0f, 1.0f) == 0.0f);
        REQUIRE(math::Clamp(0.0f, 0.0f, 1.0f) == 0.0);
        REQUIRE(math::Clamp(0.5f, 0.0f, 1.0f) == 0.5);
        REQUIRE(math::Clamp(1.0f, 0.0f, 1.0f) == 1.0);
        REQUIRE(math::Clamp(std::numeric_limits<float>::max(), 0.0f, 1.0f) == 1.0f);

        REQUIRE(math::Clamp(-4.3f, -4.0f, 5.0f) == -4.0f);
        REQUIRE(math::Clamp(-2.5f, -4.0f, 5.0f) == -2.5f);
        REQUIRE(math::Clamp(0.0f, -4.0f, 5.0f) == 0.0f);
        REQUIRE(math::Clamp(3.5f, -4.0f, 5.0f) == 3.5f);
        REQUIRE(math::Clamp(5.7f, -4.0f, 5.0f) == 5.0f);
    }
    SECTION("Saturate")
    {
        REQUIRE(math::Saturate(std::numeric_limits<double>::lowest()) == 0.0);
        REQUIRE(math::Saturate(-0.1) == 0.0);
        REQUIRE(math::Saturate(0.0) == 0.0);
        REQUIRE(math::Saturate(0.1) == 0.1);
        REQUIRE(math::Saturate(0.5) == 0.5);
        REQUIRE(math::Saturate(0.9) == 0.9);
        REQUIRE(math::Saturate(1.0) == 1.0);
        REQUIRE(math::Saturate(1.1) == 1.0);
        REQUIRE(math::Saturate(std::numeric_limits<double>::max()) == 1.0);

        REQUIRE(math::Saturate(std::numeric_limits<float>::lowest()) == 0.0f);
        REQUIRE(math::Saturate(-0.1f) == 0.0f);
        REQUIRE(math::Saturate(0.0f) == 0.0f);
        REQUIRE(math::Saturate(0.1f) == 0.1f);
        REQUIRE(math::Saturate(0.5f) == 0.5f);
        REQUIRE(math::Saturate(0.9f) == 0.9f);
        REQUIRE(math::Saturate(1.0f) == 1.0f);
        REQUIRE(math::Saturate(1.1f) == 1.0f);
        REQUIRE(math::Saturate(std::numeric_limits<float>::max()) == 1.0f);
    }
    SECTION("Lerp")
    {
        constexpr auto epsilon = std::numeric_limits<float>::epsilon();

        REQUIRE(std::abs(0.0f - math::Lerp(0.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.2f - math::Lerp(0.0f, 1.0f, 0.2f)) < epsilon);
        REQUIRE(std::abs(0.5f - math::Lerp(0.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.8f - math::Lerp(0.0f, 1.0f, 0.8f)) < epsilon);
        REQUIRE(std::abs(1.0f - math::Lerp(0.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-1.0f - math::Lerp(-1.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(-0.6f - math::Lerp(-1.0f, 1.0f, 0.2f)) < epsilon);
        REQUIRE(std::abs(0.0f - math::Lerp(-1.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.6f - math::Lerp(-1.0f, 1.0f, 0.8f)) < epsilon);
        REQUIRE(std::abs(1.0f - math::Lerp(-1.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-0.5f - math::Lerp(-0.5f, 0.5f, 0.0f)) < epsilon);
        REQUIRE(std::abs(-0.3f - math::Lerp(-0.5f, 0.5f, 0.2f)) < epsilon);
        REQUIRE(std::abs(0.0f - math::Lerp(-0.5f, 0.5f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.3f - math::Lerp(-0.5f, 0.5f, 0.8f)) < epsilon);
        REQUIRE(std::abs(0.5f - math::Lerp(-0.5f, 0.5f, 1.0f)) < epsilon);

        REQUIRE(math::Lerp(0.0f, 1.0f, 0.0f) < math::Lerp(0.0f, 1.0f, 0.1f));
        REQUIRE(math::Lerp(0.0f, 1.0f, 0.1f) < math::Lerp(0.0f, 1.0f, 0.2f));
        REQUIRE(math::Lerp(0.0f, 1.0f, 0.2f) < math::Lerp(0.0f, 1.0f, 0.3f));
        REQUIRE(math::Lerp(0.0f, 1.0f, 0.3f) < math::Lerp(0.0f, 1.0f, 0.4f));
        REQUIRE(math::Lerp(0.0f, 1.0f, 0.4f) < math::Lerp(0.0f, 1.0f, 0.5f));
        REQUIRE(math::Lerp(0.0f, 1.0f, 0.5f) < math::Lerp(0.0f, 1.0f, 0.6f));
        REQUIRE(math::Lerp(0.0f, 1.0f, 0.6f) < math::Lerp(0.0f, 1.0f, 0.7f));
        REQUIRE(math::Lerp(0.0f, 1.0f, 0.7f) < math::Lerp(0.0f, 1.0f, 0.8f));
        REQUIRE(math::Lerp(0.0f, 1.0f, 0.8f) < math::Lerp(0.0f, 1.0f, 0.9f));
        REQUIRE(math::Lerp(0.0f, 1.0f, 0.9f) < math::Lerp(0.0f, 1.0f, 1.0f));
    }
    SECTION("SmoothStep")
    {
        constexpr auto epsilon = std::numeric_limits<float>::epsilon();

        REQUIRE(std::abs(0.0f - math::SmoothStep(0.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.5f - math::SmoothStep(0.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(1.0f - math::SmoothStep(0.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-1.0f - math::SmoothStep(-1.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.0f - math::SmoothStep(-1.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(1.0f - math::SmoothStep(-1.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-0.5f - math::SmoothStep(-0.5f, 0.5f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.0f - math::SmoothStep(-0.5f, 0.5f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.5f - math::SmoothStep(-0.5f, 0.5f, 1.0f)) < epsilon);

        REQUIRE(math::SmoothStep(0.0f, 1.0f, 0.0f) < math::SmoothStep(0.0f, 1.0f, 0.1f));
        REQUIRE(math::SmoothStep(0.0f, 1.0f, 0.1f) < math::SmoothStep(0.0f, 1.0f, 0.2f));
        REQUIRE(math::SmoothStep(0.0f, 1.0f, 0.2f) < math::SmoothStep(0.0f, 1.0f, 0.3f));
        REQUIRE(math::SmoothStep(0.0f, 1.0f, 0.3f) < math::SmoothStep(0.0f, 1.0f, 0.4f));
        REQUIRE(math::SmoothStep(0.0f, 1.0f, 0.4f) < math::SmoothStep(0.0f, 1.0f, 0.5f));
        REQUIRE(math::SmoothStep(0.0f, 1.0f, 0.5f) < math::SmoothStep(0.0f, 1.0f, 0.6f));
        REQUIRE(math::SmoothStep(0.0f, 1.0f, 0.6f) < math::SmoothStep(0.0f, 1.0f, 0.7f));
        REQUIRE(math::SmoothStep(0.0f, 1.0f, 0.7f) < math::SmoothStep(0.0f, 1.0f, 0.8f));
        REQUIRE(math::SmoothStep(0.0f, 1.0f, 0.8f) < math::SmoothStep(0.0f, 1.0f, 0.9f));
        REQUIRE(math::SmoothStep(0.0f, 1.0f, 0.9f) < math::SmoothStep(0.0f, 1.0f, 1.0f));
    }
    SECTION("ToDegrees")
    {
        constexpr auto epsilon = 0.0000001f;
        REQUIRE(std::abs(0.0f - math::ToDegrees(0.0f).value) < epsilon);
        REQUIRE(std::abs(45.0f - math::ToDegrees(0.7853981633f).value) < epsilon);
        REQUIRE(std::abs(90.0f - math::ToDegrees(1.5707963267f).value) < epsilon);
        REQUIRE(std::abs(180.0f - math::ToDegrees(3.1415926535f).value) < epsilon);
        REQUIRE(std::abs(360.0f - math::ToDegrees(6.2831853071f).value) < epsilon);

        REQUIRE(std::abs(0.0f - math::ToDegrees(Radian<float>(0.0f)).value) < epsilon);
        REQUIRE(std::abs(45.0f - math::ToDegrees(Radian<float>(0.7853981633f)).value) < epsilon);
        REQUIRE(std::abs(90.0f - math::ToDegrees(Radian<float>(1.5707963267f)).value) < epsilon);
        REQUIRE(std::abs(180.0f - math::ToDegrees(Radian<float>(3.1415926535f)).value) < epsilon);
        REQUIRE(std::abs(360.0f - math::ToDegrees(Radian<float>(6.2831853071f)).value) < epsilon);
    }
    SECTION("ToRadians")
    {
        constexpr auto epsilon = 0.0000005f;
        REQUIRE(std::abs(0.0f - math::ToRadians(0.0f).value) < epsilon);
        REQUIRE(std::abs(0.7853981633f - math::ToRadians(45.0f).value) < epsilon);
        REQUIRE(std::abs(1.5707963267f - math::ToRadians(90.0f).value) < epsilon);
        REQUIRE(std::abs(3.1415926535f - math::ToRadians(180.0f).value) < epsilon);
        REQUIRE(std::abs(6.2831853071f - math::ToRadians(360.0f).value) < epsilon);

        REQUIRE(std::abs(0.0f - math::ToRadians(Degree<float>(0.0f)).value) < epsilon);
        REQUIRE(std::abs(0.7853981633f - math::ToRadians(Degree<float>(45.0f)).value) < epsilon);
        REQUIRE(std::abs(1.5707963267f - math::ToRadians(Degree<float>(90.0f)).value) < epsilon);
        REQUIRE(std::abs(3.1415926535f - math::ToRadians(Degree<float>(180.0f)).value) < epsilon);
        REQUIRE(std::abs(6.2831853071f - math::ToRadians(Degree<float>(360.0f)).value) < epsilon);
    }
}
