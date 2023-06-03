// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/gamepad_uuid.h"
#include <catch_amalgamated.hpp>

using pomdog::GamepadUUID;

TEST_CASE("GamepadUUID::ToString", "[GamepadUUID]")
{
    GamepadUUID uuid;
    uuid.busType = 0x2301;
    uuid.vendorID = 0x6745;
    uuid.productID = 0xab89;
    uuid.versionNumber = 0xefcd;
    REQUIRE(uuid.toString() == "012300004567000089ab0000cdef0000");
}
