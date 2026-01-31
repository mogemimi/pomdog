// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/degree.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/radian.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Degree32;
using pomdog::f32;
using pomdog::f64;
using pomdog::Radian32;
namespace math = pomdog::math;

TEST_CASE("Radian32")
{
    SUBCASE("constructor")
    {
        const Radian32 r = 0.5f;
        REQUIRE_EQ(r.get(), 0.5f);
        REQUIRE_EQ(static_cast<f32>(r), 0.5f);
    }
    SUBCASE("binary operators")
    {
        REQUIRE_EQ((Radian32{0.5f} + Radian32{0.2f}).get(), 0.5f + 0.2f);
        REQUIRE_EQ((Radian32{0.5f} - Radian32{0.2f}).get(), 0.5f - 0.2f);
        REQUIRE_EQ((Radian32{0.5f} * Radian32{0.2f}).get(), 0.5f * 0.2f);
        REQUIRE_EQ((Radian32{0.5f} / Radian32{0.2f}).get(), 0.5f / 0.2f);
    }
    SUBCASE("toRadian")
    {
        using pomdog::math::toRadian;
        constexpr auto toRad = math::Pi<f32> / 180.0f;
        REQUIRE_EQ(toRadian(Degree32{0.0f}), Radian32{0.0f * toRad});
        REQUIRE_EQ(toRadian(Degree32{1.0f}), Radian32{1.0f * toRad});
        REQUIRE_EQ(toRadian(Degree32{45.0f}), Radian32{45.0f * toRad});
        REQUIRE_EQ(toRadian(Degree32{90.0f}), Radian32{90.0f * toRad});
        REQUIRE_EQ(toRadian(Degree32{180.0f}), Radian32{180.0f * toRad});
        REQUIRE_EQ(toRadian(Degree32{360.0f}), Radian32{360.0f * toRad});
        REQUIRE_EQ(toRadian(Degree32{-1.0f}), Radian32{-1.0f * toRad});

        REQUIRE(toRadian(Degree32{0.0f}).get() == doctest::Approx(0.0));
        REQUIRE(toRadian(Degree32{1.0f}).get() == doctest::Approx(0.0174533));
        REQUIRE(toRadian(Degree32{180.0f}).get() == doctest::Approx(3.1415926535));

        REQUIRE_EQ(toRadian(0.0f), Radian32{0.0f * toRad});
        REQUIRE_EQ(toRadian(1.0f), Radian32{1.0f * toRad});
        REQUIRE_EQ(toRadian(45.0f), Radian32{45.0f * toRad});
        REQUIRE_EQ(toRadian(90.0f), Radian32{90.0f * toRad});
        REQUIRE_EQ(toRadian(180.0f), Radian32{180.0f * toRad});
        REQUIRE_EQ(toRadian(360.0f), Radian32{360.0f * toRad});
        REQUIRE_EQ(toRadian(-1.0f), Radian32{-1.0f * toRad});

        REQUIRE(toRadian(0.0f).get() == doctest::Approx(0.0));
        REQUIRE(toRadian(1.0f).get() == doctest::Approx(0.0174533));
        REQUIRE(toRadian(180.0f).get() == doctest::Approx(3.1415926535));

        REQUIRE(toRadian(0.0f).get() == doctest::Approx(0.0).epsilon(0.00001));
        REQUIRE(toRadian(180.0f).get() == doctest::Approx(math::Pi<f64>).epsilon(0.00001));
        REQUIRE(toRadian(360.0f).get() == doctest::Approx(math::TwoPi<f64>).epsilon(0.00001));
        REQUIRE(toRadian(90.0f).get() == doctest::Approx(math::PiOver2<f64>).epsilon(0.00001));
        REQUIRE(toRadian(45.0f).get() == doctest::Approx(math::PiOver4<f64>).epsilon(0.00001));
    }
}
