// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/memory/alignment.h"
#include <catch_amalgamated.hpp>
#include <array>
#include <cstdint>
#include <string>

using namespace pomdog;

TEST_CASE("alignment", "[memory]")
{
    using pomdog::memory::alignTo;
    using pomdog::memory::isPowerOfTwo;

    SECTION("isPowerOfTwo")
    {
        REQUIRE(isPowerOfTwo(1));
        REQUIRE(isPowerOfTwo(2));
        REQUIRE(isPowerOfTwo(4));
        REQUIRE(isPowerOfTwo(8));
        REQUIRE(isPowerOfTwo(16));
        REQUIRE(isPowerOfTwo(32));
        REQUIRE(isPowerOfTwo(64));
        REQUIRE(isPowerOfTwo(256));
        REQUIRE(isPowerOfTwo(512));
        REQUIRE(isPowerOfTwo(1024));

        REQUIRE(!isPowerOfTwo(0));
        REQUIRE(!isPowerOfTwo(3));
        REQUIRE(!isPowerOfTwo(5));
        REQUIRE(!isPowerOfTwo(6));
        REQUIRE(!isPowerOfTwo(7));
        REQUIRE(!isPowerOfTwo(9));
        REQUIRE(!isPowerOfTwo(10));
        REQUIRE(!isPowerOfTwo(11));
    }
    SECTION("alignTo")
    {
        REQUIRE(alignTo(0, 0) == 0);
        REQUIRE(alignTo(0, 1) == 0);
        REQUIRE(alignTo(0, 2) == 0);

        REQUIRE(alignTo(reinterpret_cast<void*>(1), 1) == reinterpret_cast<void*>(1));
        REQUIRE(alignTo(reinterpret_cast<void*>(1), 2) == reinterpret_cast<void*>(2));
        REQUIRE(alignTo(reinterpret_cast<void*>(1), 16) == reinterpret_cast<void*>(16));

        REQUIRE(alignTo(reinterpret_cast<void*>(0), 8) == reinterpret_cast<void*>(0));
        REQUIRE(alignTo(reinterpret_cast<void*>(1), 8) == reinterpret_cast<void*>(8));
        REQUIRE(alignTo(reinterpret_cast<void*>(6), 8) == reinterpret_cast<void*>(8));
        REQUIRE(alignTo(reinterpret_cast<void*>(7), 8) == reinterpret_cast<void*>(8));
        REQUIRE(alignTo(reinterpret_cast<void*>(8), 8) == reinterpret_cast<void*>(8));
        REQUIRE(alignTo(reinterpret_cast<void*>(9), 8) == reinterpret_cast<void*>(16));
        REQUIRE(alignTo(reinterpret_cast<void*>(15), 8) == reinterpret_cast<void*>(16));
        REQUIRE(alignTo(reinterpret_cast<void*>(16), 8) == reinterpret_cast<void*>(16));
        REQUIRE(alignTo(reinterpret_cast<void*>(17), 8) == reinterpret_cast<void*>(24));

        REQUIRE(alignTo(reinterpret_cast<void*>(8), 1) == reinterpret_cast<void*>(8));
        REQUIRE(alignTo(reinterpret_cast<void*>(8), 4) == reinterpret_cast<void*>(8));
        REQUIRE(alignTo(reinterpret_cast<void*>(8), 8) == reinterpret_cast<void*>(8));
        REQUIRE(alignTo(reinterpret_cast<void*>(16), 4) == reinterpret_cast<void*>(16));
        REQUIRE(alignTo(reinterpret_cast<void*>(16), 8) == reinterpret_cast<void*>(16));
        REQUIRE(alignTo(reinterpret_cast<void*>(16), 16) == reinterpret_cast<void*>(16));
        REQUIRE(alignTo(reinterpret_cast<void*>(16), 64) == reinterpret_cast<void*>(64));
    }
}
