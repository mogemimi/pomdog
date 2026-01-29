// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/degree.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/radian.h"
#include <catch_amalgamated.hpp>

namespace math = pomdog::math;
using pomdog::f32;
using pomdog::f64;

TEST_CASE("MathConstants", "[MathConstants]")
{
    SECTION("Constants")
    {
        // NOTE: float
        REQUIRE(math::Pi<f32> == 3.1415926535f);
        REQUIRE(math::TwoPi<f32> == 6.2831853071f);
        REQUIRE(math::OneOverPi<f32> == 0.3183098861f);
        REQUIRE(math::OneOver2Pi<f32> == 0.1591549430f);
        REQUIRE(math::PiOver2<f32> == 1.5707963267f);
        REQUIRE(math::PiOver4<f32> == 0.7853981633f);

        // NOTE: double
        REQUIRE(math::Pi<f64> == 3.14159265358979323846);
        REQUIRE(math::TwoPi<f64> == 6.28318530717958647692);
        REQUIRE(math::OneOverPi<f64> == 0.31830988618379067154);
        REQUIRE(math::OneOver2Pi<f64> == 0.15915494309189533576);
        REQUIRE(math::PiOver2<f64> == 1.57079632679489661923);
        REQUIRE(math::PiOver4<f64> == 0.78539816339744830962);
    }
}
