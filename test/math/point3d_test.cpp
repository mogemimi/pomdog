// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/point3d.h"
#include <catch_amalgamated.hpp>

using pomdog::Point3D;

TEST_CASE("Point3D", "[Point3D]")
{
    SECTION("FirstTestCase")
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
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::denorm_min(),
        };
        REQUIRE(std::numeric_limits<int>::max() == coodinate.x);
        REQUIRE(std::numeric_limits<int>::min() == coodinate.y);
        REQUIRE(std::numeric_limits<int>::denorm_min() == coodinate.z);
    }
    SECTION("Constants")
    {
        REQUIRE(Point3D{0, 0, 0} == Point3D::Zero());
    }
    SECTION("Addition")
    {
        REQUIRE(Point3D{0, 0, 0} == Point3D{0, 0, 0} + Point3D{0, 0, 0});
        REQUIRE(Point3D{7, 9, 11} == Point3D{2, 3, 4} + Point3D{5, 6, 7});
    }
    SECTION("Subtraction")
    {
        REQUIRE(Point3D{0, 0, 0} == Point3D{0, 0, 0} - Point3D{0, 0, 0});
        REQUIRE(Point3D{2 - 5, 3 - 6, 4 - 7} == Point3D{2, 3, 4} - Point3D{5, 6, 7});
    }
    SECTION("Multiply")
    {
        Point3D const coordinate{3, 4, 5};

        REQUIRE(Point3D{9, 16, 25} == coordinate * coordinate);
        REQUIRE(Point3D{6, 8, 10} == coordinate * 2);
        REQUIRE(Point3D{6, 8, 10} == 2 * coordinate);
    }
    SECTION("Division")
    {
        Point3D const coordinate{6, 8, 10};

        REQUIRE(Point3D{1, 1, 1} == coordinate / coordinate);
        REQUIRE(Point3D{3, 4, 5} == coordinate / 2);
    }
}
