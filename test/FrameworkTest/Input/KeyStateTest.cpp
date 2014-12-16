//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Input/KeyState.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::KeyState;

TEST(KeyState, StaticCastBoolean)
{
	static_assert(static_cast<bool>(KeyState::Down) == true, "");
	static_assert(static_cast<bool>(KeyState::Up) == false, "");
}
