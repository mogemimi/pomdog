// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/f32_rect2d.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::F32Rect2D;
using pomdog::Vector2;

TEST_CASE("F32Rect2D")
{
    SUBCASE("minX maxX minY maxY")
    {
        F32Rect2D rect = {10.0f, 20.0f, 30.0f, 40.0f};
        REQUIRE(rect.minX() == 10.0f);
        REQUIRE(rect.maxX() == 40.0f);
        REQUIRE(rect.minY() == 20.0f);
        REQUIRE(rect.maxY() == 60.0f);
    }
    SUBCASE("center")
    {
        F32Rect2D rect = {10.0f, 20.0f, 30.0f, 40.0f};
        auto c = rect.center();
        REQUIRE(c.x == doctest::Approx(25.0));
        REQUIRE(c.y == doctest::Approx(40.0));
    }
    SUBCASE("location")
    {
        F32Rect2D rect = {10.0f, 20.0f, 30.0f, 40.0f};
        auto loc = rect.location();
        REQUIRE(loc.x == 10.0f);
        REQUIRE(loc.y == 20.0f);
    }
    SUBCASE("contains point")
    {
        F32Rect2D rect = {10.0f, 20.0f, 30.0f, 40.0f};
        REQUIRE(rect.contains(10.0f, 20.0f));
        REQUIRE(rect.contains(40.0f, 60.0f));
        REQUIRE(rect.contains(25.0f, 40.0f));

        REQUIRE_FALSE(rect.contains(9.9f, 20.0f));
        REQUIRE_FALSE(rect.contains(40.1f, 20.0f));
        REQUIRE_FALSE(rect.contains(10.0f, 19.9f));
        REQUIRE_FALSE(rect.contains(10.0f, 60.1f));
    }
    SUBCASE("contains Vector2")
    {
        F32Rect2D rect = {10.0f, 20.0f, 30.0f, 40.0f};
        REQUIRE(rect.contains(Vector2{25.0f, 40.0f}));
        REQUIRE_FALSE(rect.contains(Vector2{9.9f, 20.0f}));
    }
    SUBCASE("createFromMinMax")
    {
        auto rect = F32Rect2D::createFromMinMax(Vector2{10.0f, 20.0f}, Vector2{40.0f, 60.0f});
        REQUIRE(rect.x == 10.0f);
        REQUIRE(rect.y == 20.0f);
        REQUIRE(rect.width == 30.0f);
        REQUIRE(rect.height == 40.0f);
    }
    SUBCASE("approxEqual")
    {
        F32Rect2D a = {10.0f, 20.0f, 30.0f, 40.0f};
        F32Rect2D b = {10.0f, 20.0f, 30.0f, 40.0f};
        REQUIRE(pomdog::math::approxEqual(a, b));

        F32Rect2D c = {10.1f, 20.0f, 30.0f, 40.0f};
        REQUIRE_FALSE(pomdog::math::approxEqual(a, c, 0.01f));
    }
}
