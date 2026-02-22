// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/point2d.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::i32;
using pomdog::Point2D;
using pomdog::Vector2;
namespace math = pomdog::math;

TEST_CASE("Point2D")
{
    SUBCASE("constructor")
    {
        Point2D coordinate{0, 0};
        REQUIRE(coordinate.x == 0);
        REQUIRE(coordinate.y == 0);

        coordinate = {1, 2};
        REQUIRE(coordinate.x == 1);
        REQUIRE(coordinate.y == 2);

        coordinate = {
            std::numeric_limits<i32>::max(),
            std::numeric_limits<i32>::min(),
        };
        REQUIRE(coordinate.x == std::numeric_limits<i32>::max());
        REQUIRE(coordinate.y == std::numeric_limits<i32>::min());
    }
    SUBCASE("constants")
    {
        REQUIRE(Point2D{0, 0} == Point2D::createZero());
    }
    SUBCASE("add")
    {
        REQUIRE(Point2D{0, 0} == Point2D{0, 0} + Point2D{0, 0});
        REQUIRE(Point2D{7, 9} == Point2D{4, 5} + Point2D{3, 4});
    }
    SUBCASE("subtract")
    {
        REQUIRE(Point2D{0, 0} == Point2D{0, 0} - Point2D{0, 0});
        REQUIRE(Point2D{2 - 5, 3 - 4} == Point2D{2, 3} - Point2D{5, 4});
    }
    SUBCASE("multiply by scalar")
    {
        REQUIRE(Point2D{0, 0} == Point2D{0, 0} * 0);
        REQUIRE(Point2D{0, 0} == Point2D{7, 8} * 0);
        REQUIRE(Point2D{0, 0} == Point2D{0, 0} * 9);
        REQUIRE(Point2D{63, 72} == Point2D{7, 8} * 9);
    }
    SUBCASE("scalar multiply")
    {
        REQUIRE(Point2D{0, 0} == 0 * Point2D{0, 0});
        REQUIRE(Point2D{0, 0} == 0 * Point2D{7, 8});
        REQUIRE(Point2D{0, 0} == 9 * Point2D{0, 0});
        REQUIRE(Point2D{63, 72} == 9 * Point2D{7, 8});
    }
    SUBCASE("multiply pointwise")
    {
        REQUIRE(Point2D{0, 0} == Point2D{0, 0} * Point2D{0, 0});
        REQUIRE(Point2D{21, 72} == Point2D{7, 8} * Point2D{3, 9});
    }
    SUBCASE("divide by scalar")
    {
        REQUIRE(Point2D{0, 0} == Point2D{0, 0} / 1);
        REQUIRE(Point2D{7, 8} == Point2D{7, 8} / 1);
        REQUIRE(Point2D{0, 0} == Point2D{0, 0} / 9);
        REQUIRE(Point2D{70 / 9, 80 / 9} == Point2D{70, 80} / 9);
    }
    SUBCASE("divide pointwise")
    {
        REQUIRE(Point2D{0, 0} == Point2D{0, 0} / Point2D{1, 1});
        REQUIRE(Point2D{1, 1} == Point2D{7, 8} / Point2D{7, 8});
        REQUIRE(Point2D{0, 0} == Point2D{0, 0} / Point2D{9, 9});
        REQUIRE(Point2D{70 / 5, 80 / 10} == Point2D{70, 80} / Point2D{5, 10});
    }
    SUBCASE("toPoint2D")
    {
        REQUIRE(math::toPoint2D(Vector2{6.0f, 7.0f}) == Point2D{6, 7});
        REQUIRE(math::toPoint2D(Vector2{-6.0f, -7.0f}) == Point2D{-6, -7});
    }
    SUBCASE("toVector2")
    {
        REQUIRE(math::toVector2(Point2D{6, 7}) == Vector2{6.0f, 7.0f});
        REQUIRE(math::toVector2(Point2D{-6, -7}) == Vector2{-6.0f, -7.0f});
    }
    SUBCASE("abs")
    {
        REQUIRE(math::abs(Point2D{6, 7}) == Point2D{6, 7});
        REQUIRE(math::abs(Point2D{-6, -7}) == Point2D{6, 7});
        REQUIRE(math::abs(Point2D{-6, 7}) == Point2D{6, 7});
        REQUIRE(math::abs(Point2D{6, -7}) == Point2D{6, 7});
    }
}
