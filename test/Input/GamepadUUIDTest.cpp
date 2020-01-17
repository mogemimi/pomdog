// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Input/GamepadUUID.hpp"
#include "catch.hpp"

using Pomdog::GamepadUUID;

TEST_CASE("GamepadUUID::ToString", "[GamepadUUID]")
{
    GamepadUUID uuid;
    uuid.BusType = 0x2301;
    uuid.VendorID = 0x6745;
    uuid.ProductID = 0xab89;
    uuid.VersionNumber = 0xefcd;
    REQUIRE(uuid.ToString() == "012300004567000089ab0000cdef0000");
}
