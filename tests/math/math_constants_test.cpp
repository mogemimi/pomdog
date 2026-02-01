// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/math_constants.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace math = pomdog::math;
using pomdog::f32;
using pomdog::f64;

TEST_CASE("math_constants")
{
    SUBCASE("f32")
    {
        REQUIRE(math::Pi<f32> == 3.1415926535f);
        REQUIRE(math::TwoPi<f32> == 6.2831853071f);
        REQUIRE(math::OneOverPi<f32> == 0.3183098861f);
        REQUIRE(math::OneOver2Pi<f32> == 0.1591549430f);
        REQUIRE(math::PiOver2<f32> == 1.5707963267f);
        REQUIRE(math::PiOver4<f32> == 0.7853981633f);
    }
    SUBCASE("f64")
    {
        REQUIRE(math::Pi<f64> == 3.14159265358979323846);
        REQUIRE(math::TwoPi<f64> == 6.28318530717958647692);
        REQUIRE(math::OneOverPi<f64> == 0.31830988618379067154);
        REQUIRE(math::OneOver2Pi<f64> == 0.15915494309189533576);
        REQUIRE(math::PiOver2<f64> == 1.57079632679489661923);
        REQUIRE(math::PiOver4<f64> == 0.78539816339744830962);
    }
    SUBCASE("Pi")
    {
        REQUIRE(std::abs(3.1415926535f - math::Pi<float>) < 0.0000001f);
        REQUIRE(std::abs(3.14159265358979323846 - math::Pi<double>) < 0.0000000000001);
        REQUIRE(std::abs(3.14159265358979323846l - math::Pi<long double>) < 0.00000000000001l);
    }
    SUBCASE("TwoPi")
    {
        REQUIRE(std::abs(6.2831853071f - math::TwoPi<float>) < 0.0000001f);
        REQUIRE(std::abs(6.28318530717958647692 - math::TwoPi<double>) < 0.0000000000001);
        REQUIRE(std::abs(6.28318530717958647692l - math::TwoPi<long double>) < 0.00000000000001l);
    }
    SUBCASE("OneOverPi")
    {
        REQUIRE(std::abs(0.31830988618f - math::OneOverPi<float>) < 0.0000001f);
        REQUIRE(std::abs(0.318309886183790671538 - math::OneOverPi<double>) < 0.0000000000001);
        REQUIRE(std::abs(0.318309886183790671538l - math::OneOverPi<long double>) < 0.00000000000001l);
    }
    SUBCASE("OneOver2Pi")
    {
        REQUIRE(std::abs(0.15915494309f - math::OneOver2Pi<float>) < 0.0000001f);
        REQUIRE(std::abs(0.159154943091895335769 - math::OneOver2Pi<double>) < 0.0000000000001);
        REQUIRE(std::abs(0.159154943091895335769l - math::OneOver2Pi<long double>) < 0.00000000000001l);
    }
    SUBCASE("PiOver2")
    {
        REQUIRE(std::abs(1.57079632679f - math::PiOver2<float>) < 0.0000001f);
        REQUIRE(std::abs(1.57079632679489661923 - math::PiOver2<double>) < 0.0000000000001);
        REQUIRE(std::abs(1.57079632679489661923l - math::PiOver2<long double>) < 0.00000000000001l);
    }
    SUBCASE("PiOver4")
    {
        REQUIRE(std::abs(0.78539816339f - math::PiOver4<float>) < 0.0000001f);
        REQUIRE(std::abs(0.785398163397448309616 - math::PiOver4<double>) < 0.0000000000001);
        REQUIRE(std::abs(0.785398163397448309616l - math::PiOver4<long double>) < 0.00000000000001l);
    }
}
