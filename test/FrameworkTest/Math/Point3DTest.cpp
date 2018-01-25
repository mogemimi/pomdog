// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/Point3D.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::Point3D;

TEST(Point3D, FirstTestCase)
{
    Point3D coodinate{0, 0, 0};
    EXPECT_EQ(coodinate.X, 0);
    EXPECT_EQ(coodinate.Y, 0);
    EXPECT_EQ(coodinate.Z, 0);

    coodinate = {1, 2, 3};
    EXPECT_EQ(coodinate.X, 1);
    EXPECT_EQ(coodinate.Y, 2);
    EXPECT_EQ(coodinate.Z, 3);

    coodinate = {
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::denorm_min()
    };
    EXPECT_EQ(coodinate.X, std::numeric_limits<int>::max());
    EXPECT_EQ(coodinate.Y, std::numeric_limits<int>::min());
    EXPECT_EQ(coodinate.Z, std::numeric_limits<int>::denorm_min());
}

TEST(Point3D, Constants)
{
    EXPECT_EQ(Point3D(0, 0, 0), Point3D::Zero);
}

TEST(Point3D, Addition)
{
    EXPECT_EQ(Point3D(0, 0, 0), Point3D(0, 0, 0) + Point3D(0, 0, 0));
    EXPECT_EQ(Point3D(7, 9, 11), Point3D(2, 3, 4) + Point3D(5, 6, 7));
}

TEST(Point3D, Subtraction)
{
    EXPECT_EQ(Point3D(0, 0, 0), Point3D(0, 0, 0) - Point3D(0, 0, 0));
    EXPECT_EQ(Point3D(2 - 5, 3 - 6, 4 - 7), Point3D(2, 3, 4) - Point3D(5, 6, 7));
}

TEST(Point3D, Multiply)
{
    Point3D const coordinate{3, 4, 5};

    EXPECT_EQ(Point3D(9, 16, 25), coordinate * coordinate);
    EXPECT_EQ(Point3D(6, 8, 10), coordinate * 2);
    EXPECT_EQ(Point3D(6, 8, 10), 2 * coordinate);
}

TEST(Point3D, Division)
{
    Point3D const coordinate{6, 8, 10};

    EXPECT_EQ(Point3D(1, 1, 1), coordinate / coordinate);
    EXPECT_EQ(Point3D(3, 4, 5), coordinate / 2);
}
