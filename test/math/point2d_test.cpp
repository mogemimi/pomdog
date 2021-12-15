// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/point2d.hpp"
#include <catch_amalgamated.hpp>

using pomdog::Point2D;

TEST_CASE("Point2D", "[Point2D]")
{
    SECTION("FirstTestCase")
    {
        Point2D coodinate{0, 0};
        REQUIRE(coodinate.X == 0);
        REQUIRE(coodinate.Y == 0);

        coodinate = {1, 2};
        REQUIRE(coodinate.X == 1);
        REQUIRE(coodinate.Y == 2);

        coodinate = {
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
        };
        REQUIRE(std::numeric_limits<int>::max() == coodinate.X);
        REQUIRE(std::numeric_limits<int>::min() == coodinate.Y);
    }
    SECTION("Constants")
    {
        REQUIRE(Point2D{0, 0} == Point2D::Zero);
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
}
