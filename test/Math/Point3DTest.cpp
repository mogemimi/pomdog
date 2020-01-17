// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Point3D.hpp"
#include "catch.hpp"

using Pomdog::Point3D;

TEST_CASE("Point3D", "[Point3D]")
{
    SECTION("FirstTestCase")
    {
        Point3D coodinate{0, 0, 0};
        REQUIRE(coodinate.X == 0);
        REQUIRE(coodinate.Y == 0);
        REQUIRE(coodinate.Z == 0);

        coodinate = {1, 2, 3};
        REQUIRE(coodinate.X == 1);
        REQUIRE(coodinate.Y == 2);
        REQUIRE(coodinate.Z == 3);

        coodinate = {
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::denorm_min()
        };
        REQUIRE(std::numeric_limits<int>::max() == coodinate.X);
        REQUIRE(std::numeric_limits<int>::min() == coodinate.Y);
        REQUIRE(std::numeric_limits<int>::denorm_min() == coodinate.Z);
    }
    SECTION("Constants")
    {
        REQUIRE(Point3D{0, 0, 0} == Point3D::Zero);
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
