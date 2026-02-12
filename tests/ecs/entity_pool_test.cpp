// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/ecs/entity.h"
#include "pomdog/ecs/entity_pool.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace {
using pomdog::u16;
using pomdog::u32;
using pomdog::ecs::Entity;
using pomdog::ecs::EntityPool;
} // namespace

TEST_CASE("EntityPool")
{
    SUBCASE("empty case")
    {
        EntityPool pool;
        REQUIRE(pool.count() == 0);
        REQUIRE(pool.capacity() == 0);
    }
    SUBCASE("first case")
    {
        EntityPool pool;
        pool.initialize(42);
        REQUIRE(pool.count() == 0);
        REQUIRE(pool.capacity() == 42);

        auto e = pool.create();
        REQUIRE(e == Entity{0, 1});
        REQUIRE(pool.count() == 1);
        REQUIRE(pool.capacity() == 42);

        pool.destroy(e);
        REQUIRE(pool.count() == 0);
        REQUIRE(pool.capacity() == 42);

        e = pool.create();
        REQUIRE(e == Entity{0, 2});
        REQUIRE(pool.count() == 1);
        REQUIRE(pool.capacity() == 42);
    }
    SUBCASE("single case")
    {
        EntityPool pool;
        pool.initialize(1);
        REQUIRE(pool.count() == 0);
        REQUIRE(pool.capacity() == 1);

        auto e = pool.create();
        REQUIRE(pool.count() == 1);
        REQUIRE(pool.capacity() == 1);

        pool.destroy(e);
        REQUIRE(pool.count() == 0);
        REQUIRE(pool.capacity() == 1);
    }
    SUBCASE("multiple case")
    {
        EntityPool pool;
        pool.initialize(1000);
        REQUIRE(pool.count() == 0);
        REQUIRE(pool.capacity() == 1000);

        for (u16 i = 0; i < 1000; i++) {
            auto e = pool.create();
            REQUIRE(e == Entity{i, 1});
            REQUIRE(pool.count() == (i + 1));
            REQUIRE(pool.capacity() == 1000);
        }

        for (u16 i = 0; i < 1000; i++) {
            pool.destroy(Entity{i, 1});
            REQUIRE(pool.count() == (1000 - 1 - i));
            REQUIRE(pool.capacity() == 1000);
        }
    }
    SUBCASE("version")
    {
        std::initializer_list<u32> args = {1, 2, 3, 63, 64, 65, 100};

        constexpr u16 maxUint14 = 0x3FFFul;

        for (const auto arg : args) {
            EntityPool pool;
            pool.initialize(arg);

            REQUIRE(pool.count() == 0);
            REQUIRE(pool.capacity() == arg);

            for (u16 version = 1; version <= maxUint14; version++) {
                for (u32 i = 0; i < pool.capacity(); i++) {
                    auto e = pool.create();

                    REQUIRE(e.index() == i);
                    REQUIRE(e.version() == version);

                    REQUIRE(pool.count() == (i + 1));
                    REQUIRE(pool.capacity() == arg);
                }

                pool.forEach([&](Entity e) {
                    REQUIRE(pool.exists(e));
                    pool.destroy(e);
                    REQUIRE(!pool.exists(e));
                });

                REQUIRE(pool.count() == 0);
                REQUIRE(pool.capacity() == arg);
            }

            for (u16 version = 1; version <= maxUint14; version++) {
                for (u32 i = 0; i < pool.capacity(); i++) {
                    auto e = pool.create();

                    REQUIRE(e.index() == i);
                    REQUIRE(e.version() == version);

                    REQUIRE(pool.count() == (i + 1));
                    REQUIRE(pool.capacity() == arg);
                }

                pool.forEach([&](Entity e) {
                    REQUIRE(pool.exists(e));
                    pool.destroy(e);
                    REQUIRE(!pool.exists(e));
                });

                REQUIRE(pool.count() == 0);
                REQUIRE(pool.capacity() == arg);
            }

            for (u32 i = 0; i < pool.capacity(); i++) {
                auto e = pool.create();

                REQUIRE(e.index() == i);
                REQUIRE(e.version() == 1);

                REQUIRE(pool.count() == (i + 1));
                REQUIRE(pool.capacity() == arg);
            }

            pool.destroyAll();

            REQUIRE(pool.count() == 0);
            REQUIRE(pool.capacity() == arg);

            for (u32 i = 0; i < pool.capacity(); i++) {
                auto e = pool.create();

                REQUIRE(e.index() == i);
                REQUIRE(e.version() == 1);

                REQUIRE(pool.count() == (i + 1));
                REQUIRE(pool.capacity() == arg);
            }

            pool.destroyAll();

            REQUIRE(pool.count() == 0);
            REQUIRE(pool.capacity() == arg);
        }
    }
    SUBCASE("index")
    {
        std::initializer_list<u32> args = {1, 2, 3, 63, 64, 65, 127, 128, 129, 200};

        for (const auto arg : args) {
            EntityPool pool;
            pool.initialize(arg);

            REQUIRE(pool.count() == 0);
            REQUIRE(pool.capacity() == arg);

            for (u32 i = 0; i < pool.capacity(); i++) {
                auto e = pool.create();

                REQUIRE(e.index() == i);
                REQUIRE(e.version() == 1);

                REQUIRE(pool.count() == (i + 1));
                REQUIRE(pool.capacity() == arg);
            }

            pool.forEach([&](Entity e) {
                REQUIRE(pool.exists(e));
                pool.destroy(e);
                REQUIRE(!pool.exists(e));
            });

            REQUIRE(pool.count() == 0);
            REQUIRE(pool.capacity() == arg);

            for (u32 i = 0; i < pool.capacity(); i++) {
                auto e = pool.create();

                REQUIRE(e.index() == i);
                REQUIRE(e.version() == 2);

                REQUIRE(pool.count() == (i + 1));
                REQUIRE(pool.capacity() == arg);
            }

            pool.destroyAll();

            REQUIRE(pool.count() == 0);
            REQUIRE(pool.capacity() == arg);
        }
    }
    SUBCASE("forEachReverse skips destroyed entities")
    {
        EntityPool pool;
        pool.initialize(10);

        // NOTE: Create some entities
        std::vector<Entity> created;
        for (int i = 0; i < 5; ++i) {
            created.push_back(pool.create());
        }

        pool.destroy(created[1]);
        pool.destroy(created[4]);

        std::vector<Entity> visited;
        pool.forEachReverse([&](Entity e) {
            visited.push_back(e);
        });

        REQUIRE(visited.end() == std::find(visited.begin(), visited.end(), created[1]));
        REQUIRE(visited.end() == std::find(visited.begin(), visited.end(), created[4]));

        // NOTE: Ensure order is still reversed for the remaining
        std::vector<Entity> expected = {created[3], created[2], created[0]};
        REQUIRE(visited == expected);
    }
}

TEST_CASE("EntityPool::forEach and forEachReverse")
{
    EntityPool pool;
    pool.initialize(100);

    // NOTE: Create some entities
    std::vector<Entity> created;
    for (int i = 0; i < 90; i++) {
        created.push_back(pool.create());
    }

    SUBCASE("forEach iterates in creation order")
    {
        std::vector<Entity> visited;
        pool.forEach([&](Entity e) {
            visited.push_back(e);
        });

        REQUIRE(visited.size() == created.size());

        // NOTE: must match creation order
        REQUIRE(visited == created);
    }

    SUBCASE("forEachReverse iterates in reverse creation order")
    {
        std::vector<Entity> visited;
        pool.forEachReverse([&](Entity e) {
            visited.push_back(e);
        });

        std::vector<Entity> expected = created;
        std::reverse(expected.begin(), expected.end());

        REQUIRE(visited.size() == created.size());

        // NOTE: must be reverse of creation order
        REQUIRE(visited == expected);
    }

    SUBCASE("forEach skips destroyed entities")
    {
        // NOTE: Destroy one entity
        pool.destroy(created[2]);

        std::vector<Entity> visited;
        pool.forEach([&](Entity e) {
            visited.push_back(e);
        });

        REQUIRE(visited.end() == std::find(visited.begin(), visited.end(), created[2]));
    }
}
