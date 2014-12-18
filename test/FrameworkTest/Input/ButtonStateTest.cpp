//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include <Pomdog/Input/ButtonState.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::ButtonState;

TEST(ButtonState, StaticCastBoolean)
{
	static_assert(static_cast<bool>(ButtonState::Pressed) == true, "");
	static_assert(static_cast<bool>(ButtonState::Released) == false, "");
}
