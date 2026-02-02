// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/input/gamepad_uuid.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::GamepadUUID;

TEST_CASE("GamepadUUID")
{
    SUBCASE("default constructor")
    {
        GamepadUUID uuid = {};
        REQUIRE(uuid.busType == 0);
        REQUIRE(uuid.vendorID == 0);
        REQUIRE(uuid.productID == 0);
        REQUIRE(uuid.versionNumber == 0);
    }
    SUBCASE("set values")
    {
        GamepadUUID uuid;
        uuid.busType = 0x03;
        uuid.vendorID = 0x054C;
        uuid.productID = 0x0CE6;
        uuid.versionNumber = 0x0100;

        REQUIRE(uuid.busType == 0x03);
        REQUIRE(uuid.vendorID == 0x054C);
        REQUIRE(uuid.productID == 0x0CE6);
        REQUIRE(uuid.versionNumber == 0x0100);
    }
    SUBCASE("compare")
    {
        GamepadUUID uuid1;
        uuid1.busType = 0x03;
        uuid1.vendorID = 0x054C;
        uuid1.productID = 0x0CE6;
        uuid1.versionNumber = 0x0100;

        GamepadUUID uuid2;
        uuid2.busType = 0x03;
        uuid2.vendorID = 0x054C;
        uuid2.productID = 0x0CE6;
        uuid2.versionNumber = 0x0100;

        GamepadUUID uuid3;
        uuid3.busType = 0x03;
        uuid3.vendorID = 0x045E;
        uuid3.productID = 0x028E;
        uuid3.versionNumber = 0x0114;

        REQUIRE(uuid1.vendorID == uuid2.vendorID);
        REQUIRE(uuid1.productID == uuid2.productID);
        REQUIRE(uuid1.vendorID != uuid3.vendorID);
        REQUIRE(uuid1.productID != uuid3.productID);
    }
    SUBCASE("toString")
    {
        GamepadUUID uuid;
        uuid.busType = 0x2301;
        uuid.vendorID = 0x6745;
        uuid.productID = 0xab89;
        uuid.versionNumber = 0xefcd;

        const auto str = uuid.toString();
        REQUIRE(str == "012300004567000089ab0000cdef0000");
    }
}
