// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/text_input/utf16_conversion.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::textinput::detail::appendUTF16AsUTF8;
using pomdog::textinput::detail::toUTF8Offset;

namespace {

[[nodiscard]] std::string
toUTF8(std::u16string_view text)
{
    std::string result;
    appendUTF16AsUTF8(text, result);
    return result;
}

} // namespace

TEST_CASE("appendUTF16AsUTF8")
{
    // NOTE: MSVC warns (C4866) when a REQUIRE argument is a temporary string,
    // so each result is named first.
    SUBCASE("converts ASCII")
    {
        const auto result = toUTF8(u"abc");
        REQUIRE(result == "abc");
    }
    SUBCASE("converts characters of the basic multilingual plane")
    {
        // NOTE: U+65E5 U+672C, "Japan" in Japanese.
        const auto result = toUTF8(u"日本");
        REQUIRE(result == "\xE6\x97\xA5\xE6\x9C\xAC");
    }
    SUBCASE("joins a surrogate pair")
    {
        // NOTE: U+1F3AE, a video game emoji.
        const auto result = toUTF8(u"\xD83C\xDFAE");
        REQUIRE(result == "\xF0\x9F\x8E\xAE");
    }
    SUBCASE("replaces unpaired surrogates")
    {
        const auto highOnly = toUTF8(u"\xD83C!");
        const auto lowOnly = toUTF8(u"!\xDFAE");
        REQUIRE(highOnly == "\xEF\xBF\xBD!");
        REQUIRE(lowOnly == "!\xEF\xBF\xBD");
    }
    SUBCASE("appends to the existing text")
    {
        std::string result = "x";
        appendUTF16AsUTF8(u"y", result);
        REQUIRE(result == "xy");
    }
}

TEST_CASE("toUTF8Offset")
{
    // NOTE: "a", U+65E5, and U+1F3AE take 1, 3, and 4 bytes in UTF-8.
    const std::u16string_view text = u"a日\xD83C\xDFAE";
    REQUIRE(toUTF8Offset(text, 0) == 0);
    REQUIRE(toUTF8Offset(text, 1) == 1);
    REQUIRE(toUTF8Offset(text, 2) == 4);
    REQUIRE(toUTF8Offset(text, 4) == 8);

    SUBCASE("does not split a surrogate pair")
    {
        REQUIRE(toUTF8Offset(text, 3) == 4);
    }
    SUBCASE("stops at the end of the text")
    {
        REQUIRE(toUTF8Offset(text, 10) == 8);
    }
}
