// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Math.hpp"
#include "catch.hpp"

namespace Math = Pomdog::Math;
using Pomdog::Degree;
using Pomdog::Radian;

TEST_CASE("MathHelper", "[MathHelper]")
{
    SECTION("Constants")
    {
        // NOTE: float
        REQUIRE(Math::Pi<float> == 3.1415926535f);
        REQUIRE(Math::TwoPi<float> == 6.2831853071f);
        REQUIRE(Math::OneOverPi<float> == 0.3183098861f);
        REQUIRE(Math::OneOver2Pi<float> == 0.1591549430f);
        REQUIRE(Math::PiOver2<float> == 1.5707963267f);
        REQUIRE(Math::PiOver4<float> == 0.7853981633f);

        // NOTE: double
        REQUIRE(Math::Pi<double> == 3.14159265358979323846);
        REQUIRE(Math::TwoPi<double> == 6.28318530717958647692);
        REQUIRE(Math::OneOverPi<double> == 0.31830988618379067154);
        REQUIRE(Math::OneOver2Pi<double> == 0.15915494309189533576);
        REQUIRE(Math::PiOver2<double> == 1.57079632679489661923);
        REQUIRE(Math::PiOver4<double> == 0.78539816339744830962);
    }
    SECTION("Clamp")
    {
        REQUIRE(Math::Clamp(std::numeric_limits<double>::lowest(), 0.0, 1.0) == 0.0);
        REQUIRE(Math::Clamp(0.0, 0.0, 1.0) == 0.0);
        REQUIRE(Math::Clamp(0.5, 0.0, 1.0) == 0.5);
        REQUIRE(Math::Clamp(1.0, 0.0, 1.0) == 1.0);
        REQUIRE(Math::Clamp(std::numeric_limits<double>::max(), 0.0, 1.0) == 1.0);

        REQUIRE(Math::Clamp(std::numeric_limits<float>::lowest(), 0.0f, 1.0f) == 0.0f);
        REQUIRE(Math::Clamp(0.0f, 0.0f, 1.0f) == 0.0);
        REQUIRE(Math::Clamp(0.5f, 0.0f, 1.0f) == 0.5);
        REQUIRE(Math::Clamp(1.0f, 0.0f, 1.0f) == 1.0);
        REQUIRE(Math::Clamp(std::numeric_limits<float>::max(), 0.0f, 1.0f) == 1.0f);

        REQUIRE(Math::Clamp(-4.3f, -4.0f, 5.0f) == -4.0f);
        REQUIRE(Math::Clamp(-2.5f, -4.0f, 5.0f) == -2.5f);
        REQUIRE(Math::Clamp(0.0f, -4.0f, 5.0f) == 0.0f);
        REQUIRE(Math::Clamp(3.5f, -4.0f, 5.0f) == 3.5f);
        REQUIRE(Math::Clamp(5.7f, -4.0f, 5.0f) == 5.0f);
    }
    SECTION("Saturate")
    {
        REQUIRE(Math::Saturate(std::numeric_limits<double>::lowest()) == 0.0);
        REQUIRE(Math::Saturate(-0.1) == 0.0);
        REQUIRE(Math::Saturate(0.0) == 0.0);
        REQUIRE(Math::Saturate(0.1) == 0.1);
        REQUIRE(Math::Saturate(0.5) == 0.5);
        REQUIRE(Math::Saturate(0.9) == 0.9);
        REQUIRE(Math::Saturate(1.0) == 1.0);
        REQUIRE(Math::Saturate(1.1) == 1.0);
        REQUIRE(Math::Saturate(std::numeric_limits<double>::max()) == 1.0);

        REQUIRE(Math::Saturate(std::numeric_limits<float>::lowest()) == 0.0f);
        REQUIRE(Math::Saturate(-0.1f) == 0.0f);
        REQUIRE(Math::Saturate(0.0f) == 0.0f);
        REQUIRE(Math::Saturate(0.1f) == 0.1f);
        REQUIRE(Math::Saturate(0.5f) == 0.5f);
        REQUIRE(Math::Saturate(0.9f) == 0.9f);
        REQUIRE(Math::Saturate(1.0f) == 1.0f);
        REQUIRE(Math::Saturate(1.1f) == 1.0f);
        REQUIRE(Math::Saturate(std::numeric_limits<float>::max()) == 1.0f);
    }
    SECTION("Lerp")
    {
        constexpr auto epsilon = std::numeric_limits<float>::epsilon();

        REQUIRE(std::abs(0.0f - Math::Lerp(0.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.2f - Math::Lerp(0.0f, 1.0f, 0.2f)) < epsilon);
        REQUIRE(std::abs(0.5f - Math::Lerp(0.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.8f - Math::Lerp(0.0f, 1.0f, 0.8f)) < epsilon);
        REQUIRE(std::abs(1.0f - Math::Lerp(0.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-1.0f - Math::Lerp(-1.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(-0.6f - Math::Lerp(-1.0f, 1.0f, 0.2f)) < epsilon);
        REQUIRE(std::abs(0.0f - Math::Lerp(-1.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.6f - Math::Lerp(-1.0f, 1.0f, 0.8f)) < epsilon);
        REQUIRE(std::abs(1.0f - Math::Lerp(-1.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-0.5f - Math::Lerp(-0.5f, 0.5f, 0.0f)) < epsilon);
        REQUIRE(std::abs(-0.3f - Math::Lerp(-0.5f, 0.5f, 0.2f)) < epsilon);
        REQUIRE(std::abs(0.0f - Math::Lerp(-0.5f, 0.5f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.3f - Math::Lerp(-0.5f, 0.5f, 0.8f)) < epsilon);
        REQUIRE(std::abs(0.5f - Math::Lerp(-0.5f, 0.5f, 1.0f)) < epsilon);

        REQUIRE(Math::Lerp(0.0f, 1.0f, 0.0f) < Math::Lerp(0.0f, 1.0f, 0.1f));
        REQUIRE(Math::Lerp(0.0f, 1.0f, 0.1f) < Math::Lerp(0.0f, 1.0f, 0.2f));
        REQUIRE(Math::Lerp(0.0f, 1.0f, 0.2f) < Math::Lerp(0.0f, 1.0f, 0.3f));
        REQUIRE(Math::Lerp(0.0f, 1.0f, 0.3f) < Math::Lerp(0.0f, 1.0f, 0.4f));
        REQUIRE(Math::Lerp(0.0f, 1.0f, 0.4f) < Math::Lerp(0.0f, 1.0f, 0.5f));
        REQUIRE(Math::Lerp(0.0f, 1.0f, 0.5f) < Math::Lerp(0.0f, 1.0f, 0.6f));
        REQUIRE(Math::Lerp(0.0f, 1.0f, 0.6f) < Math::Lerp(0.0f, 1.0f, 0.7f));
        REQUIRE(Math::Lerp(0.0f, 1.0f, 0.7f) < Math::Lerp(0.0f, 1.0f, 0.8f));
        REQUIRE(Math::Lerp(0.0f, 1.0f, 0.8f) < Math::Lerp(0.0f, 1.0f, 0.9f));
        REQUIRE(Math::Lerp(0.0f, 1.0f, 0.9f) < Math::Lerp(0.0f, 1.0f, 1.0f));
    }
    SECTION("SmoothStep")
    {
        constexpr auto epsilon = std::numeric_limits<float>::epsilon();

        REQUIRE(std::abs(0.0f - Math::SmoothStep(0.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.5f - Math::SmoothStep(0.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(1.0f - Math::SmoothStep(0.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-1.0f - Math::SmoothStep(-1.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.0f - Math::SmoothStep(-1.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(1.0f - Math::SmoothStep(-1.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-0.5f - Math::SmoothStep(-0.5f, 0.5f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.0f - Math::SmoothStep(-0.5f, 0.5f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.5f - Math::SmoothStep(-0.5f, 0.5f, 1.0f)) < epsilon);

        REQUIRE(Math::SmoothStep(0.0f, 1.0f, 0.0f) < Math::SmoothStep(0.0f, 1.0f, 0.1f));
        REQUIRE(Math::SmoothStep(0.0f, 1.0f, 0.1f) < Math::SmoothStep(0.0f, 1.0f, 0.2f));
        REQUIRE(Math::SmoothStep(0.0f, 1.0f, 0.2f) < Math::SmoothStep(0.0f, 1.0f, 0.3f));
        REQUIRE(Math::SmoothStep(0.0f, 1.0f, 0.3f) < Math::SmoothStep(0.0f, 1.0f, 0.4f));
        REQUIRE(Math::SmoothStep(0.0f, 1.0f, 0.4f) < Math::SmoothStep(0.0f, 1.0f, 0.5f));
        REQUIRE(Math::SmoothStep(0.0f, 1.0f, 0.5f) < Math::SmoothStep(0.0f, 1.0f, 0.6f));
        REQUIRE(Math::SmoothStep(0.0f, 1.0f, 0.6f) < Math::SmoothStep(0.0f, 1.0f, 0.7f));
        REQUIRE(Math::SmoothStep(0.0f, 1.0f, 0.7f) < Math::SmoothStep(0.0f, 1.0f, 0.8f));
        REQUIRE(Math::SmoothStep(0.0f, 1.0f, 0.8f) < Math::SmoothStep(0.0f, 1.0f, 0.9f));
        REQUIRE(Math::SmoothStep(0.0f, 1.0f, 0.9f) < Math::SmoothStep(0.0f, 1.0f, 1.0f));
    }
    SECTION("ToDegrees")
    {
        constexpr auto epsilon = 0.0000001f;
        REQUIRE(std::abs(0.0f - Math::ToDegrees(0.0f).value) < epsilon);
        REQUIRE(std::abs(45.0f - Math::ToDegrees(0.7853981633f).value) < epsilon);
        REQUIRE(std::abs(90.0f - Math::ToDegrees(1.5707963267f).value) < epsilon);
        REQUIRE(std::abs(180.0f - Math::ToDegrees(3.1415926535f).value) < epsilon);
        REQUIRE(std::abs(360.0f - Math::ToDegrees(6.2831853071f).value) < epsilon);

        REQUIRE(std::abs(0.0f - Math::ToDegrees(Radian<float>(0.0f)).value) < epsilon);
        REQUIRE(std::abs(45.0f - Math::ToDegrees(Radian<float>(0.7853981633f)).value) < epsilon);
        REQUIRE(std::abs(90.0f - Math::ToDegrees(Radian<float>(1.5707963267f)).value) < epsilon);
        REQUIRE(std::abs(180.0f - Math::ToDegrees(Radian<float>(3.1415926535f)).value) < epsilon);
        REQUIRE(std::abs(360.0f - Math::ToDegrees(Radian<float>(6.2831853071f)).value) < epsilon);
    }
    SECTION("ToRadians")
    {
        constexpr auto epsilon = 0.0000005f;
        REQUIRE(std::abs(0.0f - Math::ToRadians(0.0f).value) < epsilon);
        REQUIRE(std::abs(0.7853981633f - Math::ToRadians(45.0f).value) < epsilon);
        REQUIRE(std::abs(1.5707963267f - Math::ToRadians(90.0f).value) < epsilon);
        REQUIRE(std::abs(3.1415926535f - Math::ToRadians(180.0f).value) < epsilon);
        REQUIRE(std::abs(6.2831853071f - Math::ToRadians(360.0f).value) < epsilon);

        REQUIRE(std::abs(0.0f - Math::ToRadians(Degree<float>(0.0f)).value) < epsilon);
        REQUIRE(std::abs(0.7853981633f - Math::ToRadians(Degree<float>(45.0f)).value) < epsilon);
        REQUIRE(std::abs(1.5707963267f - Math::ToRadians(Degree<float>(90.0f)).value) < epsilon);
        REQUIRE(std::abs(3.1415926535f - Math::ToRadians(Degree<float>(180.0f)).value) < epsilon);
        REQUIRE(std::abs(6.2831853071f - Math::ToRadians(Degree<float>(360.0f)).value) < epsilon);
    }
}
