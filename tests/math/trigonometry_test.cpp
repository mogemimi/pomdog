// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/trigonometry.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::f64;
using pomdog::i32;
namespace math = pomdog::math;

namespace {

/// Returns sin(pi * x) and cos(pi * x) computed in f64 after reducing x exactly.
[[nodiscard]] std::tuple<f64, f64>
referenceSinCosPi(f64 x)
{
    const f64 a = std::fmod(std::abs(x), 2.0);
    const f64 n = std::nearbyint(a * 2.0);
    const f64 r = a - n * 0.5;
    const f64 sinR = std::sin(math::Pi<f64> * r);
    const f64 cosR = std::cos(math::Pi<f64> * r);

    f64 sinValue = 0.0;
    f64 cosValue = 0.0;
    switch (static_cast<i32>(n) & 3) {
    case 0:
        sinValue = sinR;
        cosValue = cosR;
        break;
    case 1:
        sinValue = cosR;
        cosValue = -sinR;
        break;
    case 2:
        sinValue = -sinR;
        cosValue = -cosR;
        break;
    default:
        sinValue = -cosR;
        cosValue = sinR;
        break;
    }
    if (std::signbit(x)) {
        sinValue = -sinValue;
    }
    return {sinValue, cosValue};
}

/// Returns the distance from value to reference in units of the f32 ulp at reference.
[[nodiscard]] f64
ulpDistance(f32 value, f64 reference)
{
    i32 exponent = 0;
    [[maybe_unused]] const f64 mantissa = std::frexp(reference, &exponent);
    const f64 ulp = std::ldexp(1.0, std::max(exponent - 24, -149));
    return std::abs(static_cast<f64>(value) - reference) / ulp;
}

} // namespace

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
    SUBCASE("values around the end of the fast reduction")
    {
        // NOTE: At and above 2^21 (f32) or 2^50 (f64), the reduction calls
        // std::fmod before rounding to the nearest multiple of 0.5.
        REQUIRE_EQ(math::sinpi(2097151.5f), -1.0f);
        REQUIRE_EQ(math::sinpi(2097152.5f), 1.0f);
        REQUIRE(math::sinpi(2097151.75f) == doctest::Approx(-0.70710678));
        REQUIRE(math::sinpi(2097152.25f) == doctest::Approx(0.70710678));

        REQUIRE_EQ(math::sinpi(0x1p50 - 0.5), -1.0);
        REQUIRE_EQ(math::sinpi(0x1p50 + 0.5), 1.0);
        REQUIRE(math::sinpi(0x1p50 - 0.25) == doctest::Approx(-0.70710678118654752).epsilon(1e-15));
        REQUIRE(math::sinpi(0x1p50 + 0.25) == doctest::Approx(0.70710678118654752).epsilon(1e-15));
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
    SUBCASE("f32 results are within 1.13 ulp")
    {
        // NOTE: The two fixed inputs are where an exhaustive check over all
        // f32 inputs in [0, 2] found the largest error.
        const auto check = [](f32 x) {
            CAPTURE(x);
            const auto [s, c] = math::sincospi(x);
            const auto [expectedSin, expectedCos] = referenceSinCosPi(static_cast<f64>(x));
            REQUIRE(ulpDistance(s, expectedSin) <= 1.13);
            REQUIRE(ulpDistance(c, expectedCos) <= 1.13);
        };
        check(0.250895977f);
        check(0.249104023f);
        for (i32 i = -4096; i <= 4096; ++i) {
            check(static_cast<f32>(i) * 0.000977517f);
        }
    }
    SUBCASE("f64 results match a reference")
    {
        for (i32 i = -4096; i <= 4096; ++i) {
            const f64 x = static_cast<f64>(i) * 0.000977517;
            CAPTURE(x);
            const auto [s, c] = math::sincospi(x);
            const auto [expectedSin, expectedCos] = referenceSinCosPi(x);
            REQUIRE(s == doctest::Approx(expectedSin).epsilon(1e-15));
            REQUIRE(c == doctest::Approx(expectedCos).epsilon(1e-15));
        }
    }
}
