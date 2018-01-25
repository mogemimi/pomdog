// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/Point2D.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::Point2D;

TEST(Point2D, FirstTestCase)
{
    Point2D coodinate{0, 0};
    EXPECT_EQ(coodinate.X, 0);
    EXPECT_EQ(coodinate.Y, 0);

    coodinate = {1, 2};
    EXPECT_EQ(coodinate.X, 1);
    EXPECT_EQ(coodinate.Y, 2);

    coodinate = {
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min()
    };
    EXPECT_EQ(coodinate.X, std::numeric_limits<int>::max());
    EXPECT_EQ(coodinate.Y, std::numeric_limits<int>::min());
}

TEST(Point2D, Constants)
{
    EXPECT_EQ(Point2D(0, 0), Point2D::Zero);
}

TEST(Point2D, Addition)
{
    EXPECT_EQ(Point2D(0, 0), Point2D(0, 0) + Point2D(0, 0));
    EXPECT_EQ(Point2D(6, 8), Point2D(2, 3) + Point2D(4, 5));
}

TEST(Point2D, Subtraction)
{
    EXPECT_EQ(Point2D(0, 0), Point2D(0, 0) - Point2D(0, 0));
    EXPECT_EQ(Point2D(-4, -5), Point2D(2, 3) - Point2D(6, 8));
    EXPECT_EQ(Point2D(+4, +5), Point2D(6, 8) - Point2D(2, 3));
}

TEST(Point2D, Multiply)
{
    Point2D const coordinate{3, 4};

    EXPECT_EQ(Point2D(9, 16), coordinate * coordinate);
    EXPECT_EQ(Point2D(6, 8), coordinate * 2);
    EXPECT_EQ(Point2D(6, 8), 2 * coordinate);
}

TEST(Point2D, Division)
{
    Point2D const coordinate{6, 8};

    EXPECT_EQ(Point2D(1, 1), coordinate / coordinate);
    EXPECT_EQ(Point2D(3, 4), coordinate / 2);
}
