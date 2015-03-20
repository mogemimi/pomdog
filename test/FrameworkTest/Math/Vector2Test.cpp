// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include <Pomdog/Math/Vector2.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::Vector2;

TEST(Vector2, TrivialCase)
{
	Vector2 vec {0, 0};
	EXPECT_EQ(vec.X, 0.0f);
	EXPECT_EQ(vec.Y, 0.0f);
	
	vec = {1, 2};
	EXPECT_EQ(vec.X, 1.0f);
	EXPECT_EQ(vec.Y, 2.0f);
}

TEST(Vector2, Addition)
{
	EXPECT_EQ(Vector2(5, 7), Vector2(2, 3) + Vector2(3, 4));
}

TEST(Vector2, Subtraction)
{
	EXPECT_EQ(Vector2(-4, -5), Vector2(2, 3) - Vector2(6, 8));
	EXPECT_EQ(Vector2(+4, +5), Vector2(6, 8) - Vector2(2, 3));
}

TEST(Vector2, Multiply)
{
	Vector2 const result(4.0f * 3.0f, 7.0f * 3.0f);
	
	EXPECT_EQ(result, Vector2(4, 7) * 3);
	EXPECT_EQ(result, Vector2(4, 7) * Vector2(3, 3));
	EXPECT_EQ(result, Vector2(3, 3) * Vector2(4, 7));
	EXPECT_EQ(result, 3.0f * Vector2(4, 7));
}

TEST(Vector2, Division)
{
	EXPECT_EQ(Vector2(10.0f/2, 8.0f/2), Vector2(10, 8) / 2);
	EXPECT_EQ(Vector2(10.0f/2, 8.0f/2), Vector2(10, 8) / Vector2(2, 2));
}
