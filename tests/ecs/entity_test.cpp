// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/ecs/entity.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace {
using pomdog::u16;
using pomdog::u32;
using pomdog::ecs::Entity;
} // namespace

TEST_CASE("Entity")
{
    SUBCASE("default")
    {
        Entity e;
        REQUIRE(e.index() == 0);
        REQUIRE(e.version() == 0);
        REQUIRE(e.uint32() == 0);
    }
    SUBCASE("constructor")
    {
        Entity e{42, 100};
        REQUIRE(e.index() == 42);
        REQUIRE(e.version() == 100);
        REQUIRE(e.uint32() == (u32(100) << 18 | u32(42)));
    }
    SUBCASE("null")
    {
        Entity e = pomdog::ecs::null();
        REQUIRE(e.index() == 0);
        REQUIRE(e.version() == 0);
        REQUIRE(e.uint32() == 0);
    }
    SUBCASE("256 edge case")
    {
        for (u16 i = 250; i < 260; i++) {
            Entity e{i, 12345};
            REQUIRE(e.index() == i);
            REQUIRE(e.version() == 12345);
            REQUIRE(e.uint32() == (u32(12345) << 18 | u32(i)));
        }
    }
    SUBCASE("0x3FFFF edge case")
    {
        Entity e{u32(0x3FFFFul), 12345};
        REQUIRE(e.index() == u32(0x3FFFFul));
        REQUIRE(e.version() == 12345);
        REQUIRE(e.uint32() == (u32(12345) << 18 | u32(0x3FFFFul)));
    }
    SUBCASE("0x3FFF edge")
    {
        Entity e{12345, u16(0x3FFFul)};
        REQUIRE(e.index() == 12345);
        REQUIRE(e.version() == u16(0x3FFFul));
        REQUIRE(e.uint32() == (u32(0x3FFFul) << 18 | u32(12345)));
    }
}
