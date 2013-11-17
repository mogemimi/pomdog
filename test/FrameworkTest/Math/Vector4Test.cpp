//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <pomdog/Math/Vector4.hpp>

TEST(TrivialCase, Vector4Test)
{
	using Pomdog::Vector4;
	
	Vector4 vec {0, 0, 0, 0};
	EXPECT_EQ(vec.x, 0.0f);
	EXPECT_EQ(vec.y, 0.0f);
	EXPECT_EQ(vec.z, 0.0f);
	EXPECT_EQ(vec.w, 0.0f);
	
	vec = {1, 2, 3, 4};
	EXPECT_EQ(vec.x, 1.0f);
	EXPECT_EQ(vec.y, 2.0f);
	EXPECT_EQ(vec.z, 3.0f);
	EXPECT_EQ(vec.w, 4.0f);
}

TEST(Addition, Vector4Test)
{
	using Pomdog::Vector4;
	
	EXPECT_EQ(Vector4(1.f + 5.f, 2.f + 6.f, 3.f + 7.f, 4.f + 8.f), Vector4(1, 2, 3, 4) + Vector4(5, 6, 7, 8));
	EXPECT_EQ(Vector4(1.f - 5.f, 2.f - 6.f, 3.f - 7.f, 4.f - 8.f), Vector4(1, 2, 3, 4) + Vector4(-5, -6, -7, -8));
}

TEST(Subtraction, Vector4Test)
{
	using Pomdog::Vector4;
	
	EXPECT_EQ(Vector4(1.f - 5.f, 2.f - 6.f, 3.f - 7.f, 4.f - 8.f), Vector4(1, 2, 3, 4) - Vector4(5, 6, 7, 8));
	EXPECT_EQ(Vector4(1.f + 5.f, 2.f + 6.f, 3.f + 7.f, 4.f + 8.f), Vector4(1, 2, 3, 4) - Vector4(-5, -6, -7, -8));
}

TEST(Multiply, Vector4Test)
{
	using Pomdog::Vector4;
	
	EXPECT_EQ(Vector4(4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f), Vector4(4, 5, 6, 7) * Vector4(3, 3, 3, 3));
	EXPECT_EQ(Vector4(4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f), Vector4(3, 3, 3, 3) * Vector4(4, 5, 6, 7));
	EXPECT_EQ(Vector4(4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f), Vector4(4, 5, 6, 7) * 3);
	EXPECT_EQ(Vector4(4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f), 3.0f * Vector4(4, 5, 6, 7));
}

TEST(Division, Vector4Test)
{
	using Pomdog::Vector4;
	
	EXPECT_EQ(Vector4(4.f / 3.f, 5.f / 3.f, 6.f / 3.f, 7.f / 3.f), Vector4(4, 5, 6, 7) / Vector4(3, 3, 3, 3));
	EXPECT_EQ(Vector4(3.f / 4.f, 3.f / 5.f, 3.f / 6.f, 3.f / 7.f), Vector4(3, 3, 3, 3) / Vector4(4, 5, 6, 7));
	EXPECT_EQ(Vector4(4.f / 3.f, 5.f / 3.f, 6.f / 3.f, 7.f / 3.f), Vector4(4, 5, 6, 7) / 3.0f);
}
