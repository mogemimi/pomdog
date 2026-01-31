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

TEST_CASE("Degree32")
{
    SUBCASE("constructor")
    {
        const Degree32 r = 0.5f;
        REQUIRE_EQ(r.get(), 0.5f);
        REQUIRE_EQ(static_cast<f32>(r), 0.5f);
    }
    SUBCASE("binary operators")
    {
        REQUIRE_EQ((Degree32{0.5f} + Degree32{0.2f}).get(), 0.5f + 0.2f);
        REQUIRE_EQ((Degree32{0.5f} - Degree32{0.2f}).get(), 0.5f - 0.2f);
        REQUIRE_EQ((Degree32{0.5f} * Degree32{0.2f}).get(), 0.5f * 0.2f);
        REQUIRE_EQ((Degree32{0.5f} / Degree32{0.2f}).get(), 0.5f / 0.2f);
    }
    SUBCASE("toDegree")
    {
        using pomdog::math::toDegree;
        constexpr auto toDeg = 180.0f / math::Pi<f32>;
        REQUIRE_EQ(toDegree(Radian32{0.0f}), Degree32{0.0f * toDeg});
        REQUIRE_EQ(toDegree(Radian32{1.0f}), Degree32{1.0f * toDeg});
        REQUIRE_EQ(toDegree(Radian32{3.1415926535f}), Degree32{3.1415926535f * toDeg});
        REQUIRE_EQ(toDegree(Radian32{-3.1415926535f}), Degree32{-3.1415926535f * toDeg});

        REQUIRE(toDegree(Radian32{0.0f}).get() == doctest::Approx(0.0));
        REQUIRE(toDegree(Radian32{1.0f}).get() == doctest::Approx(57.2958));
        REQUIRE(toDegree(Radian32{3.1415926535f}).get() == doctest::Approx(180.0));

        REQUIRE_EQ(toDegree(0.0f), Degree32{0.0f * toDeg});
        REQUIRE_EQ(toDegree(1.0f), Degree32{1.0f * toDeg});
        REQUIRE_EQ(toDegree(3.1415926535f), Degree32{3.1415926535f * toDeg});
        REQUIRE_EQ(toDegree(-3.1415926535f), Degree32{-3.1415926535f * toDeg});

        REQUIRE(toDegree(0.0f).get() == doctest::Approx(0.0));
        REQUIRE(toDegree(1.0f).get() == doctest::Approx(57.2958));
        REQUIRE(toDegree(3.1415926535f).get() == doctest::Approx(180.0));

        REQUIRE(toDegree(0.0f).get() == doctest::Approx(0.0).epsilon(0.00001));
        REQUIRE(toDegree(math::Pi<f32>).get() == doctest::Approx(180.0).epsilon(0.00001));
        REQUIRE(toDegree(math::TwoPi<f32>).get() == doctest::Approx(360.0).epsilon(0.00001));
        REQUIRE(toDegree(math::PiOver2<f32>).get() == doctest::Approx(90.0).epsilon(0.00001));
        REQUIRE(toDegree(math::PiOver4<f32>).get() == doctest::Approx(45.0).epsilon(0.00001));
    }
}
