// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/memory/alignment.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::memory::alignTo;
using pomdog::memory::isPowerOfTwo;

TEST_CASE("Alignment")
{
    SUBCASE("isPowerOfTwo")
    {
        REQUIRE(isPowerOfTwo(1));
        REQUIRE(isPowerOfTwo(2));
        REQUIRE(isPowerOfTwo(4));
        REQUIRE(isPowerOfTwo(8));
        REQUIRE(isPowerOfTwo(16));
        REQUIRE(isPowerOfTwo(32));
        REQUIRE(isPowerOfTwo(64));
        REQUIRE(isPowerOfTwo(128));
        REQUIRE(isPowerOfTwo(256));
        REQUIRE(isPowerOfTwo(512));
        REQUIRE(isPowerOfTwo(1024));

        REQUIRE_FALSE(isPowerOfTwo(0));
        REQUIRE_FALSE(isPowerOfTwo(3));
        REQUIRE_FALSE(isPowerOfTwo(5));
        REQUIRE_FALSE(isPowerOfTwo(6));
        REQUIRE_FALSE(isPowerOfTwo(7));
        REQUIRE_FALSE(isPowerOfTwo(9));
        REQUIRE_FALSE(isPowerOfTwo(10));
        REQUIRE_FALSE(isPowerOfTwo(15));
        REQUIRE_FALSE(isPowerOfTwo(17));
        REQUIRE_FALSE(isPowerOfTwo(100));
    }
    SUBCASE("alignTo")
    {
        constexpr auto ptrcast = [](std::uintptr_t address) -> void* {
            return reinterpret_cast<void*>(address);
        };

        REQUIRE(alignTo(ptrcast(0), 0) == ptrcast(0));
        REQUIRE(alignTo(ptrcast(0), 1) == ptrcast(0));
        REQUIRE(alignTo(ptrcast(0), 2) == ptrcast(0));

        REQUIRE(alignTo(ptrcast(1), 1) == ptrcast(1));
        REQUIRE(alignTo(ptrcast(1), 2) == ptrcast(2));
        REQUIRE(alignTo(ptrcast(1), 16) == ptrcast(16));

        REQUIRE(alignTo(ptrcast(0), 8) == ptrcast(0));
        REQUIRE(alignTo(ptrcast(1), 8) == ptrcast(8));
        REQUIRE(alignTo(ptrcast(6), 8) == ptrcast(8));
        REQUIRE(alignTo(ptrcast(7), 8) == ptrcast(8));
        REQUIRE(alignTo(ptrcast(8), 8) == ptrcast(8));
        REQUIRE(alignTo(ptrcast(9), 8) == ptrcast(16));
        REQUIRE(alignTo(ptrcast(15), 8) == ptrcast(16));
        REQUIRE(alignTo(ptrcast(16), 8) == ptrcast(16));
        REQUIRE(alignTo(ptrcast(17), 8) == ptrcast(24));

        REQUIRE(alignTo(ptrcast(8), 1) == ptrcast(8));
        REQUIRE(alignTo(ptrcast(8), 4) == ptrcast(8));
        REQUIRE(alignTo(ptrcast(8), 8) == ptrcast(8));
        REQUIRE(alignTo(ptrcast(16), 4) == ptrcast(16));
        REQUIRE(alignTo(ptrcast(16), 8) == ptrcast(16));
        REQUIRE(alignTo(ptrcast(16), 16) == ptrcast(16));
        REQUIRE(alignTo(ptrcast(16), 64) == ptrcast(64));

        // Already aligned pointer should not change
        REQUIRE(alignTo(ptrcast(0x100), 4) == ptrcast(0x100));
        REQUIRE(alignTo(ptrcast(0x100), 8) == ptrcast(0x100));
    }
    SUBCASE("alignTo with buffer")
    {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN

        std::array<std::uint8_t, 64> buffer{};
        auto* base = buffer.data();

        // Align to 1 byte
        auto* p1 = alignTo(base, 1);
        REQUIRE(p1 == base);

        // Align to 4 bytes
        auto* p4 = alignTo(base + 1, 4);
        REQUIRE(reinterpret_cast<std::uintptr_t>(p4) % 4 == 0);
        REQUIRE(p4 >= base + 1);

        // Align to 8 bytes
        auto* p8 = alignTo(base + 1, 8);
        REQUIRE(reinterpret_cast<std::uintptr_t>(p8) % 8 == 0);
        REQUIRE(p8 >= base + 1);

        // Align to 16 bytes
        auto* p16 = alignTo(base + 1, 16);
        REQUIRE(reinterpret_cast<std::uintptr_t>(p16) % 16 == 0);
        REQUIRE(p16 >= base + 1);

        POMDOG_CLANG_UNSAFE_BUFFER_END
    }
}
