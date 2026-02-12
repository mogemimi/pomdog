// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/ecs/entity.h"
#include "pomdog/ecs/entity_bin.h"
#include "pomdog/ecs/entity_pool.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace {
using pomdog::u16;
using pomdog::u32;
using pomdog::ecs::Entity;
using pomdog::ecs::EntityBinning;
using pomdog::ecs::EntityPool;
} // namespace

TEST_CASE("EntityBinning")
{
    SUBCASE("default")
    {
        EntityPool pool;
        pool.initialize(42);

        EntityBinning binning;
        binning.initialize(5, pool.capacity());

        REQUIRE(binning.getBinCount() == 5);
        REQUIRE(binning.getEntitiesCapacity() == pool.capacity());

        for (u32 i = 0; i < pool.capacity(); i++) {
            auto e = pool.create();

            REQUIRE(binning.isEntityIndexEmpty(e.index()));

            const auto binIndex = i % 5;
            binning.append(binIndex, e);
            REQUIRE(!binning.isEntityIndexEmpty(e.index()));
        }

        {
            u32 count1 = 0;
            u32 count2 = 0;
            for (u32 binIndex = 0; binIndex < 5; binIndex++) {
                count1 += binning.getEntitiesCountPerBin(binIndex);

                binning.forEach(binIndex, [&](Entity e) {
                    REQUIRE(e.index() % 5 == binIndex);
                    REQUIRE(!binning.isEntityIndexEmpty(e.index()));
                    count2 += 1;
                });
            }
            REQUIRE(count1 == pool.count());
            REQUIRE(count2 == pool.count());
        }
        {
            u32 count1 = 0;
            u32 count2 = 0;
            for (u32 binIndex = 0; binIndex < 5; binIndex++) {
                count1 += binning.getEntitiesCountPerBin(binIndex);

                binning.forEach(binIndex, [&](Entity e) {
                    count2 += 1;
                    binning.remove(e);
                    REQUIRE(binning.isEntityIndexEmpty(e.index()));
                });
            }
            REQUIRE(count1 == pool.count());
            REQUIRE(count2 == pool.count());
        }
    }
}

TEST_CASE("EntityBinning::forEach and findIf")
{
    EntityBinning binning;
    binning.initialize(1, 10);

    constexpr u32 binIndex = 0;
    constexpr u32 entityCount = 5;
    constexpr u32 entityVersion = 42;

    for (u32 i = 0; i < entityCount; i++) {
        // NOTE: Append entities with indices 4, 3, 2, 1, 0
        binning.append(binIndex, Entity{(entityCount - i) - 1, entityVersion});
    }

    SUBCASE("forEach")
    {
        std::vector<u32> visited;
        binning.forEach(binIndex, [&](Entity e) {
            visited.push_back(e.index());
        });
        CHECK(visited == std::vector<u32>{0, 1, 2, 3, 4});
    }
    SUBCASE("forEach with removal during iteration")
    {
        std::vector<u32> visited;
        binning.forEach(binIndex, [&](Entity e) {
            visited.push_back(e.index());
            binning.remove(e);
            REQUIRE(binning.isEntityIndexEmpty(e.index()));
        });
        CHECK(visited == std::vector<u32>{0, 1, 2, 3, 4});
    }
    SUBCASE("forEach with removal of non-matching entry during iteration")
    {
        std::vector<u32> visited;
        binning.forEach(binIndex, [&](Entity e) {
            visited.push_back(e.index());
            if (e.index() == 2) {
                binning.remove(Entity{3, entityVersion});
                REQUIRE(binning.isEntityIndexEmpty(3));
            }
        });
        CHECK(visited == std::vector<u32>{0, 1, 2, 4});
    }
    SUBCASE("findIf")
    {
        for (u32 i = 0; i < entityCount; i++) {
            auto result = binning.findIf(binIndex, [&](Entity e) {
                return e.index() == i;
            });
            REQUIRE(result != pomdog::ecs::null());
            REQUIRE(result.index() == i);
        }
    }
    SUBCASE("findIf with removal during iteration")
    {
        std::vector<u32> visited;

        auto result = binning.findIf(binIndex, [&](Entity e) {
            visited.push_back(e.index());
            if (e.index() == 2) {
                binning.remove(e);
                return false;
            }
            return e.index() == 4;
        });

        CHECK(visited == std::vector<u32>{0, 1, 2, 3, 4});
        REQUIRE(result != pomdog::ecs::null());
        REQUIRE(binning.getEntitiesCountPerBin(binIndex) == entityCount - 1);
        REQUIRE(result.index() == 4);
    }
    SUBCASE("findIf with removal of non-matching entry during iteration")
    {
        std::vector<u32> visited;

        auto result = binning.findIf(binIndex, [&](Entity e) {
            visited.push_back(e.index());
            if (e.index() == 2) {
                binning.remove(Entity{3, entityVersion});
                return false;
            }
            return e.index() == 4;
        });

        CHECK(visited == std::vector<u32>{0, 1, 2, 4});
        REQUIRE(result != pomdog::ecs::null());
        REQUIRE(binning.getEntitiesCountPerBin(binIndex) == entityCount - 1);
        REQUIRE(result.index() == 4);
    }
}
