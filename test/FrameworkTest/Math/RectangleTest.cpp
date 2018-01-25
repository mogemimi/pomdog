// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/Rectangle.hpp>
#include <Pomdog/Math/Point2D.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::Rectangle;
using Pomdog::Point2D;

TEST(Rectangle, TrivialCase)
{
    using Pomdog::Rectangle;
    Rectangle rect = {320, 240, 640, 480};
    EXPECT_EQ(320, rect.X);
    EXPECT_EQ(240, rect.Y);
    EXPECT_EQ(640, rect.Width);
    EXPECT_EQ(480, rect.Height);
    EXPECT_EQ(Point2D(320, 240), rect.GetLocation());

    EXPECT_EQ(320, rect.GetLeft());
    EXPECT_EQ(320 + 640, rect.GetRight());
    EXPECT_EQ(240, rect.GetTop());
    EXPECT_EQ(240 + 480, rect.GetBottom());
}

TEST(Rectangle, Location)
{
    using Pomdog::Rectangle;
    Rectangle rect = {320, 240, 640, 480};
    ASSERT_EQ(320, rect.X);
    ASSERT_EQ(240, rect.Y);
    ASSERT_EQ(640, rect.Width);
    ASSERT_EQ(480, rect.Height);
    EXPECT_EQ(Point2D(320, 240), rect.GetLocation());

    rect.SetLocation(Point2D{-42, -24});
    EXPECT_EQ(Point2D(-42, -24), rect.GetLocation());
    EXPECT_EQ(-42, rect.X);
    EXPECT_EQ(-24, rect.Y);
    ASSERT_EQ(640, rect.Width);
    ASSERT_EQ(480, rect.Height);
    EXPECT_EQ(-42, rect.GetLeft());
    EXPECT_EQ(-42 + 640, rect.GetRight());
    EXPECT_EQ(-24, rect.GetTop());
    EXPECT_EQ(-24 + 480, rect.GetBottom());
}

TEST(Rectangle, Inflate)
{
    using Pomdog::Rectangle;
    Rectangle rect = {320, 240, 640, 480};
    rect.Inflate(96, 42);
    EXPECT_EQ(320 - 96, rect.X);
    EXPECT_EQ(240 - 42, rect.Y);
    ASSERT_EQ(640 + (96 * 2), rect.Width);
    ASSERT_EQ(480 + (42 * 2), rect.Height);
    EXPECT_EQ(320 - 96, rect.GetLeft());
    EXPECT_EQ(320 + 640 + 96, rect.GetRight());
    EXPECT_EQ(240 - 42, rect.GetTop());
    EXPECT_EQ(240 + 480 + 42, rect.GetBottom());
}

TEST(Rectangle, Offset)
{
    using Pomdog::Rectangle;
    Rectangle rect = {320, 240, 640, 480};
    rect.Offset(96, 42);
    EXPECT_EQ(320 + 96, rect.X);
    EXPECT_EQ(240 + 42, rect.Y);
    ASSERT_EQ(640, rect.Width);
    ASSERT_EQ(480, rect.Height);
    EXPECT_EQ(320 + 96, rect.GetLeft());
    EXPECT_EQ(320 + 96 + 640, rect.GetRight());
    EXPECT_EQ(240 + 42, rect.GetTop());
    EXPECT_EQ(240 + 42 + 480, rect.GetBottom());
}

TEST(Rectangle, Contains_Point2D)
{
    using Pomdog::Rectangle;
    Rectangle rect = {32, 24, 64, 48};
    const int left = rect.GetLeft();
    const int right = rect.GetRight();
    const int top = rect.GetTop();
    const int bottom = rect.GetBottom();
    const int centerX = rect.GetCenter().X;
    const int centerY = rect.GetCenter().Y;

    EXPECT_TRUE(rect.Contains(left, top));
    EXPECT_TRUE(rect.Contains(centerX, centerY));
    EXPECT_FALSE(rect.Contains(left - 1, centerY));

    EXPECT_TRUE(rect.Contains({left, top}));
    EXPECT_TRUE(rect.Contains({left, centerY}));
    EXPECT_TRUE(rect.Contains({left, bottom}));
    EXPECT_TRUE(rect.Contains({right, top}));
    EXPECT_TRUE(rect.Contains({right, centerY}));
    EXPECT_TRUE(rect.Contains({right, bottom}));
    EXPECT_TRUE(rect.Contains({left, top}));
    EXPECT_TRUE(rect.Contains({centerX, top}));
    EXPECT_TRUE(rect.Contains({right, top}));
    EXPECT_TRUE(rect.Contains({left, bottom}));
    EXPECT_TRUE(rect.Contains({centerX, bottom}));
    EXPECT_TRUE(rect.Contains({right, bottom}));

    EXPECT_TRUE(rect.Contains({centerX, centerY}));
    EXPECT_TRUE(rect.Contains({left + 1, top + 1}));
    EXPECT_TRUE(rect.Contains({left + 1, bottom - 1}));
    EXPECT_TRUE(rect.Contains({right - 1, top + 1}));
    EXPECT_TRUE(rect.Contains({right - 1, bottom - 1}));

    EXPECT_FALSE(rect.Contains({left - 1, centerY}));
    EXPECT_FALSE(rect.Contains({right + 1, centerY}));
    EXPECT_FALSE(rect.Contains({centerX, top - 1}));
    EXPECT_FALSE(rect.Contains({centerX, bottom + 1}));
}

TEST(Rectangle, Intersects_Point2D)
{
    using Pomdog::Rectangle;
    Rectangle rect = {32, 24, 64, 48};
    const int left = rect.GetLeft();
    const int right = rect.GetRight();
    const int top = rect.GetTop();
    const int bottom = rect.GetBottom();
    const int centerX = rect.GetCenter().X;
    const int centerY = rect.GetCenter().Y;

    EXPECT_TRUE(rect.Contains({left, top}));
    EXPECT_TRUE(rect.Contains({left, centerY}));
    EXPECT_TRUE(rect.Contains({left, bottom}));
    EXPECT_TRUE(rect.Contains({right, top}));
    EXPECT_TRUE(rect.Contains({right, centerY}));
    EXPECT_TRUE(rect.Contains({right, bottom}));
    EXPECT_TRUE(rect.Contains({left, top}));
    EXPECT_TRUE(rect.Contains({centerX, top}));
    EXPECT_TRUE(rect.Contains({right, top}));
    EXPECT_TRUE(rect.Contains({left, bottom}));
    EXPECT_TRUE(rect.Contains({centerX, bottom}));
    EXPECT_TRUE(rect.Contains({right, bottom}));

    EXPECT_TRUE(rect.Contains({centerX, centerY}));
    EXPECT_TRUE(rect.Contains({left + 1, top + 1}));
    EXPECT_TRUE(rect.Contains({left + 1, bottom - 1}));
    EXPECT_TRUE(rect.Contains({right - 1, top + 1}));
    EXPECT_TRUE(rect.Contains({right - 1, bottom - 1}));

    EXPECT_FALSE(rect.Contains({left - 1, centerY}));
    EXPECT_FALSE(rect.Contains({right + 1, centerY}));
    EXPECT_FALSE(rect.Contains({centerX, top - 1}));
    EXPECT_FALSE(rect.Contains({centerX, bottom + 1}));
}

TEST(Rectangle, Contains_Rectangle)
{
    using Pomdog::Rectangle;
    Rectangle rect = {32, 24, 64, 48};

    EXPECT_TRUE(rect.Contains(rect));
    EXPECT_TRUE(rect.Contains({32, 24, 64, 48}));
    EXPECT_FALSE(rect.Contains({32 - 1, 24, 64, 48}));
    EXPECT_FALSE(rect.Contains({32, 24 - 1, 64, 48}));
    EXPECT_FALSE(rect.Contains({32, 24, 64 + 1, 48}));
    EXPECT_FALSE(rect.Contains({32, 24, 64, 48 + 1}));
}

TEST(Rectangle, Intersects_Rectangle)
{
    using Pomdog::Rectangle;
    Rectangle rect = {32, 24, 64, 48};

    EXPECT_TRUE(rect.Intersects({32, 24, 64, 48}));
    EXPECT_TRUE(rect.Intersects({32 - 1, 24, 64, 48}));
    EXPECT_TRUE(rect.Intersects({32, 24 - 1, 64, 48}));
    EXPECT_TRUE(rect.Intersects({32, 24, 64 + 1, 48}));
    EXPECT_TRUE(rect.Intersects({32, 24, 64, 48 + 1}));
}
