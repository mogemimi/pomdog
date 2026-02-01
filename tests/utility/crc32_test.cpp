// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/crc32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::u32;
using pomdog::u8;
using pomdog::hash::computeCRC32;

TEST_CASE("CRC32")
{
    SUBCASE("empty string")
    {
        // NOTE: pomdog's CRC32 returns initial value (0xFFFFFFFF) for empty input
        const auto crc = computeCRC32("");
        REQUIRE(crc == 0xFFFFFFFF);
    }
    SUBCASE("hello world")
    {
        const auto crc = computeCRC32("Hello, World!");
        REQUIRE(crc == 0xec4ac3d0);
    }
    SUBCASE("123456789")
    {
        auto crc = computeCRC32("123456789");
        REQUIRE(crc == 0xcbf43926);
    }
    SUBCASE("same input")
    {
        const auto crc1 = computeCRC32("Test");
        const auto crc2 = computeCRC32("Test");
        REQUIRE(crc1 == crc2);

        const auto crc3 = computeCRC32("Different");
        REQUIRE(crc1 != crc3);
    }
    SUBCASE("case sensitive")
    {
        const auto crcLower = computeCRC32("test");
        const auto crcUpper = computeCRC32("TEST");
        REQUIRE(crcLower != crcUpper);
    }
    SUBCASE("known values")
    {
        REQUIRE_EQ(computeCRC32("chuck norris"), u32(995152849U));
        REQUIRE_EQ(computeCRC32("Chuck Norris"), u32(617418015U));
        REQUIRE_EQ(computeCRC32("Fus Ro Dah"), u32(2507294250U));
    }
    SUBCASE("span input")
    {
        POMDOG_CLANG_SUPPRESS_WARNING_PUSH
        POMDOG_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage-in-container")

        const std::string data = "Span test data";
        const auto crc1 = computeCRC32(std::string_view{data});
        const auto crc2 = computeCRC32(std::span<const u8>{
            reinterpret_cast<const u8*>(data.data()),
            data.size() * sizeof(char)});
        REQUIRE(crc1 == crc2);

        POMDOG_CLANG_SUPPRESS_WARNING_POP
    }
}
