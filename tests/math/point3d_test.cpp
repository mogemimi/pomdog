// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/point3d.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::i32;
using pomdog::Point3D;
using pomdog::Vector3;
namespace math = pomdog::math;

TEST_CASE("Point3D")
{
    SUBCASE("first test case")
    {
        Point3D coodinate{0, 0, 0};
        REQUIRE(coodinate.x == 0);
        REQUIRE(coodinate.y == 0);
        REQUIRE(coodinate.z == 0);

        coodinate = {1, 2, 3};
        REQUIRE(coodinate.x == 1);
        REQUIRE(coodinate.y == 2);
        REQUIRE(coodinate.z == 3);

        coodinate = {
            std::numeric_limits<i32>::max(),
            std::numeric_limits<i32>::min(),
            std::numeric_limits<i32>::denorm_min(),
        };
        REQUIRE(coodinate.x == std::numeric_limits<i32>::max());
        REQUIRE(coodinate.y == std::numeric_limits<i32>::min());
        REQUIRE(coodinate.z == std::numeric_limits<i32>::denorm_min());
    }
    SUBCASE("constants")
    {
        REQUIRE(Point3D{0, 0, 0} == Point3D::createZero());
    }
    SUBCASE("add")
    {
        REQUIRE(Point3D{0, 0, 0} == Point3D{0, 0, 0} + Point3D{0, 0, 0});
        REQUIRE(Point3D{7, 9, 11} == Point3D{4, 5, 6} + Point3D{3, 4, 5});
    }
    SUBCASE("subtract")
    {
        REQUIRE(Point3D{0, 0, 0} == Point3D{0, 0, 0} - Point3D{0, 0, 0});
        REQUIRE(Point3D{2 - 5, 3 - 4, 4 - 3} == Point3D{2, 3, 4} - Point3D{5, 4, 3});
    }
    SUBCASE("multiply")
    {
        REQUIRE(Point3D{0, 0, 0} == Point3D{0, 0, 0} * 0);
        REQUIRE(Point3D{0, 0, 0} == Point3D{7, 8, 9} * 0);
        REQUIRE(Point3D{0, 0, 0} == Point3D{0, 0, 0} * 10);
        REQUIRE(Point3D{70, 80, 90} == Point3D{7, 8, 9} * 10);
    }
    SUBCASE("multiply")
    {
        REQUIRE(Point3D{0, 0, 0} == 0 * Point3D{0, 0, 0});
        REQUIRE(Point3D{0, 0, 0} == 0 * Point3D{7, 8, 9});
        REQUIRE(Point3D{0, 0, 0} == 10 * Point3D{0, 0, 0});
        REQUIRE(Point3D{70, 80, 90} == 10 * Point3D{7, 8, 9});
    }
    SUBCASE("multiply pointwise")
    {
        REQUIRE(Point3D{0, 0, 0} == Point3D{0, 0, 0} * Point3D{0, 0, 0});
        REQUIRE(Point3D{21, 40, 99} == Point3D{7, 8, 9} * Point3D{3, 5, 11});
    }
    SUBCASE("divide")
    {
        REQUIRE(Point3D{0, 0, 0} == Point3D{0, 0, 0} / 1);
        REQUIRE(Point3D{7, 8, 9} == Point3D{7, 8, 9} / 1);
        REQUIRE(Point3D{0, 0, 0} == Point3D{0, 0, 0} / 9);
        REQUIRE(Point3D{70 / 10, 80 / 10, 90 / 10} == Point3D{70, 80, 90} / 10);
    }
    SUBCASE("divide")
    {
        REQUIRE(Point3D{0, 0, 0} == Point3D{0, 0, 0} / Point3D{1, 1, 1});
        REQUIRE(Point3D{1, 1, 1} == Point3D{7, 8, 9} / Point3D{7, 8, 9});
        REQUIRE(Point3D{0, 0, 0} == Point3D{0, 0, 0} / Point3D{9, 9, 9});
        REQUIRE(Point3D{70 / 5, 80 / 10, 90 / 15} == Point3D{70, 80, 90} / Point3D{5, 10, 15});
    }
    SUBCASE("toPoint3D")
    {
        REQUIRE(math::toPoint3D(Vector3{6.0f, 7.0f, 8.0f}) == Point3D{6, 7, 8});
        REQUIRE(math::toPoint3D(Vector3{-6.0f, -7.0f, -8.0f}) == Point3D{-6, -7, -8});
    }
    SUBCASE("toVector3")
    {
        REQUIRE(math::toVector3(Point3D{6, 7, 8}) == Vector3{6.0f, 7.0f, 8.0f});
        REQUIRE(math::toVector3(Point3D{-6, -7, -8}) == Vector3{-6.0f, -7.0f, -8.0f});
    }
    SUBCASE("abs")
    {
        REQUIRE(math::abs(Point3D{6, 7, 8}) == Point3D{6, 7, 8});
        REQUIRE(math::abs(Point3D{-6, -7, -8}) == Point3D{6, 7, 8});
        REQUIRE(math::abs(Point3D{-6, 7, 8}) == Point3D{6, 7, 8});
        REQUIRE(math::abs(Point3D{6, -7, 8}) == Point3D{6, 7, 8});
        REQUIRE(math::abs(Point3D{6, 7, -8}) == Point3D{6, 7, 8});
    }
}
