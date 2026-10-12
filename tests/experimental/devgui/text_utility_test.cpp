// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/devgui/text_utility.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::devgui::detail::clampCharacterBoundary;
using pomdog::devgui::detail::findCharacterBoundaryAt;
using pomdog::devgui::detail::fitTextWithEllipsis;
using pomdog::devgui::detail::nextCharacterBoundary;
using pomdog::devgui::detail::previousCharacterBoundary;

namespace {

// NOTE: Non-ASCII text is written as UTF-8 byte escapes, with the characters
// named in comments.

// NOTE: U+65E5 and U+672C, "Japan" in Japanese.
constexpr std::string_view japanese = "\xE6\x97\xA5\xE6\x9C\xAC";

// NOTE: U+0E01 THAI CHARACTER KO KAI, U+0E35 THAI CHARACTER SARA II above it,
// and U+0E19 THAI CHARACTER NO NU.
constexpr std::string_view thai = "\xE0\xB8\x81\xE0\xB8\xB5\xE0\xB8\x99";

// NOTE: "e" followed by U+0301 COMBINING ACUTE ACCENT, then "x".
constexpr std::string_view combining = "e\xCC\x81x";

// NOTE: U+1F44D THUMBS UP SIGN with U+1F3FB EMOJI MODIFIER FITZPATRICK TYPE-1-2.
constexpr std::string_view skinTone = "\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBB";

// NOTE: U+1F468 MAN, U+200D ZERO WIDTH JOINER, and U+1F469 WOMAN.
constexpr std::string_view joined = "\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA9";

// NOTE: U+30AB KATAKANA LETTER KA with U+3099 COMBINING KATAKANA-HIRAGANA
// VOICED SOUND MARK, a decomposed "ga", and U+30CF KATAKANA LETTER HA with
// U+309A COMBINING KATAKANA-HIRAGANA SEMI-VOICED SOUND MARK, a decomposed "pa".
constexpr std::string_view voicedKana = "\xE3\x82\xAB\xE3\x82\x99";
constexpr std::string_view semiVoicedKana = "\xE3\x83\x8F\xE3\x82\x9A";

// NOTE: "1", U+FE0F VARIATION SELECTOR-16, and U+20E3 COMBINING ENCLOSING
// KEYCAP, the keycap digit one emoji.
constexpr std::string_view keycap = "1\xEF\xB8\x8F\xE2\x83\xA3";

// NOTE: The flags of Japan (U+1F1EF U+1F1F5) and the United States
// (U+1F1FA U+1F1F8), each a pair of regional indicators, then a lone U+1F1EF.
constexpr std::string_view flags = "\xF0\x9F\x87\xAF\xF0\x9F\x87\xB5\xF0\x9F\x87\xBA\xF0\x9F\x87\xB8\xF0\x9F\x87\xAF";

constexpr std::string_view ascii = "abcd";
constexpr std::string_view letters = "abcdefgh";

// NOTE: A byte that cannot start a UTF-8 sequence, and a truncated sequence.
constexpr std::string_view invalidLead = "\xFF"
                                         "a";
constexpr std::string_view truncated = "\xE6\x97";

[[nodiscard]] f32
measureBytes(std::string_view text)
{
    return static_cast<f32>(text.size()) * 10.0f;
}

} // namespace

TEST_CASE("devgui::detail::nextCharacterBoundary")
{
    SUBCASE("steps over ASCII and multibyte characters")
    {
        REQUIRE(nextCharacterBoundary(ascii, 0) == 1);
        REQUIRE(nextCharacterBoundary(japanese, 0) == 3);
        REQUIRE(nextCharacterBoundary(japanese, 3) == 6);
        REQUIRE(nextCharacterBoundary(japanese, 6) == 6);
    }
    SUBCASE("keeps marks with their base character")
    {
        REQUIRE(nextCharacterBoundary(thai, 0) == 6);
        REQUIRE(nextCharacterBoundary(thai, 6) == 9);
        REQUIRE(nextCharacterBoundary(combining, 0) == 3);
        REQUIRE(nextCharacterBoundary(skinTone, 0) == 8);
        REQUIRE(nextCharacterBoundary(joined, 0) == 11);
        REQUIRE(nextCharacterBoundary(voicedKana, 0) == 6);
        REQUIRE(nextCharacterBoundary(semiVoicedKana, 0) == 6);
        REQUIRE(nextCharacterBoundary(keycap, 0) == 7);
    }
    SUBCASE("pairs regional indicators into flags")
    {
        REQUIRE(nextCharacterBoundary(flags, 0) == 8);
        REQUIRE(nextCharacterBoundary(flags, 8) == 16);
        REQUIRE(nextCharacterBoundary(flags, 16) == 20);
    }
    SUBCASE("treats invalid bytes as single characters")
    {
        REQUIRE(nextCharacterBoundary(invalidLead, 0) == 1);
        REQUIRE(nextCharacterBoundary(truncated, 0) == 1);
    }
}

TEST_CASE("devgui::detail::previousCharacterBoundary")
{
    REQUIRE(previousCharacterBoundary(ascii, 3) == 2);
    REQUIRE(previousCharacterBoundary(ascii, 0) == 0);
    REQUIRE(previousCharacterBoundary(japanese, 6) == 3);
    REQUIRE(previousCharacterBoundary(thai, 6) == 0);
    REQUIRE(previousCharacterBoundary(thai, 9) == 6);
    REQUIRE(previousCharacterBoundary(skinTone, 8) == 0);
    REQUIRE(previousCharacterBoundary(voicedKana, 6) == 0);
    REQUIRE(previousCharacterBoundary(keycap, 7) == 0);
    REQUIRE(previousCharacterBoundary(flags, 16) == 8);
    REQUIRE(previousCharacterBoundary(flags, 20) == 16);
}

TEST_CASE("devgui::detail::clampCharacterBoundary")
{
    SUBCASE("preserves boundaries and clamps offsets past the end")
    {
        const auto emptyBoundary = clampCharacterBoundary({}, 1);
        REQUIRE(emptyBoundary == 0);
        REQUIRE(clampCharacterBoundary(ascii, 0) == 0);
        REQUIRE(clampCharacterBoundary(ascii, 2) == 2);
        REQUIRE(clampCharacterBoundary(ascii, 100) == 4);
        REQUIRE(clampCharacterBoundary(japanese, 3) == 3);
        REQUIRE(clampCharacterBoundary(japanese, 6) == 6);
        REQUIRE(clampCharacterBoundary(japanese, 100) == 6);
    }
    SUBCASE("moves offsets inside UTF-8 characters back to their start")
    {
        REQUIRE(clampCharacterBoundary(japanese, 1) == 0);
        REQUIRE(clampCharacterBoundary(japanese, 2) == 0);
        REQUIRE(clampCharacterBoundary(japanese, 4) == 3);
        REQUIRE(clampCharacterBoundary(japanese, 5) == 3);
    }
    SUBCASE("keeps combining marks and joined emoji with their base character")
    {
        REQUIRE(clampCharacterBoundary(thai, 3) == 0);
        REQUIRE(clampCharacterBoundary(thai, 5) == 0);
        REQUIRE(clampCharacterBoundary(thai, 6) == 6);
        REQUIRE(clampCharacterBoundary(combining, 1) == 0);
        REQUIRE(clampCharacterBoundary(combining, 2) == 0);
        REQUIRE(clampCharacterBoundary(combining, 3) == 3);
        REQUIRE(clampCharacterBoundary(skinTone, 4) == 0);
        REQUIRE(clampCharacterBoundary(skinTone, 8) == 8);
        REQUIRE(clampCharacterBoundary(joined, 4) == 0);
        REQUIRE(clampCharacterBoundary(joined, 7) == 0);
        REQUIRE(clampCharacterBoundary(joined, 10) == 0);
        REQUIRE(clampCharacterBoundary(joined, 11) == 11);
        REQUIRE(clampCharacterBoundary(voicedKana, 3) == 0);
        REQUIRE(clampCharacterBoundary(semiVoicedKana, 3) == 0);
        REQUIRE(clampCharacterBoundary(keycap, 1) == 0);
        REQUIRE(clampCharacterBoundary(keycap, 4) == 0);
        REQUIRE(clampCharacterBoundary(flags, 4) == 0);
        REQUIRE(clampCharacterBoundary(flags, 12) == 8);
        REQUIRE(clampCharacterBoundary(flags, 16) == 16);
    }
}

TEST_CASE("devgui::detail::fitTextWithEllipsis")
{
    // NOTE: Each byte is 10 pixels wide and the ellipsis is 30.
    REQUIRE(fitTextWithEllipsis(measureBytes, letters, 75.0f, 30.0f) == 4);
    REQUIRE(fitTextWithEllipsis(measureBytes, letters, 20.0f, 30.0f) == 0);

    // NOTE: A Thai character and its mark take 60 pixels together, so they do
    // not fit in 50.
    REQUIRE(fitTextWithEllipsis(measureBytes, thai, 80.0f, 30.0f) == 0);
    REQUIRE(fitTextWithEllipsis(measureBytes, thai, 90.0f, 30.0f) == 6);
}

TEST_CASE("devgui::detail::findCharacterBoundaryAt")
{
    REQUIRE(findCharacterBoundaryAt(measureBytes, ascii, -5.0f) == 0);
    REQUIRE(findCharacterBoundaryAt(measureBytes, ascii, 14.0f) == 1);
    REQUIRE(findCharacterBoundaryAt(measureBytes, ascii, 16.0f) == 2);
    REQUIRE(findCharacterBoundaryAt(measureBytes, ascii, 100.0f) == 4);
    REQUIRE(findCharacterBoundaryAt(measureBytes, japanese, 20.0f) == 3);
}
