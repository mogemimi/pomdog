// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/compilation_target_architecture.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cstdint>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using namespace pomdog;

TEST_CASE("types")
{
    SUBCASE("type sizes")
    {
        REQUIRE(sizeof(i8) == 1);
        REQUIRE(sizeof(i16) == 2);
        REQUIRE(sizeof(i32) == 4);
        REQUIRE(sizeof(i64) == 8);

        REQUIRE(sizeof(u8) == 1);
        REQUIRE(sizeof(u16) == 2);
        REQUIRE(sizeof(u32) == 4);
        REQUIRE(sizeof(u64) == 8);

        REQUIRE(sizeof(f32) == 4);
        REQUIRE(sizeof(f64) == 8);
    }

    SUBCASE("integer signedness")
    {
        REQUIRE(std::is_signed_v<i8>);
        REQUIRE(std::is_signed_v<i16>);
        REQUIRE(std::is_signed_v<i32>);
        REQUIRE(std::is_signed_v<i64>);

        REQUIRE(std::is_unsigned_v<u8>);
        REQUIRE(std::is_unsigned_v<u16>);
        REQUIRE(std::is_unsigned_v<u32>);
        REQUIRE(std::is_unsigned_v<u64>);
    }

    SUBCASE("byte order runtime verification")
    {
        const std::uint32_t value = 0x01020304;
        const auto* bytes = reinterpret_cast<const std::uint8_t*>(&value);

        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
#if defined(POMDOG_BYTEORDER_LITTLE_ENDIAN)
        REQUIRE(bytes[0] == 0x04);
        REQUIRE(bytes[3] == 0x01);
#elif defined(POMDOG_BYTEORDER_BIG_ENDIAN)
        REQUIRE(bytes[0] == 0x01);
        REQUIRE(bytes[3] == 0x04);
#endif
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }
}
