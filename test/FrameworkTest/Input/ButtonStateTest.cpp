//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Input/ButtonState.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::ButtonState;

TEST(ButtonState, StaticCastBoolean)
{
	EXPECT_TRUE(static_cast<bool>(ButtonState::Pressed));
	EXPECT_FALSE(static_cast<bool>(ButtonState::Released));
	
	ButtonState buttonState = ButtonState::Pressed;
	EXPECT_TRUE(static_cast<bool>(buttonState));
	
	buttonState = ButtonState::Released;
	EXPECT_FALSE(static_cast<bool>(buttonState));
}
