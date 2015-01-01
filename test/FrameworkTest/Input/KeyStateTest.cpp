//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include <Pomdog/Input/KeyState.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::KeyState;

TEST(KeyState, StaticCastBoolean)
{
	static_assert(static_cast<bool>(KeyState::Down) == true, "");
	static_assert(static_cast<bool>(KeyState::Up) == false, "");
}
