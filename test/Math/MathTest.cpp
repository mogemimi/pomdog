// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Math.hpp"
#include "catch.hpp"
#include <cfloat>

namespace MathHelper = Pomdog::MathHelper;
using Pomdog::Degree;
using Pomdog::Radian;

TEST_CASE("MathHelper", "[MathHelper]")
{
    SECTION("Constants")
    {
        namespace Math = Pomdog::Math;

        REQUIRE(Math::Pi<float> == 3.1415926535f);
        REQUIRE(Math::TwoPi<float> == 6.2831853071f);
        REQUIRE(Math::OneOverPi<float> == 0.3183098861f);
        REQUIRE(Math::OneOver2Pi<float> == 0.1591549430f);
        REQUIRE(Math::PiOver2<float> == 1.5707963267f);
        REQUIRE(Math::PiOver4<float> == 0.7853981633f);

    #if defined(DBL_MANT_DIG)
        REQUIRE(Math::Pi<double> == 3.14159265358979323846);
        REQUIRE(Math::TwoPi<double> == 6.28318530717958647692);
        REQUIRE(Math::OneOverPi<double> == 0.31830988618379067154);
        REQUIRE(Math::OneOver2Pi<double> == 0.15915494309189533576);
        REQUIRE(Math::PiOver2<double> == 1.57079632679489661923);
        REQUIRE(Math::PiOver4<double> == 0.78539816339744830962);
    #endif

    #if defined(LDBL_MANT_DIG)
        REQUIRE(Math::Pi<long double> == 3.14159265358979323846L);
        REQUIRE(Math::TwoPi<long double> == 6.28318530717958647692L);
        REQUIRE(Math::OneOverPi<long double> == 0.31830988618379067154L);
        REQUIRE(Math::OneOver2Pi<long double> == 0.15915494309189533576L);
        REQUIRE(Math::PiOver2<long double> == 1.57079632679489661923L);
        REQUIRE(Math::PiOver4<long double> == 0.78539816339744830962L);
    #endif
    }
    SECTION("Clamp")
    {
        REQUIRE(MathHelper::Clamp(std::numeric_limits<double>::lowest(), 0.0, 1.0) == 0.0);
        REQUIRE(MathHelper::Clamp(0.0, 0.0, 1.0) == 0.0);
        REQUIRE(MathHelper::Clamp(0.5, 0.0, 1.0) == 0.5);
        REQUIRE(MathHelper::Clamp(1.0, 0.0, 1.0) == 1.0);
        REQUIRE(MathHelper::Clamp(std::numeric_limits<double>::max(), 0.0, 1.0) == 1.0);

        REQUIRE(MathHelper::Clamp(std::numeric_limits<float>::lowest(), 0.0f, 1.0f) == 0.0f);
        REQUIRE(MathHelper::Clamp(0.0f, 0.0f, 1.0f) == 0.0);
        REQUIRE(MathHelper::Clamp(0.5f, 0.0f, 1.0f) == 0.5);
        REQUIRE(MathHelper::Clamp(1.0f, 0.0f, 1.0f) == 1.0);
        REQUIRE(MathHelper::Clamp(std::numeric_limits<float>::max(), 0.0f, 1.0f) == 1.0f);

        REQUIRE(MathHelper::Clamp(-4.3f, -4.0f, 5.0f) == -4.0f);
        REQUIRE(MathHelper::Clamp(-2.5f, -4.0f, 5.0f) == -2.5f);
        REQUIRE(MathHelper::Clamp(0.0f, -4.0f, 5.0f) == 0.0f);
        REQUIRE(MathHelper::Clamp(3.5f, -4.0f, 5.0f) == 3.5f);
        REQUIRE(MathHelper::Clamp(5.7f, -4.0f, 5.0f) == 5.0f);
    }
    SECTION("Saturate")
    {
        REQUIRE(MathHelper::Saturate(std::numeric_limits<double>::lowest()) == 0.0);
        REQUIRE(MathHelper::Saturate(-0.1) == 0.0);
        REQUIRE(MathHelper::Saturate(0.0) == 0.0);
        REQUIRE(MathHelper::Saturate(0.1) == 0.1);
        REQUIRE(MathHelper::Saturate(0.5) == 0.5);
        REQUIRE(MathHelper::Saturate(0.9) == 0.9);
        REQUIRE(MathHelper::Saturate(1.0) == 1.0);
        REQUIRE(MathHelper::Saturate(1.1) == 1.0);
        REQUIRE(MathHelper::Saturate(std::numeric_limits<double>::max()) == 1.0);

        REQUIRE(MathHelper::Saturate(std::numeric_limits<float>::lowest()) == 0.0f);
        REQUIRE(MathHelper::Saturate(-0.1f) == 0.0f);
        REQUIRE(MathHelper::Saturate(0.0f) == 0.0f);
        REQUIRE(MathHelper::Saturate(0.1f) == 0.1f);
        REQUIRE(MathHelper::Saturate(0.5f) == 0.5f);
        REQUIRE(MathHelper::Saturate(0.9f) == 0.9f);
        REQUIRE(MathHelper::Saturate(1.0f) == 1.0f);
        REQUIRE(MathHelper::Saturate(1.1f) == 1.0f);
        REQUIRE(MathHelper::Saturate(std::numeric_limits<float>::max()) == 1.0f);
    }
    SECTION("Lerp")
    {
        constexpr auto epsilon = std::numeric_limits<float>::epsilon();

        REQUIRE(std::abs(0.0f - MathHelper::Lerp(0.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.2f - MathHelper::Lerp(0.0f, 1.0f, 0.2f)) < epsilon);
        REQUIRE(std::abs(0.5f - MathHelper::Lerp(0.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.8f - MathHelper::Lerp(0.0f, 1.0f, 0.8f)) < epsilon);
        REQUIRE(std::abs(1.0f - MathHelper::Lerp(0.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-1.0f - MathHelper::Lerp(-1.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(-0.6f - MathHelper::Lerp(-1.0f, 1.0f, 0.2f)) < epsilon);
        REQUIRE(std::abs(0.0f - MathHelper::Lerp(-1.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.6f - MathHelper::Lerp(-1.0f, 1.0f, 0.8f)) < epsilon);
        REQUIRE(std::abs(1.0f - MathHelper::Lerp(-1.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-0.5f - MathHelper::Lerp(-0.5f, 0.5f, 0.0f)) < epsilon);
        REQUIRE(std::abs(-0.3f - MathHelper::Lerp(-0.5f, 0.5f, 0.2f)) < epsilon);
        REQUIRE(std::abs(0.0f - MathHelper::Lerp(-0.5f, 0.5f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.3f - MathHelper::Lerp(-0.5f, 0.5f, 0.8f)) < epsilon);
        REQUIRE(std::abs(0.5f - MathHelper::Lerp(-0.5f, 0.5f, 1.0f)) < epsilon);

        REQUIRE(MathHelper::Lerp(0.0f, 1.0f, 0.0f) < MathHelper::Lerp(0.0f, 1.0f, 0.1f));
        REQUIRE(MathHelper::Lerp(0.0f, 1.0f, 0.1f) < MathHelper::Lerp(0.0f, 1.0f, 0.2f));
        REQUIRE(MathHelper::Lerp(0.0f, 1.0f, 0.2f) < MathHelper::Lerp(0.0f, 1.0f, 0.3f));
        REQUIRE(MathHelper::Lerp(0.0f, 1.0f, 0.3f) < MathHelper::Lerp(0.0f, 1.0f, 0.4f));
        REQUIRE(MathHelper::Lerp(0.0f, 1.0f, 0.4f) < MathHelper::Lerp(0.0f, 1.0f, 0.5f));
        REQUIRE(MathHelper::Lerp(0.0f, 1.0f, 0.5f) < MathHelper::Lerp(0.0f, 1.0f, 0.6f));
        REQUIRE(MathHelper::Lerp(0.0f, 1.0f, 0.6f) < MathHelper::Lerp(0.0f, 1.0f, 0.7f));
        REQUIRE(MathHelper::Lerp(0.0f, 1.0f, 0.7f) < MathHelper::Lerp(0.0f, 1.0f, 0.8f));
        REQUIRE(MathHelper::Lerp(0.0f, 1.0f, 0.8f) < MathHelper::Lerp(0.0f, 1.0f, 0.9f));
        REQUIRE(MathHelper::Lerp(0.0f, 1.0f, 0.9f) < MathHelper::Lerp(0.0f, 1.0f, 1.0f));
    }
    SECTION("SmoothStep")
    {
        constexpr auto epsilon = std::numeric_limits<float>::epsilon();

        REQUIRE(std::abs(0.0f - MathHelper::SmoothStep(0.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.5f - MathHelper::SmoothStep(0.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(1.0f - MathHelper::SmoothStep(0.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-1.0f - MathHelper::SmoothStep(-1.0f, 1.0f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.0f - MathHelper::SmoothStep(-1.0f, 1.0f, 0.5f)) < epsilon);
        REQUIRE(std::abs(1.0f - MathHelper::SmoothStep(-1.0f, 1.0f, 1.0f)) < epsilon);

        REQUIRE(std::abs(-0.5f - MathHelper::SmoothStep(-0.5f, 0.5f, 0.0f)) < epsilon);
        REQUIRE(std::abs(0.0f - MathHelper::SmoothStep(-0.5f, 0.5f, 0.5f)) < epsilon);
        REQUIRE(std::abs(0.5f - MathHelper::SmoothStep(-0.5f, 0.5f, 1.0f)) < epsilon);

        REQUIRE(MathHelper::SmoothStep(0.0f, 1.0f, 0.0f) < MathHelper::SmoothStep(0.0f, 1.0f, 0.1f));
        REQUIRE(MathHelper::SmoothStep(0.0f, 1.0f, 0.1f) < MathHelper::SmoothStep(0.0f, 1.0f, 0.2f));
        REQUIRE(MathHelper::SmoothStep(0.0f, 1.0f, 0.2f) < MathHelper::SmoothStep(0.0f, 1.0f, 0.3f));
        REQUIRE(MathHelper::SmoothStep(0.0f, 1.0f, 0.3f) < MathHelper::SmoothStep(0.0f, 1.0f, 0.4f));
        REQUIRE(MathHelper::SmoothStep(0.0f, 1.0f, 0.4f) < MathHelper::SmoothStep(0.0f, 1.0f, 0.5f));
        REQUIRE(MathHelper::SmoothStep(0.0f, 1.0f, 0.5f) < MathHelper::SmoothStep(0.0f, 1.0f, 0.6f));
        REQUIRE(MathHelper::SmoothStep(0.0f, 1.0f, 0.6f) < MathHelper::SmoothStep(0.0f, 1.0f, 0.7f));
        REQUIRE(MathHelper::SmoothStep(0.0f, 1.0f, 0.7f) < MathHelper::SmoothStep(0.0f, 1.0f, 0.8f));
        REQUIRE(MathHelper::SmoothStep(0.0f, 1.0f, 0.8f) < MathHelper::SmoothStep(0.0f, 1.0f, 0.9f));
        REQUIRE(MathHelper::SmoothStep(0.0f, 1.0f, 0.9f) < MathHelper::SmoothStep(0.0f, 1.0f, 1.0f));
    }
    SECTION("ToDegrees")
    {
        constexpr auto epsilon = 0.0000001f;
        REQUIRE(std::abs(0.0f - MathHelper::ToDegrees(0.0f).value) < epsilon);
        REQUIRE(std::abs(45.0f - MathHelper::ToDegrees(0.7853981633f).value) < epsilon);
        REQUIRE(std::abs(90.0f - MathHelper::ToDegrees(1.5707963267f).value) < epsilon);
        REQUIRE(std::abs(180.0f - MathHelper::ToDegrees(3.1415926535f).value) < epsilon);
        REQUIRE(std::abs(360.0f - MathHelper::ToDegrees(6.2831853071f).value) < epsilon);

        REQUIRE(std::abs(0.0f - MathHelper::ToDegrees(Radian<float>(0.0f)).value) < epsilon);
        REQUIRE(std::abs(45.0f - MathHelper::ToDegrees(Radian<float>(0.7853981633f)).value) < epsilon);
        REQUIRE(std::abs(90.0f - MathHelper::ToDegrees(Radian<float>(1.5707963267f)).value) < epsilon);
        REQUIRE(std::abs(180.0f - MathHelper::ToDegrees(Radian<float>(3.1415926535f)).value) < epsilon);
        REQUIRE(std::abs(360.0f - MathHelper::ToDegrees(Radian<float>(6.2831853071f)).value) < epsilon);
    }
    SECTION("ToRadians")
    {
        constexpr auto epsilon = 0.0000005f;
        REQUIRE(std::abs(0.0f - MathHelper::ToRadians(0.0f).value) < epsilon);
        REQUIRE(std::abs(0.7853981633f - MathHelper::ToRadians(45.0f).value) < epsilon);
        REQUIRE(std::abs(1.5707963267f - MathHelper::ToRadians(90.0f).value) < epsilon);
        REQUIRE(std::abs(3.1415926535f - MathHelper::ToRadians(180.0f).value) < epsilon);
        REQUIRE(std::abs(6.2831853071f - MathHelper::ToRadians(360.0f).value) < epsilon);

        REQUIRE(std::abs(0.0f - MathHelper::ToRadians(Degree<float>(0.0f)).value) < epsilon);
        REQUIRE(std::abs(0.7853981633f - MathHelper::ToRadians(Degree<float>(45.0f)).value) < epsilon);
        REQUIRE(std::abs(1.5707963267f - MathHelper::ToRadians(Degree<float>(90.0f)).value) < epsilon);
        REQUIRE(std::abs(3.1415926535f - MathHelper::ToRadians(Degree<float>(180.0f)).value) < epsilon);
        REQUIRE(std::abs(6.2831853071f - MathHelper::ToRadians(Degree<float>(360.0f)).value) < epsilon);
    }
}
