// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/text_input/utf32_conversion.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::textinput::detail::appendCodePointAsUTF8;
using pomdog::textinput::detail::appendUTF32AsUTF8;
using pomdog::textinput::detail::appendUTF8AsUTF32;
using pomdog::textinput::detail::toUTF8Offset;

namespace {

[[nodiscard]] std::u32string
toUTF32(std::string_view text)
{
    std::u32string result;
    appendUTF8AsUTF32(text, result);
    return result;
}

[[nodiscard]] std::string
toUTF8(std::u32string_view text)
{
    std::string result;
    appendUTF32AsUTF8(text, result);
    return result;
}

} // namespace

TEST_CASE("appendCodePointAsUTF8")
{
    SUBCASE("encodes one to four bytes")
    {
        // NOTE: U+00E9, U+65E5, and U+1F3AE take 2, 3, and 4 bytes in UTF-8.
        std::string result;
        appendCodePointAsUTF8(U'a', result);
        appendCodePointAsUTF8(char32_t{0x00E9}, result);
        appendCodePointAsUTF8(char32_t{0x65E5}, result);
        appendCodePointAsUTF8(char32_t{0x1F3AE}, result);
        REQUIRE(result == "a\xC3\xA9\xE6\x97\xA5\xF0\x9F\x8E\xAE");
    }
    SUBCASE("replaces surrogates and values past U+10FFFF")
    {
        std::string result;
        appendCodePointAsUTF8(char32_t{0xD800}, result);
        appendCodePointAsUTF8(char32_t{0x110000}, result);
        REQUIRE(result == "\xEF\xBF\xBD\xEF\xBF\xBD");
    }
}

TEST_CASE("appendUTF8AsUTF32")
{
    // NOTE: MSVC warns (C4866) when a REQUIRE argument is a temporary string,
    // so each result is named first. U+FFFD is the replacement character.
    SUBCASE("decodes one to four bytes")
    {
        const auto result = toUTF32("a\xC3\xA9\xE6\x97\xA5\xF0\x9F\x8E\xAE");
        const std::u32string expected = U"a\u00E9日\U0001F3AE";
        REQUIRE(result == expected);
    }
    SUBCASE("replaces each byte of a truncated sequence")
    {
        const auto result = toUTF32("\xE6\x97!");
        const std::u32string expected = U"\uFFFD\uFFFD!";
        REQUIRE(result == expected);
    }
    SUBCASE("rejects overlong forms")
    {
        // NOTE: Both encode U+002F, which takes one byte.
        const auto twoBytes = toUTF32("\xC0\xAF");
        const auto threeBytes = toUTF32("\xE0\x80\xAF");
        const std::u32string expectedTwo = U"\uFFFD\uFFFD";
        const std::u32string expectedThree = U"\uFFFD\uFFFD\uFFFD";
        REQUIRE(twoBytes == expectedTwo);
        REQUIRE(threeBytes == expectedThree);
    }
    SUBCASE("rejects surrogates and values past U+10FFFF")
    {
        const auto surrogate = toUTF32("\xED\xA0\x80");
        const auto tooLarge = toUTF32("\xF4\x90\x80\x80");
        const std::u32string expectedSurrogate = U"\uFFFD\uFFFD\uFFFD";
        const std::u32string expectedTooLarge = U"\uFFFD\uFFFD\uFFFD\uFFFD";
        REQUIRE(surrogate == expectedSurrogate);
        REQUIRE(tooLarge == expectedTooLarge);
    }
    SUBCASE("appends to the existing text")
    {
        std::u32string result = U"x";
        appendUTF8AsUTF32("y", result);
        const std::u32string expected = U"xy";
        REQUIRE(result == expected);
    }
}

TEST_CASE("appendUTF32AsUTF8")
{
    SUBCASE("encodes each character")
    {
        const auto result = toUTF8(U"a\u00E9日\U0001F3AE");
        REQUIRE(result == "a\xC3\xA9\xE6\x97\xA5\xF0\x9F\x8E\xAE");
    }
    SUBCASE("replaces invalid code points")
    {
        const std::u32string text = {U'a', char32_t{0xD800}};
        const auto result = toUTF8(text);
        REQUIRE(result == "a\xEF\xBF\xBD");
    }
}

TEST_CASE("toUTF8Offset of UTF-32 text")
{
    // NOTE: "a", U+65E5, and U+1F3AE take 1, 3, and 4 bytes in UTF-8.
    const std::u32string_view text = U"a日\U0001F3AE";
    REQUIRE(toUTF8Offset(text, 0) == 0);
    REQUIRE(toUTF8Offset(text, 1) == 1);
    REQUIRE(toUTF8Offset(text, 2) == 4);
    REQUIRE(toUTF8Offset(text, 3) == 8);

    SUBCASE("stops at the end of the text")
    {
        REQUIRE(toUTF8Offset(text, 10) == 8);
    }
    SUBCASE("counts an invalid code point as U+FFFD")
    {
        const std::u32string invalid = {char32_t{0xD800}, U'a'};
        REQUIRE(toUTF8Offset(invalid, 1) == 3);
        REQUIRE(toUTF8Offset(invalid, 2) == 4);
    }
}
