// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/devgui/key.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::devgui::combineID;
using pomdog::devgui::Key;
using pomdog::devgui::WidgetID;
using namespace pomdog::devgui::literals;

namespace {

// NOTE: Keys and IDs are compile-time values; a static_assert proves that no
// string is hashed at run time.
constexpr Key speedKey = "speed"_key;
static_assert(speedKey.name == pomdog::hash::xxh32("speed", 5, pomdog::devgui::detail::keyNameSeed));
static_assert(speedKey.index == 0);
static_assert("speed"_key.at(7).index == 7);
static_assert("speed"_key.at(7).name == speedKey.name);
static_assert(combineID(WidgetID{1}, speedKey) == combineID(WidgetID{1}, speedKey));

} // namespace

TEST_CASE("devgui::Key")
{
    SUBCASE("names come from the literal")
    {
        REQUIRE("speed"_key == speedKey);
        REQUIRE("speed"_key != "rate"_key);
    }
    SUBCASE("at sets the index and keeps the name")
    {
        const auto row = "row"_key.at(42);
        REQUIRE(row.name == "row"_key.name);
        REQUIRE(row.index == 42);
        REQUIRE(row != "row"_key.at(43));
    }
}

TEST_CASE("devgui::combineID")
{
    const auto parent = WidgetID{0x1234};

    SUBCASE("the parent changes the ID")
    {
        REQUIRE(combineID(parent, speedKey) != combineID(WidgetID{0x1235}, speedKey));
    }
    SUBCASE("the index changes the ID")
    {
        REQUIRE(combineID(parent, speedKey.at(1)) != combineID(parent, speedKey.at(2)));
    }
    SUBCASE("name and index are not interchangeable")
    {
        REQUIRE(combineID(parent, Key{1, 2}) != combineID(parent, Key{2, 1}));
    }
    SUBCASE("nesting order matters")
    {
        const auto a = "a"_key;
        const auto b = "b"_key;
        REQUIRE(combineID(combineID(parent, a), b) != combineID(combineID(parent, b), a));
    }
    SUBCASE("list rows get distinct IDs")
    {
        const auto first = combineID(parent, "row"_key.at(0));
        for (pomdog::u32 i = 1; i < 1000; ++i) {
            REQUIRE(combineID(parent, "row"_key.at(i)) != first);
        }
    }
}
