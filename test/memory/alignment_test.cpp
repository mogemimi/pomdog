// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/memory/alignment.h"
#include <catch_amalgamated.hpp>
#include <array>
#include <cstdint>
#include <string>

using namespace pomdog;

TEST_CASE("alignment", "[memory]")
{
    using pomdog::detail::AlignTo;
    using pomdog::detail::IsPowerOfTwo;

    SECTION("IsPowerOfTwo")
    {
        REQUIRE(IsPowerOfTwo(1));
        REQUIRE(IsPowerOfTwo(2));
        REQUIRE(IsPowerOfTwo(4));
        REQUIRE(IsPowerOfTwo(8));
        REQUIRE(IsPowerOfTwo(16));
        REQUIRE(IsPowerOfTwo(32));
        REQUIRE(IsPowerOfTwo(64));
        REQUIRE(IsPowerOfTwo(256));
        REQUIRE(IsPowerOfTwo(512));
        REQUIRE(IsPowerOfTwo(1024));

        REQUIRE(!IsPowerOfTwo(0));
        REQUIRE(!IsPowerOfTwo(3));
        REQUIRE(!IsPowerOfTwo(5));
        REQUIRE(!IsPowerOfTwo(6));
        REQUIRE(!IsPowerOfTwo(7));
        REQUIRE(!IsPowerOfTwo(9));
        REQUIRE(!IsPowerOfTwo(10));
        REQUIRE(!IsPowerOfTwo(11));
    }
    SECTION("AlignTo")
    {
        REQUIRE(AlignTo(0, 0) == 0);
        REQUIRE(AlignTo(0, 1) == 0);
        REQUIRE(AlignTo(0, 2) == 0);

        REQUIRE(AlignTo(reinterpret_cast<void*>(1), 1) == reinterpret_cast<void*>(1));
        REQUIRE(AlignTo(reinterpret_cast<void*>(1), 2) == reinterpret_cast<void*>(2));
        REQUIRE(AlignTo(reinterpret_cast<void*>(1), 16) == reinterpret_cast<void*>(16));

        REQUIRE(AlignTo(reinterpret_cast<void*>(0), 8) == reinterpret_cast<void*>(0));
        REQUIRE(AlignTo(reinterpret_cast<void*>(1), 8) == reinterpret_cast<void*>(8));
        REQUIRE(AlignTo(reinterpret_cast<void*>(6), 8) == reinterpret_cast<void*>(8));
        REQUIRE(AlignTo(reinterpret_cast<void*>(7), 8) == reinterpret_cast<void*>(8));
        REQUIRE(AlignTo(reinterpret_cast<void*>(8), 8) == reinterpret_cast<void*>(8));
        REQUIRE(AlignTo(reinterpret_cast<void*>(9), 8) == reinterpret_cast<void*>(16));
        REQUIRE(AlignTo(reinterpret_cast<void*>(15), 8) == reinterpret_cast<void*>(16));
        REQUIRE(AlignTo(reinterpret_cast<void*>(16), 8) == reinterpret_cast<void*>(16));
        REQUIRE(AlignTo(reinterpret_cast<void*>(17), 8) == reinterpret_cast<void*>(24));

        REQUIRE(AlignTo(reinterpret_cast<void*>(8), 1) == reinterpret_cast<void*>(8));
        REQUIRE(AlignTo(reinterpret_cast<void*>(8), 4) == reinterpret_cast<void*>(8));
        REQUIRE(AlignTo(reinterpret_cast<void*>(8), 8) == reinterpret_cast<void*>(8));
        REQUIRE(AlignTo(reinterpret_cast<void*>(16), 4) == reinterpret_cast<void*>(16));
        REQUIRE(AlignTo(reinterpret_cast<void*>(16), 8) == reinterpret_cast<void*>(16));
        REQUIRE(AlignTo(reinterpret_cast<void*>(16), 16) == reinterpret_cast<void*>(16));
        REQUIRE(AlignTo(reinterpret_cast<void*>(16), 64) == reinterpret_cast<void*>(64));
    }
}
