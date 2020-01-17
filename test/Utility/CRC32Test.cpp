// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/detail/CRC32.hpp"
#include "catch.hpp"

using Pomdog::Detail::CRC32;

TEST_CASE("CRC32", "[CRC32]")
{
    SECTION("ComputeCRC32")
    {
        REQUIRE(CRC32::ComputeCRC32<char>("chuck norris") == 995152849U);
        REQUIRE(CRC32::ComputeCRC32<char>("Chuck Norris") == 617418015U);
        REQUIRE(CRC32::ComputeCRC32<char>("Fus Ro Dah") == 2507294250U);

        REQUIRE(995152849U == CRC32::ComputeCRC32(std::string("chuck norris")));
        REQUIRE(617418015U == CRC32::ComputeCRC32(std::string("Chuck Norris")));
    }
    SECTION("Vector")
    {
        REQUIRE(CRC32::ComputeCRC32<char>("Chuck Norris") == 617418015U);

        std::string name = "Chuck Norris";
        REQUIRE(CRC32::ComputeCRC32(name.data(), name.size() * sizeof(char)) == 617418015U);

        std::vector<std::uint8_t> vec8 = { 0x43, 0x68, 0x75, 0x63, 0x6b, 0x20, 0x4e, 0x6f, 0x72, 0x72, 0x69, 0x73 };
        REQUIRE(CRC32::ComputeCRC32(vec8.data(), vec8.size() * sizeof(std::uint8_t)) == 617418015U);
    }
    SECTION("Utf8String")
    {
        // UTF-8 (without BOM) encoding
        REQUIRE(CRC32::ComputeCRC32<char>(
            "\xe3\x81\xb2\xe3\x81\xa0\xe3\x81\xbe\xe3\x82\x8a"
            "\xe3\x82\xb9\xe3\x82\xb1\xe3\x83\x83\xe3\x83\x81") == 2656274492U);
        REQUIRE(CRC32::ComputeCRC32<char>(
            "\xe3\x81\x82\xe3\x81\x81\x5e\xef\xbd\x9e\xe5\xbf"
            "\x83\xe3\x81\x8c\xe3\x81\xb4\xe3\x82\x87\xe3\x82"
            "\x93\xe3\x81\xb4\xe3\x82\x87\xe3\x82\x93\xe3\x81"
            "\x99\xe3\x82\x8b\xe3\x82\x93\xe3\x81\x98\xe3\x82"
            "\x83\xe3\x81\x81\x5e\xef\xbd\x9e") == 143237079U);
    }
}
