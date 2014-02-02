//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <gtest/iutest_switch.hpp>
#include <Pomdog/Math/Quaternion.hpp>

using Pomdog::Quaternion;

TEST(Quaternion, TrivialCase)
{
	Quaternion quaternion{0, 0, 0, 1};
	EXPECT_EQ(quaternion.x, 0.0f);
	EXPECT_EQ(quaternion.y, 0.0f);
	EXPECT_EQ(quaternion.z, 0.0f);
	EXPECT_EQ(quaternion.w, 1.0f);
	
	quaternion = {4, 3, 2, 1};
	EXPECT_EQ(quaternion.x, 4.0f);
	EXPECT_EQ(quaternion.y, 3.0f);
	EXPECT_EQ(quaternion.z, 2.0f);
	EXPECT_EQ(quaternion.w, 1.0f);
}
