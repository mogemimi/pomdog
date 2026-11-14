// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/trigonometry.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cmath>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::f64;
using pomdog::i32;
namespace math = pomdog::math;

TEST_CASE("sinpi")
{
    SUBCASE("multiples of 0.5 are exact")
    {
        REQUIRE_EQ(math::sinpi(0.0f), 0.0f);
        REQUIRE_EQ(math::sinpi(0.5f), 1.0f);
        REQUIRE_EQ(math::sinpi(1.0f), 0.0f);
        REQUIRE_EQ(math::sinpi(1.5f), -1.0f);
        REQUIRE_EQ(math::sinpi(2.0f), 0.0f);
        REQUIRE_EQ(math::sinpi(2.5f), 1.0f);
        REQUIRE_EQ(math::sinpi(-0.5f), -1.0f);
        REQUIRE_EQ(math::sinpi(-1.0f), 0.0f);
        REQUIRE_EQ(math::sinpi(-3.5f), 1.0f);
        REQUIRE_EQ(math::sinpi(100.5f), 1.0f);

        REQUIRE_EQ(math::sinpi(0.5), 1.0);
        REQUIRE_EQ(math::sinpi(1.0), 0.0);
        REQUIRE_EQ(math::sinpi(1.5), -1.0);
        REQUIRE_EQ(math::sinpi(-0.5), -1.0);
    }
    SUBCASE("signs of zero")
    {
        REQUIRE_FALSE(std::signbit(math::sinpi(0.0f)));
        REQUIRE(std::signbit(math::sinpi(-0.0f)));
        REQUIRE_FALSE(std::signbit(math::sinpi(1.0f)));
        REQUIRE(std::signbit(math::sinpi(-1.0f)));
        REQUIRE_FALSE(std::signbit(math::sinpi(2.0f)));
        REQUIRE(std::signbit(math::sinpi(-2.0f)));

        REQUIRE_FALSE(std::signbit(math::sinpi(1.0)));
        REQUIRE(std::signbit(math::sinpi(-1.0)));
    }
    SUBCASE("large values")
    {
        // NOTE: Floats at and above 2^23 are integers, and at and above 2^24 even integers.
        REQUIRE_EQ(math::sinpi(4194304.5f), 1.0f);
        REQUIRE_EQ(math::sinpi(8388609.0f), 0.0f);
        REQUIRE_EQ(math::sinpi(16777216.0f), 0.0f);
        REQUIRE_EQ(math::sinpi(std::numeric_limits<f32>::max()), 0.0f);
        REQUIRE_EQ(math::sinpi(std::numeric_limits<f64>::max()), 0.0);
    }
    SUBCASE("non-finite values")
    {
        REQUIRE(std::isnan(math::sinpi(std::numeric_limits<f32>::infinity())));
        REQUIRE(std::isnan(math::sinpi(-std::numeric_limits<f32>::infinity())));
        REQUIRE(std::isnan(math::sinpi(std::numeric_limits<f32>::quiet_NaN())));
        REQUIRE(std::isnan(math::sinpi(std::numeric_limits<f64>::infinity())));
        REQUIRE(std::isnan(math::sinpi(std::numeric_limits<f64>::quiet_NaN())));
    }
    SUBCASE("matches sin(pi * x)")
    {
        for (i32 i = -1000; i <= 1000; ++i) {
            const f32 x = static_cast<f32>(i) * 0.0137f;
            CAPTURE(x);
            const auto expected = std::sin(static_cast<f64>(x) * math::Pi<f64>);
            REQUIRE(math::sinpi(x) == doctest::Approx(expected).epsilon(1e-6));
            REQUIRE(math::sinpi(static_cast<f64>(x)) == doctest::Approx(expected).epsilon(1e-12));
        }
    }
}

TEST_CASE("cospi")
{
    SUBCASE("multiples of 0.5 are exact")
    {
        REQUIRE_EQ(math::cospi(0.0f), 1.0f);
        REQUIRE_EQ(math::cospi(0.5f), 0.0f);
        REQUIRE_EQ(math::cospi(1.0f), -1.0f);
        REQUIRE_EQ(math::cospi(1.5f), 0.0f);
        REQUIRE_EQ(math::cospi(2.0f), 1.0f);
        REQUIRE_EQ(math::cospi(-0.5f), 0.0f);
        REQUIRE_EQ(math::cospi(-1.0f), -1.0f);
        REQUIRE_EQ(math::cospi(-3.0f), -1.0f);
        REQUIRE_EQ(math::cospi(100.0f), 1.0f);

        REQUIRE_EQ(math::cospi(0.0), 1.0);
        REQUIRE_EQ(math::cospi(0.5), 0.0);
        REQUIRE_EQ(math::cospi(1.0), -1.0);
        REQUIRE_EQ(math::cospi(-1.0), -1.0);
    }
    SUBCASE("signs of zero")
    {
        REQUIRE_FALSE(std::signbit(math::cospi(0.5f)));
        REQUIRE_FALSE(std::signbit(math::cospi(1.5f)));
        REQUIRE_FALSE(std::signbit(math::cospi(-0.5f)));
        REQUIRE_FALSE(std::signbit(math::cospi(-1.5f)));

        REQUIRE_FALSE(std::signbit(math::cospi(0.5)));
        REQUIRE_FALSE(std::signbit(math::cospi(-0.5)));
    }
    SUBCASE("large values")
    {
        REQUIRE_EQ(math::cospi(4194304.5f), 0.0f);
        REQUIRE_EQ(math::cospi(8388609.0f), -1.0f);
        REQUIRE_EQ(math::cospi(16777216.0f), 1.0f);
        REQUIRE_EQ(math::cospi(std::numeric_limits<f32>::max()), 1.0f);
        REQUIRE_EQ(math::cospi(std::numeric_limits<f64>::max()), 1.0);
    }
    SUBCASE("non-finite values")
    {
        REQUIRE(std::isnan(math::cospi(std::numeric_limits<f32>::infinity())));
        REQUIRE(std::isnan(math::cospi(-std::numeric_limits<f32>::infinity())));
        REQUIRE(std::isnan(math::cospi(std::numeric_limits<f32>::quiet_NaN())));
        REQUIRE(std::isnan(math::cospi(std::numeric_limits<f64>::infinity())));
        REQUIRE(std::isnan(math::cospi(std::numeric_limits<f64>::quiet_NaN())));
    }
    SUBCASE("matches cos(pi * x)")
    {
        for (i32 i = -1000; i <= 1000; ++i) {
            const f32 x = static_cast<f32>(i) * 0.0137f;
            CAPTURE(x);
            const auto expected = std::cos(static_cast<f64>(x) * math::Pi<f64>);
            REQUIRE(math::cospi(x) == doctest::Approx(expected).epsilon(1e-6));
            REQUIRE(math::cospi(static_cast<f64>(x)) == doctest::Approx(expected).epsilon(1e-12));
        }
    }
}

TEST_CASE("sincospi")
{
    SUBCASE("matches sinpi and cospi")
    {
        for (i32 i = -1000; i <= 1000; ++i) {
            const f32 x = static_cast<f32>(i) * 0.0137f;
            CAPTURE(x);
            {
                const auto [s, c] = math::sincospi(x);
                REQUIRE_EQ(s, math::sinpi(x));
                REQUIRE_EQ(c, math::cospi(x));
            }
            {
                const auto [s, c] = math::sincospi(static_cast<f64>(x));
                REQUIRE_EQ(s, math::sinpi(static_cast<f64>(x)));
                REQUIRE_EQ(c, math::cospi(static_cast<f64>(x)));
            }
        }
    }
    SUBCASE("signs of zero match sinpi and cospi")
    {
        for (i32 i = -8; i <= 8; ++i) {
            const f32 x = static_cast<f32>(i) * 0.5f;
            CAPTURE(x);
            const auto [s, c] = math::sincospi(x);
            REQUIRE_EQ(s, math::sinpi(x));
            REQUIRE_EQ(c, math::cospi(x));
            REQUIRE_EQ(std::signbit(s), std::signbit(math::sinpi(x)));
            REQUIRE_EQ(std::signbit(c), std::signbit(math::cospi(x)));
        }
    }
    SUBCASE("non-finite values")
    {
        const auto [s, c] = math::sincospi(std::numeric_limits<f32>::infinity());
        REQUIRE(std::isnan(s));
        REQUIRE(std::isnan(c));
    }
}
