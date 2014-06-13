//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <gtest/iutest_switch.hpp>
#include <Pomdog/Input/KeyState.hpp>

using Pomdog::KeyState;

TEST(KeyState, StaticCastBoolean)
{
	EXPECT_TRUE(static_cast<bool>(KeyState::Down));
	EXPECT_FALSE(static_cast<bool>(KeyState::Up));
	
	KeyState keyState = KeyState::Down;
	EXPECT_TRUE(static_cast<bool>(keyState));
	
	keyState = KeyState::Up;
	EXPECT_FALSE(static_cast<bool>(keyState));
}
