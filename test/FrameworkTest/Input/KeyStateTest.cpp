//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <Pomdog/Input/KeyState.hpp>

using Pomdog::KeyState;

TEST(KeyState, StaticCastBoolean)
{
	EXPECT_TRUE(static_cast<bool>(KeyState::Pressed));
	EXPECT_FALSE(static_cast<bool>(KeyState::Released));
	
	KeyState keyState = KeyState::Pressed;
	EXPECT_TRUE(static_cast<bool>(keyState));
	
	keyState = KeyState::Released;
	EXPECT_FALSE(static_cast<bool>(keyState));
}
