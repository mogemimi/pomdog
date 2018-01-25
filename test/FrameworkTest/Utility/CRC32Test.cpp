// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Utility/detail/CRC32.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::Detail::CRC32;

TEST(CRC32, FirstCase)
{
    EXPECT_EQ(995152849U, CRC32::ComputeCRC32<char>("chuck norris"));
    EXPECT_EQ(617418015U, CRC32::ComputeCRC32<char>("Chuck Norris"));
    EXPECT_EQ(2507294250U, CRC32::ComputeCRC32<char>("Fus Ro Dah"));

    EXPECT_EQ(995152849U, CRC32::ComputeCRC32(std::string("chuck norris")));
    EXPECT_EQ(617418015U, CRC32::ComputeCRC32(std::string("Chuck Norris")));
}

TEST(CRC32, Vector)
{
    EXPECT_EQ(617418015U, CRC32::ComputeCRC32<char>("Chuck Norris"));

    std::string name = "Chuck Norris";
    EXPECT_EQ(617418015U, CRC32::ComputeCRC32(name.data(), name.size() * sizeof(char)));

    std::vector<std::uint8_t> vec8 = { 0x43, 0x68, 0x75, 0x63, 0x6b, 0x20, 0x4e, 0x6f, 0x72, 0x72, 0x69, 0x73 };
    EXPECT_EQ(617418015U, CRC32::ComputeCRC32(vec8.data(), vec8.size() * sizeof(std::uint8_t)));
}

TEST(CRC32, Utf8String)
{
    // UTF-8 (without BOM) encoding
    EXPECT_EQ(2656274492U, CRC32::ComputeCRC32<char>(
        "\xe3\x81\xb2\xe3\x81\xa0\xe3\x81\xbe\xe3\x82\x8a"
        "\xe3\x82\xb9\xe3\x82\xb1\xe3\x83\x83\xe3\x83\x81"));
    EXPECT_EQ(143237079U, CRC32::ComputeCRC32<char>(
        "\xe3\x81\x82\xe3\x81\x81\x5e\xef\xbd\x9e\xe5\xbf"
        "\x83\xe3\x81\x8c\xe3\x81\xb4\xe3\x82\x87\xe3\x82"
        "\x93\xe3\x81\xb4\xe3\x82\x87\xe3\x82\x93\xe3\x81"
        "\x99\xe3\x82\x8b\xe3\x82\x93\xe3\x81\x98\xe3\x82"
        "\x83\xe3\x81\x81\x5e\xef\xbd\x9e"));
}
