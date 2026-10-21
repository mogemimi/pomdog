// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/text_input/composition_buffer.h"
#include "pomdog/experimental/text_input/text_input_event.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::u8;
using pomdog::textinput::TextInputEvent;
using pomdog::textinput::detail::CompositionBuffer;

namespace {

[[nodiscard]] TextInputEvent
toEvent(const CompositionBuffer& buffer)
{
    TextInputEvent event;
    buffer.fillEvent(event);
    return event;
}

} // namespace

TEST_CASE("CompositionBuffer")
{
    // NOTE: MSVC warns (C4866) when a REQUIRE argument is a temporary string,
    // so each event is named first.
    CompositionBuffer buffer;

    SUBCASE("starts empty")
    {
        REQUIRE(buffer.getLength() == 0);
        REQUIRE(buffer.getCaret() == 0);
        const auto event = toEvent(buffer);
        REQUIRE(event.text.empty());
        REQUIRE(event.caret == 0);
        REQUIRE(event.targetStart == 0);
        REQUIRE(event.targetEnd == 0);
    }
    SUBCASE("inserts, replaces, and deletes characters")
    {
        buffer.replace(0, 0, U"ac", {});
        buffer.replace(1, 0, U"b", {});
        REQUIRE(buffer.getLength() == 3);
        buffer.replace(0, 1, U"x", {});
        const auto replaced = toEvent(buffer);
        REQUIRE(replaced.text == "xbc");
        buffer.replace(1, 2, U"", {});
        const auto deleted = toEvent(buffer);
        REQUIRE(deleted.text == "x");
    }
    SUBCASE("clamps the range to the text")
    {
        buffer.replace(0, 0, U"ab", {});
        buffer.replace(5, 3, U"c", {});
        const auto appended = toEvent(buffer);
        REQUIRE(appended.text == "abc");
        buffer.replace(1, 10, U"", {});
        const auto deleted = toEvent(buffer);
        REQUIRE(deleted.text == "a");
    }
    SUBCASE("reports offsets in UTF-8 bytes")
    {
        // NOTE: U+304B U+3093 U+3058, "kanji" in hiragana, take three bytes
        // each in UTF-8.
        const std::array<u8, 3> target = {0, 1, 1};
        buffer.replace(0, 0, U"かんじ", target);
        buffer.setCaret(1);
        const auto event = toEvent(buffer);
        REQUIRE(event.text == "\xE3\x81\x8B\xE3\x82\x93\xE3\x81\x98");
        REQUIRE(event.caret == 3);
        REQUIRE(event.targetStart == 3);
        REQUIRE(event.targetEnd == 9);
    }
    SUBCASE("keeps the marks of the characters around an edit")
    {
        const std::array<u8, 4> target = {1, 1, 0, 0};
        buffer.replace(0, 0, U"abcd", target);
        const std::array<u8, 1> inserted = {1};
        buffer.replace(2, 1, U"xy", inserted);
        const auto event = toEvent(buffer);
        REQUIRE(event.text == "abxyd");
        REQUIRE(event.targetStart == 0);
        REQUIRE(event.targetEnd == 3);
    }
    SUBCASE("reports the first run of marked characters")
    {
        const std::array<u8, 4> target = {0, 1, 0, 1};
        buffer.replace(0, 0, U"abcd", target);
        const auto event = toEvent(buffer);
        REQUIRE(event.targetStart == 1);
        REQUIRE(event.targetEnd == 2);
    }
    SUBCASE("changes marks without changing the text")
    {
        buffer.replace(0, 0, U"abc", {});
        const std::array<u8, 3> target = {1, 1, 1};
        buffer.setTarget(1, target);
        const auto event = toEvent(buffer);
        REQUIRE(event.text == "abc");
        REQUIRE(event.targetStart == 1);
        REQUIRE(event.targetEnd == 3);

        const std::array<u8, 2> cleared = {0, 0};
        buffer.setTarget(1, cleared);
        const auto unmarked = toEvent(buffer);
        REQUIRE(unmarked.targetStart == unmarked.targetEnd);
    }
    SUBCASE("clamps the caret to the text")
    {
        buffer.replace(0, 0, U"ab", {});
        buffer.setCaret(10);
        REQUIRE(buffer.getCaret() == 2);
        buffer.replace(0, 2, U"", {});
        REQUIRE(buffer.getCaret() == 0);
    }
    SUBCASE("clear removes the text and moves the caret to the start")
    {
        const std::array<u8, 2> target = {1, 1};
        buffer.replace(0, 0, U"ab", target);
        buffer.setCaret(2);
        buffer.clear();
        REQUIRE(buffer.getLength() == 0);
        REQUIRE(buffer.getCaret() == 0);
        const auto event = toEvent(buffer);
        REQUIRE(event.text.empty());
        REQUIRE(event.targetStart == 0);
        REQUIRE(event.targetEnd == 0);
    }
}
