// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/ecs/entity.hpp"
#include <catch_amalgamated.hpp>
#include <algorithm>
#include <cstdint>

using pomdog::ecs::Entity;

TEST_CASE("EntityID", "[EntityID]")
{
    SECTION("trivial case")
    {
        Entity entity(1234, 5678);
        REQUIRE(entity.GetVersion() == 1234);
        REQUIRE(entity.GetIndex() == 5678);
    }
    SECTION("zero")
    {
        Entity entity(0, 0);
        REQUIRE(entity.GetVersion() == 0);
        REQUIRE(entity.GetIndex() == 0);
    }
    SECTION("uint32 max")
    {
        Entity entity(std::numeric_limits<std::uint32_t>::max(),
            std::numeric_limits<std::uint32_t>::max());
        REQUIRE(entity.GetVersion() == std::numeric_limits<std::uint32_t>::max());
        REQUIRE(entity.GetIndex() == std::numeric_limits<std::uint32_t>::max());
    }
    SECTION("uint32")
    {
        Entity entity(std::numeric_limits<std::uint32_t>::max() - 1,
            std::numeric_limits<std::uint32_t>::max() - 2);
        REQUIRE(entity.GetVersion() == std::numeric_limits<std::uint32_t>::max() - 1);
        REQUIRE(entity.GetIndex() == std::numeric_limits<std::uint32_t>::max() - 2);
    }
}
