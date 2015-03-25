// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include <Pomdog/Input/ButtonState.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::ButtonState;

TEST(ButtonState, StaticCastBoolean)
{
    static_assert(static_cast<bool>(ButtonState::Pressed) == true, "");
    static_assert(static_cast<bool>(ButtonState::Released) == false, "");
}
