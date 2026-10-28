// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/text_input/text_input_event.h"
#include "pomdog/experimental/text_input/text_mirror.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::textinput::TextInputEvent;
using pomdog::textinput::TextInputEventKind;
using pomdog::textinput::detail::TextMirror;

namespace {

constexpr pomdog::u64 fieldID = 7;

/// Returns the events as one line, such as "commit(c) select(2,2)", so that a
/// test compares them at once.
[[nodiscard]] std::string
describe(const std::vector<TextInputEvent>& events)
{
    std::string result;
    for (const auto& event : events) {
        REQUIRE(event.fieldID == fieldID);
        if (!result.empty()) {
            result += " ";
        }
        // NOTE: An if chain, because the test build warns both about a switch
        // without a default label and about one that covers every kind.
        const auto kind = event.kind;
        if (kind == TextInputEventKind::Commit) {
            result += "commit(" + event.text + ")";
        }
        else if (kind == TextInputEventKind::CompositionUpdate) {
            result += "compose(" + event.text + "," + std::to_string(event.caret) + "," +
                      std::to_string(event.targetStart) + "," + std::to_string(event.targetEnd) + ")";
        }
        else if (kind == TextInputEventKind::CompositionEnd) {
            result += "end";
        }
        else if (kind == TextInputEventKind::DeleteSurrounding) {
            result += "delete(" + std::to_string(event.deleteBefore) + "," + std::to_string(event.deleteAfter) + ")";
        }
        else if (kind == TextInputEventKind::SetSelection) {
            result += "select(" + std::to_string(event.caret) + "," + std::to_string(event.anchor) + ")";
        }
    }
    return result;
}

/// Returns mirror.matches(text, caret, anchor). The text is a C string because
/// MSVC warns (C4866) when a REQUIRE argument converts a string literal to a
/// std::string_view.
[[nodiscard]] bool
shows(const TextMirror& mirror, const char* text, pomdog::u32 caret, pomdog::u32 anchor)
{
    return mirror.matches(text, caret, anchor);
}

} // namespace

TEST_CASE("TextMirror")
{
    // NOTE: MSVC warns (C4866) when a REQUIRE argument is a temporary string,
    // so each description is named first.
    TextMirror mirror;
    std::vector<TextInputEvent> events;

    SUBCASE("typing inserts at the caret")
    {
        mirror.reset("ab", 2, 2);
        mirror.applyEdit("abc", 3, 3, fieldID, events);
        const auto log = describe(events);
        REQUIRE(log == "commit(c)");
        REQUIRE(shows(mirror, "abc", 3, 3));
    }
    SUBCASE("typing replaces the selection")
    {
        mirror.reset("hello", 5, 0);
        mirror.applyEdit("x", 1, 1, fieldID, events);
        const auto log = describe(events);
        REQUIRE(log == "commit(x)");
    }
    SUBCASE("a deletion before the caret deletes surrounding text")
    {
        mirror.reset("abc", 3, 3);
        mirror.applyEdit("ab", 2, 2, fieldID, events);
        const auto log = describe(events);
        REQUIRE(log == "delete(1,0)");
    }
    SUBCASE("a deletion keeps whole code points")
    {
        mirror.reset("a\u00E9", 3, 3);
        mirror.applyEdit("a", 1, 1, fieldID, events);
        const auto deleted = describe(events);
        REQUIRE(deleted == "delete(2,0)");

        // NOTE: The two letters share their first byte.
        events.clear();
        mirror.reset("\u00E9", 2, 2);
        mirror.applyEdit("\u00E8", 2, 2, fieldID, events);
        const auto replaced = describe(events);
        REQUIRE(replaced == "delete(2,0) commit(\u00E8)");
    }
    SUBCASE("a deleted selection")
    {
        mirror.reset("abcd", 1, 3);
        mirror.applyEdit("ad", 1, 1, fieldID, events);
        const auto log = describe(events);
        REQUIRE(log == "select(3,3) delete(2,0)");
    }
    SUBCASE("a moved caret selects")
    {
        mirror.reset("hello", 5, 5);
        mirror.applyEdit("hello", 2, 2, fieldID, events);
        mirror.applyEdit("hello", 4, 1, fieldID, events);
        const auto log = describe(events);
        REQUIRE(log == "select(2,2) select(4,1)");
        REQUIRE(shows(mirror, "hello", 4, 1));
    }
    SUBCASE("a word replaced away from the caret")
    {
        mirror.reset("teh cat", 7, 7);
        mirror.applyEdit("the cat", 7, 7, fieldID, events);
        const auto log = describe(events);
        REQUIRE(log == "select(3,3) delete(2,0) commit(he) select(7,7)");
    }
    SUBCASE("an unchanged state adds nothing")
    {
        mirror.reset("abc", 1, 1);
        mirror.applyEdit("abc", 1, 1, fieldID, events);
        REQUIRE(events.empty());
    }
    SUBCASE("reset clamps the selection")
    {
        mirror.reset("ab", 9, 5);
        REQUIRE(shows(mirror, "ab", 2, 2));
    }
    SUBCASE("composes at the caret and commits")
    {
        // NOTE: Each kana is three bytes.
        mirror.reset("ab", 1, 1);
        mirror.applyComposition("aかb", 4, 4, fieldID, events);
        mirror.applyComposition("aかなb", 7, 7, fieldID, events);
        REQUIRE(mirror.isComposing());
        REQUIRE(shows(mirror, "ab", 1, 1));
        mirror.endComposition("a仮名b", 7, 7, fieldID, events);
        REQUIRE_FALSE(mirror.isComposing());
        const auto log = describe(events);
        REQUIRE(log == "compose(か,3,0,0) compose(かな,6,0,0) commit(仮名) end");
        REQUIRE(shows(mirror, "a仮名b", 7, 7));
    }
    SUBCASE("a selection in the composition marks the converted clause")
    {
        mirror.reset("", 0, 0);
        mirror.applyComposition("かんじ", 9, 3, fieldID, events);
        const auto log = describe(events);
        REQUIRE(log == "compose(かんじ,9,3,9)");
    }
    SUBCASE("a composition replaces the selection")
    {
        mirror.reset("abc", 3, 0);
        mirror.applyComposition("x", 1, 1, fieldID, events);
        mirror.endComposition("x", 1, 1, fieldID, events);
        const auto log = describe(events);
        REQUIRE(log == "compose(x,1,0,0) commit(x) end");
    }
    SUBCASE("a composition over text of the field selects that text")
    {
        mirror.reset("cat sat", 7, 7);
        mirror.applyComposition("cat mat", 7, 7, fieldID, events);
        REQUIRE(shows(mirror, "cat sat", 5, 4));
        mirror.endComposition("cat mat", 7, 7, fieldID, events);
        const auto log = describe(events);
        REQUIRE(log == "select(5,4) compose(m,1,0,0) commit(m) select(7,7) end");
    }
    SUBCASE("a cancelled composition only ends")
    {
        mirror.reset("ab", 1, 1);
        mirror.applyComposition("axb", 2, 2, fieldID, events);
        mirror.endComposition("ab", 1, 1, fieldID, events);
        const auto log = describe(events);
        REQUIRE(log == "compose(x,1,0,0) end");
    }
}
