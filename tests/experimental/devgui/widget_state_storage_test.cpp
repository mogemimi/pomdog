// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/devgui/widget_state_storage.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <map>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::u32;
using pomdog::u64;
using pomdog::devgui::WidgetID;
using pomdog::devgui::WidgetStateStorage;

TEST_CASE("devgui::WidgetStateStorage")
{
    WidgetStateStorage storage;
    storage.initialize(4);

    SUBCASE("stores and finds values")
    {
        REQUIRE_FALSE(storage.find(WidgetID{10}).has_value());
        storage.store(WidgetID{10}, 100);
        storage.store(WidgetID{20}, 200);
        REQUIRE(storage.find(WidgetID{10}) == u64{100});
        REQUIRE(storage.find(WidgetID{20}) == u64{200});
        REQUIRE(storage.count() == 2);
    }
    SUBCASE("overwrites an existing value")
    {
        storage.store(WidgetID{10}, 100);
        storage.store(WidgetID{10}, 101);
        REQUIRE(storage.find(WidgetID{10}) == u64{101});
        REQUIRE(storage.count() == 1);
    }
    SUBCASE("removes values")
    {
        storage.store(WidgetID{10}, 100);
        storage.remove(WidgetID{10});
        REQUIRE_FALSE(storage.find(WidgetID{10}).has_value());
        REQUIRE(storage.count() == 0);
    }
    SUBCASE("evicts the least recently used value when full")
    {
        storage.store(WidgetID{1}, 1);
        storage.store(WidgetID{2}, 2);
        storage.store(WidgetID{3}, 3);
        storage.store(WidgetID{4}, 4);

        // NOTE: Reading 1 makes 2 the least recently used value.
        REQUIRE(storage.find(WidgetID{1}) == u64{1});
        storage.store(WidgetID{5}, 5);

        REQUIRE(storage.count() == 4);
        REQUIRE_FALSE(storage.find(WidgetID{2}).has_value());
        REQUIRE(storage.find(WidgetID{1}) == u64{1});
        REQUIRE(storage.find(WidgetID{3}) == u64{3});
        REQUIRE(storage.find(WidgetID{4}) == u64{4});
        REQUIRE(storage.find(WidgetID{5}) == u64{5});
    }
    SUBCASE("clear removes everything")
    {
        storage.store(WidgetID{1}, 1);
        storage.clear();
        REQUIRE(storage.count() == 0);
        REQUIRE_FALSE(storage.find(WidgetID{1}).has_value());
    }
}

TEST_CASE("devgui::WidgetStateStorage matches a reference map")
{
    // NOTE: IDs that share low bits collide in the hash table, which exercises
    // probing and the backward-shift deletion.
    constexpr u32 capacity = 64;
    WidgetStateStorage storage;
    storage.initialize(capacity);
    std::map<u64, u64> reference;

    u64 seed = 12345;
    const auto next = [&seed]() -> u64 {
        seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
        return seed >> 33;
    };

    for (int step = 0; step < 20000; ++step) {
        const auto id = WidgetID{((next() % 48) << 16) + 1};
        const auto action = next() % 3;
        if (action == 0) {
            storage.remove(id);
            reference.erase(id.value);
        }
        else {
            const auto value = next();
            storage.store(id, value);
            reference[id.value] = value;
        }
        // NOTE: At most 48 distinct IDs exist, so nothing is evicted.
        REQUIRE(storage.count() == reference.size());
    }
    for (u64 k = 0; k < 48; ++k) {
        const auto id = WidgetID{(k << 16) + 1};
        const auto found = storage.find(id);
        const auto expected = reference.find(id.value);
        if (expected == reference.end()) {
            REQUIRE_FALSE(found.has_value());
        }
        else {
            REQUIRE(found == expected->second);
        }
    }
}
