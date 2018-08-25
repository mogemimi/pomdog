// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Input/GamepadUUID.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::GamepadUUID;

TEST(GamepadUUID, ToString)
{
    GamepadUUID uuid;
    uuid.BusType = 0x2301;
    uuid.VendorID = 0x6745;
    uuid.ProductID = 0xab89;
    uuid.VersionNumber = 0xefcd;
    EXPECT_EQ("012300004567000089ab0000cdef0000", uuid.ToString());
}
