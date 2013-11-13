//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <pomdog/Math/Vector3.hpp>

TEST(TrivialCase, Vector3Test)
{
	using Pomdog::Vector3;
	
	Vector3 vec {0, 0, 0};
	EXPECT_EQ(vec.x, 0.0f);
	EXPECT_EQ(vec.y, 0.0f);
	EXPECT_EQ(vec.z, 0.0f);
	
	vec = {1, 2, 3};
	EXPECT_EQ(vec.x, 1.0f);
	EXPECT_EQ(vec.y, 2.0f);
	EXPECT_EQ(vec.z, 3.0f);
}
