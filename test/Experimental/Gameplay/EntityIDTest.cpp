// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay/EntityID.hpp"
#include "catch.hpp"
#include <algorithm>
#include <cstdint>

using Pomdog::EntityID;

TEST_CASE("EntityID", "[EntityID]")
{
    SECTION("trivial case")
    {
        EntityID id(1234, 5678);
        REQUIRE(id.SequenceNumber() == 1234);
        REQUIRE(id.Index() == 5678);
    }
    SECTION("zero")
    {
        EntityID id(0, 0);
        REQUIRE(id.SequenceNumber() == 0);
        REQUIRE(id.Index() == 0);
    }
    SECTION("uint32 max")
    {
        EntityID id(std::numeric_limits<std::uint32_t>::max(),
            std::numeric_limits<std::uint32_t>::max());
        REQUIRE(id.SequenceNumber() == std::numeric_limits<std::uint32_t>::max());
        REQUIRE(id.Index() == std::numeric_limits<std::uint32_t>::max());
    }
    SECTION("uint32")
    {
        EntityID id(std::numeric_limits<std::uint32_t>::max() - 1,
            std::numeric_limits<std::uint32_t>::max() - 2);
        REQUIRE(id.SequenceNumber() == std::numeric_limits<std::uint32_t>::max() - 1);
        REQUIRE(id.Index() == std::numeric_limits<std::uint32_t>::max() - 2);
    }
}
