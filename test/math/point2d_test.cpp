// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/point2d.h"
#include "pomdog/math/vector2.h"
#include <catch_amalgamated.hpp>

using pomdog::Point2D;
using pomdog::Vector2;

TEST_CASE("Point2D", "[Point2D]")
{
    SECTION("FirstTestCase")
    {
        Point2D coodinate{0, 0};
        REQUIRE(coodinate.x == 0);
        REQUIRE(coodinate.y == 0);

        coodinate = {1, 2};
        REQUIRE(coodinate.x == 1);
        REQUIRE(coodinate.y == 2);

        coodinate = {
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
        };
        REQUIRE(std::numeric_limits<int>::max() == coodinate.x);
        REQUIRE(std::numeric_limits<int>::min() == coodinate.y);
    }
    SECTION("Constants")
    {
        REQUIRE(Point2D{0, 0} == Point2D::createZero());
    }
    SECTION("Addition")
    {
        REQUIRE(Point2D{0, 0} == Point2D{0, 0} + Point2D{0, 0});
        REQUIRE(Point2D{6, 8} == Point2D{2, 3} + Point2D{4, 5});
    }
    SECTION("Subtraction")
    {
        REQUIRE(Point2D{0, 0} == Point2D{0, 0} - Point2D{0, 0});
        REQUIRE(Point2D{-4, -5} == Point2D{2, 3} - Point2D{6, 8});
        REQUIRE(Point2D{+4, +5} == Point2D{6, 8} - Point2D{2, 3});
    }
    SECTION("Multiply")
    {
        Point2D const coordinate{3, 4};

        REQUIRE(Point2D{9, 16} == coordinate * coordinate);
        REQUIRE(Point2D{6, 8} == coordinate * 2);
        REQUIRE(Point2D{6, 8} == 2 * coordinate);
    }
    SECTION("Division")
    {
        Point2D const coordinate{6, 8};

        REQUIRE(Point2D{1, 1} == coordinate / coordinate);
        REQUIRE(Point2D{3, 4} == coordinate / 2);
    }
    SECTION("toPoint2D")
    {
        REQUIRE(pomdog::math::toPoint2D(Vector2{6.0f, 7.0f}) == Point2D{6, 7});
        REQUIRE(pomdog::math::toPoint2D(Vector2{-6.0f, -7.0f}) == Point2D{-6, -7});
    }
    SECTION("toVector2")
    {
        REQUIRE(pomdog::math::toVector2(Point2D{6, 7}) == Vector2{6.0f, 7.0f});
        REQUIRE(pomdog::math::toVector2(Point2D{-6, -7}) == Vector2{-6.0f, -7.0f});
    }
    SECTION("abs")
    {
        REQUIRE(pomdog::math::abs(Point2D{6, 7}) == Point2D{6, 7});
        REQUIRE(pomdog::math::abs(Point2D{-6, -7}) == Point2D{6, 7});
        REQUIRE(pomdog::math::abs(Point2D{-6, 7}) == Point2D{6, 7});
        REQUIRE(pomdog::math::abs(Point2D{6, -7}) == Point2D{6, 7});
    }
}
