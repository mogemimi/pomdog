// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/ecs/entity_lru_queue.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace {
using pomdog::u32;
using pomdog::ecs::EntityLRUQueue;
} // namespace

TEST_CASE("EntityLRUQueue")
{
    EntityLRUQueue queue;
    const u32 capacity = 5;

    SUBCASE("initialization and capacity")
    {
        queue.initialize(capacity);
        REQUIRE(queue.capacity() == capacity);
        REQUIRE(queue.count() == 0);
        REQUIRE(queue.full() == false);

        REQUIRE(queue.cached(0) == false);
        REQUIRE(queue.cached(1) == false);
        REQUIRE(queue.cached(2) == false);
        REQUIRE(queue.cached(3) == false);
        REQUIRE(queue.cached(4) == false);
    }
    SUBCASE("put and cached")
    {
        queue.initialize(capacity);
        queue.put(1);
        queue.put(2);
        queue.put(3);

        REQUIRE(queue.cached(0) == false);
        REQUIRE(queue.cached(1) == true);
        REQUIRE(queue.cached(2) == true);
        REQUIRE(queue.cached(3) == true);
        REQUIRE(queue.cached(4) == false);
        REQUIRE(queue.count() == 3);
    }
    SUBCASE("remove")
    {
        queue.initialize(capacity);
        queue.put(2);
        queue.put(3);
        queue.put(4);

        queue.remove(3);
        REQUIRE(queue.cached(3) == false);
        REQUIRE(queue.count() == 2);

        queue.remove(2);
        REQUIRE(queue.cached(2) == false);
        REQUIRE(queue.count() == 1);

        queue.remove(4);
        REQUIRE(queue.cached(4) == false);
        REQUIRE(queue.count() == 0);
    }
    SUBCASE("clear")
    {
        queue.initialize(capacity);
        queue.put(1);
        queue.put(2);
        queue.put(3);

        queue.clear();
        REQUIRE(queue.count() == 0);
        REQUIRE(queue.cached(1) == false);
        REQUIRE(queue.cached(2) == false);
        REQUIRE(queue.cached(3) == false);
    }
    SUBCASE("oldest")
    {
        queue.initialize(capacity);
        queue.put(1);
        queue.put(2);
        queue.put(3);
        queue.put(4);

        auto oldest = queue.oldest();
        REQUIRE(oldest.has_value());
        REQUIRE(oldest.value() == 1);

        queue.remove(1);
        oldest = queue.oldest();
        REQUIRE(oldest.has_value());
        REQUIRE(oldest.value() == 2);

        queue.put(2);
        oldest = queue.oldest();
        REQUIRE(oldest.has_value());
        REQUIRE(oldest.value() == 3);

        queue.put(3);
        oldest = queue.oldest();
        REQUIRE(oldest.has_value());
        REQUIRE(oldest.value() == 4);
    }
    SUBCASE("full and capacity")
    {
        queue.initialize(capacity);
        queue.put(0);
        queue.put(1);
        queue.put(2);
        queue.put(3);
        queue.put(4);

        REQUIRE(queue.full() == true);
        REQUIRE(queue.count() == capacity);
    }
    SUBCASE("forEachLatest")
    {
        queue.initialize(capacity);
        queue.put(1);
        queue.put(2);
        queue.put(3);

        std::vector<u32> result;
        queue.forEachLatest([&result](u32 index) {
            result.push_back(index);
        });

        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == 3);
        REQUIRE(result[1] == 2);
        REQUIRE(result[2] == 1);
    }
    SUBCASE("forEachOldest")
    {
        queue.initialize(capacity);
        queue.put(1);
        queue.put(2);
        queue.put(3);

        std::vector<u32> result;
        queue.forEachOldest([&result](u32 index) {
            result.push_back(index);
        });

        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == 1);
        REQUIRE(result[1] == 2);
        REQUIRE(result[2] == 3);
    }
}

TEST_CASE("EntityLRUQueue::findLatest and findOldest")
{
    EntityLRUQueue lru;
    lru.initialize(10);

    for (u32 i = 0; i < 5; i++) {
        lru.put(i);
    }

    SUBCASE("findLatest finds newest matching index")
    {
        auto result = lru.findLatest([](u32 index) {
            return index >= 2;
        });
        REQUIRE(result.has_value());
        CHECK(result.value() == 4);
    }
    SUBCASE("findOldest finds oldest matching index")
    {
        auto result = lru.findOldest([](u32 index) {
            return index >= 2;
        });
        REQUIRE(result.has_value());
        CHECK(result.value() == 2);
    }
    SUBCASE("findLatest returns nullopt if no match")
    {
        auto result = lru.findLatest([](u32 index) {
            return index > 100;
        });
        CHECK_FALSE(result.has_value());
    }
    SUBCASE("findOldest returns nullopt if no match")
    {
        auto result = lru.findOldest([](u32 index) {
            return index > 100;
        });
        CHECK_FALSE(result.has_value());
    }
    SUBCASE("findLatest respects touch/move to front")
    {
        lru.put(2);

        auto result = lru.findLatest([](u32 index) {
            return index == 2;
        });
        REQUIRE(result.has_value());
        CHECK(result.value() == 2);
    }
    SUBCASE("findOldest skips removed entries")
    {
        lru.remove(2);

        auto result = lru.findOldest([](u32 index) {
            return index >= 2;
        });

        REQUIRE(result.has_value());
        CHECK(result.value() == 3);
    }
    SUBCASE("forEachLatest with removal during iteration")
    {
        CHECK(lru.cached(2) == true);
        CHECK(lru.count() == 5);
        std::vector<u32> visited;
        lru.forEachLatest([&lru, &visited](u32 index) {
            visited.push_back(index);
            if (index == 2) {
                lru.remove(2);
            }
        });
        CHECK(visited == std::vector<u32>{4, 3, 2, 1, 0});
        CHECK(lru.cached(2) == false);
        CHECK(lru.count() == 4);
    }
    SUBCASE("forEachLatest with removal of non-matching entry during iteration")
    {
        CHECK(lru.cached(1) == true);
        CHECK(lru.count() == 5);
        std::vector<u32> visited;
        lru.forEachLatest([&lru, &visited](u32 index) {
            visited.push_back(index);
            if (index == 2) {
                lru.remove(1);
            }
        });
        CHECK(visited == std::vector<u32>{4, 3, 2, 0});
        CHECK(lru.cached(1) == false);
        CHECK(lru.count() == 4);
    }
    SUBCASE("forEachOldest with removal during iteration")
    {
        CHECK(lru.cached(2) == true);
        CHECK(lru.count() == 5);
        std::vector<u32> visited;
        lru.forEachOldest([&lru, &visited](u32 index) {
            visited.push_back(index);
            if (index == 2) {
                lru.remove(2);
            }
        });
        CHECK(visited == std::vector<u32>{0, 1, 2, 3, 4});
        CHECK(lru.cached(2) == false);
        CHECK(lru.count() == 4);
    }
    SUBCASE("forEachOldest with removal of non-matching entry during iteration")
    {
        CHECK(lru.cached(3) == true);
        CHECK(lru.count() == 5);
        std::vector<u32> visited;
        lru.forEachOldest([&lru, &visited](u32 index) {
            visited.push_back(index);
            if (index == 2) {
                lru.remove(3);
            }
        });
        CHECK(visited == std::vector<u32>{0, 1, 2, 4});
        CHECK(lru.cached(3) == false);
        CHECK(lru.count() == 4);
    }
    SUBCASE("findLatest with removal during iteration")
    {
        CHECK(lru.cached(2) == true);
        CHECK(lru.count() == 5);
        auto result = lru.findLatest([&lru](u32 index) {
            if (index == 2) {
                lru.remove(2);
            }
            return index == 1;
        });
        REQUIRE(result.has_value());
        CHECK(result.value() == 1);
        CHECK(lru.cached(2) == false);
        CHECK(lru.count() == 4);
    }
    SUBCASE("findLatest with removal of non-matching entry during iteration")
    {
        CHECK(lru.cached(1) == true);
        CHECK(lru.count() == 5);
        auto result = lru.findLatest([&lru](u32 index) {
            if (index == 2) {
                lru.remove(1);
            }
            return index == 0;
        });
        REQUIRE(result.has_value());
        CHECK(result.value() == 0);
        CHECK(lru.cached(1) == false);
        CHECK(lru.count() == 4);
    }
    SUBCASE("findOldest with removal during iteration")
    {
        CHECK(lru.cached(2) == true);
        CHECK(lru.count() == 5);
        auto result = lru.findOldest([&lru](u32 index) {
            if (index == 2) {
                lru.remove(2);
            }
            return index == 3;
        });
        REQUIRE(result.has_value());
        CHECK(result.value() == 3);
        CHECK(lru.cached(2) == false);
        CHECK(lru.count() == 4);
    }
    SUBCASE("findOldest with removal of non-matching entry during iteration")
    {
        CHECK(lru.cached(3) == true);
        CHECK(lru.count() == 5);
        auto result = lru.findOldest([&lru](u32 index) {
            if (index == 2) {
                lru.remove(3);
            }
            return index == 4;
        });
        REQUIRE(result.has_value());
        CHECK(result.value() == 4);
        CHECK(lru.cached(3) == false);
        CHECK(lru.count() == 4);
    }
}
