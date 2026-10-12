// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/i18n/line_break_rules.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::i18n::isBreakableWhitespace;
using pomdog::i18n::isCJKCharacter;
using pomdog::i18n::isLineEndProhibited;
using pomdog::i18n::isLineStartProhibited;
using pomdog::i18n::isThaiCharacter;
using pomdog::i18n::isThaiCombiningVowelOrToneMark;
using pomdog::i18n::isThaiLeadingVowel;

TEST_CASE("isCJKCharacter")
{
    SUBCASE("CJK unified ideographs")
    {
        CHECK(isCJKCharacter(U'漢'));
        CHECK(isCJKCharacter(U'字'));
        CHECK(isCJKCharacter(U'中'));
        CHECK(isCJKCharacter(0x4E00));
        CHECK(isCJKCharacter(0x9FFF));
        CHECK(isCJKCharacter(0x3400));
        CHECK(isCJKCharacter(0x20000));
    }
    SUBCASE("kana")
    {
        CHECK(isCJKCharacter(U'あ'));
        CHECK(isCJKCharacter(U'ん'));
        CHECK(isCJKCharacter(U'ア'));
        CHECK(isCJKCharacter(U'ン'));
        CHECK(isCJKCharacter(U'ｱ'));
    }
    SUBCASE("Hangul")
    {
        CHECK(isCJKCharacter(U'한'));
        CHECK(isCJKCharacter(U'글'));
        CHECK(isCJKCharacter(0xAC00));
        CHECK(isCJKCharacter(0xD7AF));
    }
    SUBCASE("CJK symbols, punctuation, and full-width forms")
    {
        CHECK(isCJKCharacter(U'、'));
        CHECK(isCJKCharacter(U'。'));
        CHECK(isCJKCharacter(U'「'));
        CHECK(isCJKCharacter(U'」'));
        CHECK(isCJKCharacter(U'！'));
        CHECK(isCJKCharacter(U'Ａ'));
    }
    SUBCASE("non-CJK characters")
    {
        CHECK_FALSE(isCJKCharacter(U'A'));
        CHECK_FALSE(isCJKCharacter(U'z'));
        CHECK_FALSE(isCJKCharacter(U'0'));
        CHECK_FALSE(isCJKCharacter(U' '));
        CHECK_FALSE(isCJKCharacter(U'\n'));
        CHECK_FALSE(isCJKCharacter(U'\u3000'));
        CHECK_FALSE(isCJKCharacter(0x0E01));
    }
    SUBCASE("halfwidth CJK punctuation and full-width white parentheses")
    {
        CHECK(isCJKCharacter(U'\uFF61'));
        CHECK(isCJKCharacter(U'\uFF62'));
        CHECK(isCJKCharacter(U'\uFF64'));
        CHECK(isCJKCharacter(U'\uFF5F'));
        CHECK(isCJKCharacter(U'\uFF60'));
    }
}

TEST_CASE("isLineStartProhibited")
{
    SUBCASE("CJK closing punctuation")
    {
        CHECK(isLineStartProhibited(U'、'));
        CHECK(isLineStartProhibited(U'。'));
        CHECK(isLineStartProhibited(U'」'));
        CHECK(isLineStartProhibited(U'』'));
        CHECK(isLineStartProhibited(U'）'));
        CHECK(isLineStartProhibited(U'！'));
    }
    SUBCASE("Japanese small kana and prolonged sound marks")
    {
        CHECK(isLineStartProhibited(U'っ'));
        CHECK(isLineStartProhibited(U'ッ'));
        CHECK(isLineStartProhibited(U'ゃ'));
        CHECK(isLineStartProhibited(U'ョ'));
        CHECK(isLineStartProhibited(U'ー'));
        CHECK(isLineStartProhibited(U'\u301C'));
        CHECK(isLineStartProhibited(U'ゕ'));
        CHECK(isLineStartProhibited(U'ヶ'));
        CHECK(isLineStartProhibited(0x31F0));
        CHECK(isLineStartProhibited(0x31FF));
        CHECK(isLineStartProhibited(U'\uFF67'));
        CHECK(isLineStartProhibited(U'\uFF6F'));
        CHECK(isLineStartProhibited(U'\uFF70'));
    }
    SUBCASE("Japanese iteration marks and sound marks")
    {
        CHECK(isLineStartProhibited(U'々'));
        CHECK(isLineStartProhibited(U'〻'));
        CHECK(isLineStartProhibited(U'ゝ'));
        CHECK(isLineStartProhibited(U'ゞ'));
        CHECK(isLineStartProhibited(U'ヽ'));
        CHECK(isLineStartProhibited(U'ヾ'));
        CHECK(isLineStartProhibited(U'゛'));
        CHECK(isLineStartProhibited(U'゜'));
        CHECK(isLineStartProhibited(U'\uFF9E'));
        CHECK(isLineStartProhibited(U'\uFF9F'));
    }
    SUBCASE("hyphens, closing quotation marks, and halfwidth punctuation")
    {
        CHECK(isLineStartProhibited(U'゠'));
        CHECK(isLineStartProhibited(U'\u2010'));
        CHECK(isLineStartProhibited(U'\u2013'));
        CHECK(isLineStartProhibited(U'\u2019'));
        CHECK(isLineStartProhibited(U'\u201D'));
        CHECK(isLineStartProhibited(U'\u203A'));
        CHECK(isLineStartProhibited(U'\u00BB'));
        CHECK(isLineStartProhibited(U'\u301E'));
        CHECK(isLineStartProhibited(U'\u301F'));
        CHECK(isLineStartProhibited(U'\uFF60'));
        CHECK(isLineStartProhibited(U'\u2986'));
        CHECK(isLineStartProhibited(U'\uFF61'));
        CHECK(isLineStartProhibited(U'\uFF63'));
        CHECK(isLineStartProhibited(U'\uFF64'));
        CHECK(isLineStartProhibited(U'\uFF65'));
        CHECK(isLineStartProhibited(U'\u203C'));
        CHECK(isLineStartProhibited(U'\u2049'));
    }
    SUBCASE("ASCII punctuation")
    {
        CHECK(isLineStartProhibited(U')'));
        CHECK(isLineStartProhibited(U']'));
        CHECK(isLineStartProhibited(U','));
        CHECK(isLineStartProhibited(U'.'));
        CHECK(isLineStartProhibited(U'!'));
        CHECK(isLineStartProhibited(U'?'));
    }
    SUBCASE("Thai vowels and tone marks after a consonant")
    {
        CHECK(isLineStartProhibited(0x0E30));
        CHECK(isLineStartProhibited(0x0E32));
        CHECK(isLineStartProhibited(0x0E48));
    }
    SUBCASE("Thai repetition and abbreviation marks")
    {
        CHECK(isLineStartProhibited(0x0E46));
        CHECK(isLineStartProhibited(0x0E2F));
    }
    SUBCASE("characters that may start a line")
    {
        CHECK_FALSE(isLineStartProhibited(U'A'));
        CHECK_FALSE(isLineStartProhibited(U'あ'));
        CHECK_FALSE(isLineStartProhibited(U'漢'));
        CHECK_FALSE(isLineStartProhibited(U'「'));
        CHECK_FALSE(isLineStartProhibited(U'('));
        CHECK_FALSE(isLineStartProhibited(U'\uFF62'));
        CHECK_FALSE(isLineStartProhibited(U'\uFF71'));
        CHECK_FALSE(isLineStartProhibited(0x0E40));
        CHECK_FALSE(isLineStartProhibited(0x0E2E));
    }
}

TEST_CASE("isLineEndProhibited")
{
    SUBCASE("CJK opening brackets")
    {
        CHECK(isLineEndProhibited(U'「'));
        CHECK(isLineEndProhibited(U'『'));
        CHECK(isLineEndProhibited(U'（'));
        CHECK(isLineEndProhibited(U'【'));
        CHECK(isLineEndProhibited(U'《'));
        CHECK(isLineEndProhibited(U'\u301D'));
        CHECK(isLineEndProhibited(U'\uFF5F'));
        CHECK(isLineEndProhibited(U'\u2985'));
        CHECK(isLineEndProhibited(U'\uFF62'));
    }
    SUBCASE("ASCII opening brackets")
    {
        CHECK(isLineEndProhibited(U'('));
        CHECK(isLineEndProhibited(U'['));
        CHECK(isLineEndProhibited(U'{'));
    }
    SUBCASE("opening quotation marks")
    {
        CHECK(isLineEndProhibited(U'\u2018'));
        CHECK(isLineEndProhibited(U'\u201C'));
        CHECK(isLineEndProhibited(U'\u2039'));
        CHECK(isLineEndProhibited(U'\u00AB'));
    }
    SUBCASE("Thai leading vowels")
    {
        CHECK(isLineEndProhibited(0x0E40));
        CHECK(isLineEndProhibited(0x0E41));
        CHECK(isLineEndProhibited(0x0E44));
    }
    SUBCASE("characters that may end a line")
    {
        CHECK_FALSE(isLineEndProhibited(U'A'));
        CHECK_FALSE(isLineEndProhibited(U'あ'));
        CHECK_FALSE(isLineEndProhibited(U'」'));
        CHECK_FALSE(isLineEndProhibited(U')'));
        CHECK_FALSE(isLineEndProhibited(U'\uFF63'));
        CHECK_FALSE(isLineEndProhibited(0x0E01));
    }
}

TEST_CASE("isBreakableWhitespace")
{
    SUBCASE("spaces")
    {
        CHECK(isBreakableWhitespace(U'\u0020'));
        CHECK(isBreakableWhitespace(U'\u0009'));
        CHECK(isBreakableWhitespace(U'\u3000'));
        CHECK(isBreakableWhitespace(U'\u2003'));
        CHECK(isBreakableWhitespace(U'\u200B'));
    }
    SUBCASE("no-break spaces and line terminators")
    {
        CHECK_FALSE(isBreakableWhitespace(U'\u00A0'));
        CHECK_FALSE(isBreakableWhitespace(U'\u202F'));
        CHECK_FALSE(isBreakableWhitespace(U'\n'));
        CHECK_FALSE(isBreakableWhitespace(U'\r'));
    }
    SUBCASE("other characters")
    {
        CHECK_FALSE(isBreakableWhitespace(U'A'));
        CHECK_FALSE(isBreakableWhitespace(U'0'));
        CHECK_FALSE(isBreakableWhitespace(U'あ'));
    }
}

TEST_CASE("isThaiCharacter")
{
    CHECK(isThaiCharacter(0x0E01));
    CHECK(isThaiCharacter(0x0E2E));
    CHECK(isThaiCharacter(0x0E30));
    CHECK(isThaiCharacter(0x0E40));
    CHECK(isThaiCharacter(0x0E48));
    CHECK(isThaiCharacter(0x0E59));
    CHECK_FALSE(isThaiCharacter(U'A'));
    CHECK_FALSE(isThaiCharacter(U'あ'));
    CHECK_FALSE(isThaiCharacter(U'漢'));
}

TEST_CASE("isThaiCombiningVowelOrToneMark")
{
    SUBCASE("following vowels")
    {
        CHECK(isThaiCombiningVowelOrToneMark(0x0E30));
        CHECK(isThaiCombiningVowelOrToneMark(0x0E32));
        CHECK(isThaiCombiningVowelOrToneMark(0x0E33));
    }
    SUBCASE("above and below vowels")
    {
        CHECK(isThaiCombiningVowelOrToneMark(0x0E31));
        CHECK(isThaiCombiningVowelOrToneMark(0x0E34));
        CHECK(isThaiCombiningVowelOrToneMark(0x0E38));
        CHECK(isThaiCombiningVowelOrToneMark(0x0E3A));
    }
    SUBCASE("tone marks and signs")
    {
        CHECK(isThaiCombiningVowelOrToneMark(0x0E48));
        CHECK(isThaiCombiningVowelOrToneMark(0x0E4B));
        CHECK(isThaiCombiningVowelOrToneMark(0x0E4C));
    }
    SUBCASE("leading vowels and consonants")
    {
        CHECK_FALSE(isThaiCombiningVowelOrToneMark(0x0E40));
        CHECK_FALSE(isThaiCombiningVowelOrToneMark(0x0E42));
        CHECK_FALSE(isThaiCombiningVowelOrToneMark(0x0E01));
    }
}

TEST_CASE("isThaiLeadingVowel")
{
    CHECK(isThaiLeadingVowel(0x0E40));
    CHECK(isThaiLeadingVowel(0x0E41));
    CHECK(isThaiLeadingVowel(0x0E42));
    CHECK(isThaiLeadingVowel(0x0E43));
    CHECK(isThaiLeadingVowel(0x0E44));
    CHECK_FALSE(isThaiLeadingVowel(0x0E30));
    CHECK_FALSE(isThaiLeadingVowel(0x0E32));
    CHECK_FALSE(isThaiLeadingVowel(0x0E01));
}
