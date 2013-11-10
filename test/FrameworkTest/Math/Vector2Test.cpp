//
//  Copyright (C) 2013 the Pomdog Engine authors.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <pomdog/Math/Float2.hpp>

TEST(TrivialCase, Float2Test)
{
	using Pomdog::Float2;
	
	Float2 vec {0, 0};
	EXPECT_EQ(vec.x, 0.0f);
	EXPECT_EQ(vec.y, 0.0f);
	
	vec = {1, 2};
	EXPECT_EQ(vec.x, 1.0f);
	EXPECT_EQ(vec.y, 2.0f);
}
