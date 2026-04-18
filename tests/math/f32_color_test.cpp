// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/f32_color.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::F32Color;

TEST_CASE("F32Color")
{
    SUBCASE("constructor")
    {
        F32Color color{1.0f, 0.5f, 0.25f, 0.75f};
        REQUIRE(color.r == 1.0f);
        REQUIRE(color.g == 0.5f);
        REQUIRE(color.b == 0.25f);
        REQUIRE(color.a == 0.75f);
    }
    SUBCASE("default constructor")
    {
        F32Color color;
        color.r = 0.0f;
        color.g = 0.0f;
        color.b = 0.0f;
        color.a = 0.0f;
        REQUIRE(color.r == 0.0f);
        REQUIRE(color.g == 0.0f);
        REQUIRE(color.b == 0.0f);
        REQUIRE(color.a == 0.0f);
    }
    SUBCASE("addition")
    {
        F32Color a{0.1f, 0.2f, 0.3f, 0.4f};
        F32Color b{0.5f, 0.6f, 0.7f, 0.8f};
        auto c = a + b;
        REQUIRE(c.r == doctest::Approx(0.6));
        REQUIRE(c.g == doctest::Approx(0.8));
        REQUIRE(c.b == doctest::Approx(1.0));
        REQUIRE(c.a == doctest::Approx(1.2));
    }
    SUBCASE("subtraction")
    {
        F32Color a{0.5f, 0.6f, 0.7f, 0.8f};
        F32Color b{0.1f, 0.2f, 0.3f, 0.4f};
        auto c = a - b;
        REQUIRE(c.r == doctest::Approx(0.4));
        REQUIRE(c.g == doctest::Approx(0.4));
        REQUIRE(c.b == doctest::Approx(0.4));
        REQUIRE(c.a == doctest::Approx(0.4));
    }
    SUBCASE("multiply by scalar")
    {
        F32Color a{0.2f, 0.4f, 0.6f, 0.8f};
        auto c = a * 2.0f;
        REQUIRE(c.r == doctest::Approx(0.4));
        REQUIRE(c.g == doctest::Approx(0.8));
        REQUIRE(c.b == doctest::Approx(1.2));
        REQUIRE(c.a == doctest::Approx(1.6));
    }
    SUBCASE("multiply by color")
    {
        F32Color a{0.5f, 0.5f, 0.5f, 0.5f};
        F32Color b{0.4f, 0.6f, 0.8f, 1.0f};
        auto c = a * b;
        REQUIRE(c.r == doctest::Approx(0.2));
        REQUIRE(c.g == doctest::Approx(0.3));
        REQUIRE(c.b == doctest::Approx(0.4));
        REQUIRE(c.a == doctest::Approx(0.5));
    }
    SUBCASE("divide by scalar")
    {
        F32Color a{0.4f, 0.8f, 1.2f, 1.6f};
        auto c = a / 2.0f;
        REQUIRE(c.r == doctest::Approx(0.2));
        REQUIRE(c.g == doctest::Approx(0.4));
        REQUIRE(c.b == doctest::Approx(0.6));
        REQUIRE(c.a == doctest::Approx(0.8));
    }
    SUBCASE("equality")
    {
        F32Color a{1.0f, 0.5f, 0.25f, 0.75f};
        F32Color b{1.0f, 0.5f, 0.25f, 0.75f};
        REQUIRE(a == b);
        REQUIRE_FALSE(a != b);

        F32Color c{1.0f, 0.5f, 0.25f, 0.5f};
        REQUIRE(a != c);
        REQUIRE_FALSE(a == c);
    }
    SUBCASE("negate")
    {
        F32Color a{0.1f, 0.2f, 0.3f, 0.4f};
        auto c = -a;
        REQUIRE(c.r == doctest::Approx(-0.1));
        REQUIRE(c.g == doctest::Approx(-0.2));
        REQUIRE(c.b == doctest::Approx(-0.3));
        REQUIRE(c.a == doctest::Approx(-0.4));
    }
    SUBCASE("data pointer")
    {
        F32Color a{1.0f, 2.0f, 3.0f, 4.0f};
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        const float* ptr = a.data();
        REQUIRE(ptr[0] == 1.0f);
        REQUIRE(ptr[1] == 2.0f);
        REQUIRE(ptr[2] == 3.0f);
        REQUIRE(ptr[3] == 4.0f);
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }
    SUBCASE("abs")
    {
        using pomdog::math::abs;
        F32Color a{-0.1f, -0.2f, 0.3f, -0.4f};
        auto c = abs(a);
        REQUIRE(c.r == doctest::Approx(0.1));
        REQUIRE(c.g == doctest::Approx(0.2));
        REQUIRE(c.b == doctest::Approx(0.3));
        REQUIRE(c.a == doctest::Approx(0.4));
    }
    SUBCASE("compound assignment +=")
    {
        F32Color a{0.1f, 0.2f, 0.3f, 0.4f};
        a += F32Color{0.5f, 0.3f, 0.2f, 0.1f};
        REQUIRE(a.r == doctest::Approx(0.6));
        REQUIRE(a.g == doctest::Approx(0.5));
        REQUIRE(a.b == doctest::Approx(0.5));
        REQUIRE(a.a == doctest::Approx(0.5));
    }
    SUBCASE("compound assignment -=")
    {
        F32Color a{0.5f, 0.6f, 0.7f, 0.8f};
        a -= F32Color{0.1f, 0.2f, 0.3f, 0.4f};
        REQUIRE(a.r == doctest::Approx(0.4));
        REQUIRE(a.g == doctest::Approx(0.4));
        REQUIRE(a.b == doctest::Approx(0.4));
        REQUIRE(a.a == doctest::Approx(0.4));
    }
    SUBCASE("compound assignment *=")
    {
        F32Color a{0.2f, 0.4f, 0.6f, 0.8f};
        a *= 2.0f;
        REQUIRE(a.r == doctest::Approx(0.4));
        REQUIRE(a.g == doctest::Approx(0.8));
        REQUIRE(a.b == doctest::Approx(1.2));
        REQUIRE(a.a == doctest::Approx(1.6));
    }
}
