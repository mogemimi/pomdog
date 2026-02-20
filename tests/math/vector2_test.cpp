// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Vector2;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("Vector2")
{
    SUBCASE("constructor")
    {
        Vector2 vec{0, 0};
        REQUIRE(vec.x == 0.0f);
        REQUIRE(vec.y == 0.0f);

        vec = {1, 2};
        REQUIRE(vec.x == 1.0f);
        REQUIRE(vec.y == 2.0f);
    }
    SUBCASE("constants")
    {
        REQUIRE(Vector2{0.0f, 0.0f} == Vector2::createZero());
    }
    SUBCASE("addition")
    {
        REQUIRE(Vector2{5, 7} == Vector2{2, 3} + Vector2{3, 4});
    }
    SUBCASE("subtraction")
    {
        REQUIRE(Vector2{-4, -5} == Vector2{2, 3} - Vector2{6, 8});
        REQUIRE(Vector2{+4, +5} == Vector2{6, 8} - Vector2{2, 3});
    }
    SUBCASE("multiply")
    {
        Vector2 const result(4.0f * 3.0f, 7.0f * 3.0f);

        REQUIRE(result == Vector2{4, 7} * 3);
        REQUIRE(result == Vector2{4, 7} * Vector2{3, 3});
        REQUIRE(result == Vector2{3, 3} * Vector2{4, 7});
        REQUIRE(result == 3.0f * Vector2{4, 7});
    }
    SUBCASE("division")
    {
        REQUIRE(Vector2{10.0f / 2, 8.0f / 2} == Vector2{10, 8} / 2);
        REQUIRE(Vector2{10.0f / 2, 8.0f / 2} == Vector2{10, 8} / Vector2{2, 2});
    }
    SUBCASE("lerp")
    {
        using pomdog::math::lerp;
        REQUIRE(Vector2{0.0f, 0.0f} == lerp(Vector2::createZero(), Vector2{40.0f, 50.0f}, 0.0f));
        REQUIRE(Vector2{4.0f, 5.0f} == lerp(Vector2::createZero(), Vector2{40.0f, 50.0f}, 0.1f));
        REQUIRE(Vector2{36.f, 45.f} == lerp(Vector2::createZero(), Vector2{40.0f, 50.0f}, 0.9f));
        REQUIRE(Vector2{40.f, 50.f} == lerp(Vector2::createZero(), Vector2{40.0f, 50.0f}, 1.0f));

        REQUIRE(Vector2{1.0f, 2.0f} == lerp(Vector2{1.0f, 2.0f}, Vector2{4.0f, 5.0f}, 0.0f));
        REQUIRE(Vector2{2.5f, 3.5f} == lerp(Vector2{1.0f, 2.0f}, Vector2{4.0f, 5.0f}, 0.5f));
        REQUIRE(Vector2{4.0f, 5.0f} == lerp(Vector2{1.0f, 2.0f}, Vector2{4.0f, 5.0f}, 1.0f));
    }
    SUBCASE("normalize")
    {
        using pomdog::math::normalize;
        REQUIRE(normalize(Vector2{0.0f, 0.0f}) == Vector2{0.0f, 0.0f});
        REQUIRE(normalize(Vector2{1.0f, 0.0f}) == Vector2{1.0f, 0.0f});
        REQUIRE(normalize(Vector2{0.0f, 1.0f}) == Vector2{0.0f, 1.0f});
    }
    SUBCASE("rotate")
    {
        using pomdog::math::rotate;
        REQUIRE(rotate(Vector2{0.0f, 0.0f}, 0.0f) == Vector2{0.0f, 0.0f});

        {
            auto vec = rotate(Vector2{1.0f, 0.0f}, 0.0f);
            REQUIRE(vec.x == approx(1.0f));
            REQUIRE(vec.y == approx(0.0f));
        }
        {
            auto vec = rotate(Vector2{1.0f, 0.0f}, pomdog::math::PiOver2<float>);
            REQUIRE(vec.x == approx(0.0f));
            REQUIRE(vec.y == approx(1.0f));
        }
        {
            auto vec = rotate(Vector2{1.0f, 0.0f}, pomdog::math::Pi<float>);
            REQUIRE(vec.x == approx(-1.0f));
            REQUIRE(vec.y == approx(0.0f));
        }
        {
            auto vec = rotate(Vector2{1.0f, 0.0f}, -pomdog::math::PiOver2<float>);
            REQUIRE(vec.x == approx(0.0f));
            REQUIRE(vec.y == approx(-1.0f));
        }
        {
            auto vec = rotate(Vector2{1.0f, 0.0f}, pomdog::math::TwoPi<float>);
            REQUIRE(vec.x == approx(1.0f));
            REQUIRE(vec.y == approx(0.0f));
        }
    }
}
