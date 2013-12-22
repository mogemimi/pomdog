//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <Pomdog/Math/Vector3.hpp>

using Pomdog::Vector3;

TEST(Vector3, TrivialCase)
{
	Vector3 vec {0, 0, 0};
	EXPECT_EQ(vec.x, 0.0f);
	EXPECT_EQ(vec.y, 0.0f);
	EXPECT_EQ(vec.z, 0.0f);
	
	vec = {1, 2, 3};
	EXPECT_EQ(vec.x, 1.0f);
	EXPECT_EQ(vec.y, 2.0f);
	EXPECT_EQ(vec.z, 3.0f);
}

TEST(Vector3, Addition)
{
	EXPECT_EQ(Vector3(1.0f + 4.0f, 2.0f + 5.0f, 3.0f + 6.0f), Vector3(1, 2, 3) + Vector3(4, 5, 6));
	EXPECT_EQ(Vector3(1.0f - 4.0f, 2.0f - 5.0f, 3.0f - 6.0f), Vector3(1, 2, 3) + Vector3(-4, -5, -6));
}

TEST(Vector3, Subtraction)
{
	EXPECT_EQ(Vector3(1.0f - 4.0f, 2.0f - 5.0f, 3.0f - 6.0f), Vector3(1, 2, 3) - Vector3(4, 5, 6));
	EXPECT_EQ(Vector3(4.0f - 1.0f, 5.0f - 2.0f, 6.0f - 3.0f), Vector3(4, 5, 6) - Vector3(1, 2, 3));
}

TEST(Vector3, Multiply)
{
	EXPECT_EQ(Vector3(4.0f * 3.0f, 5.0f * 3.0f, 6.0f * 3.0f), Vector3(4, 5, 6) * Vector3(3, 3, 3));
	EXPECT_EQ(Vector3(4.0f * 3.0f, 5.0f * 3.0f, 6.0f * 3.0f), Vector3(3, 3, 3) * Vector3(4, 5, 6));
	EXPECT_EQ(Vector3(4.0f * 3.0f, 5.0f * 3.0f, 6.0f * 3.0f), Vector3(4, 5, 6) * 3.0f);
	EXPECT_EQ(Vector3(4.0f * 3.0f, 5.0f * 3.0f, 6.0f * 3.0f), 3.0f * Vector3(4, 5, 6));
}

TEST(Vector3, Division)
{
	EXPECT_EQ(Vector3(4.0f/3.0f, 5.0f/3.0f, 6.0f/3.0f), Vector3(4, 5, 6) / Vector3(3, 3, 3));
	EXPECT_EQ(Vector3(4.0f/3.0f, 5.0f/3.0f, 6.0f/3.0f), Vector3(4, 5, 6) / 3.0f);
	EXPECT_EQ(Vector3(3.0f/4.0f, 3.0f/5.0f, 3.0f/6.0f), Vector3(3, 3, 3) / Vector3(4, 5, 6));
}
