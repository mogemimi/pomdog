// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/clipboard/text_encoding.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::clipboard::detail::appendLatin1AsUTF8;
using pomdog::clipboard::detail::appendValidUTF8;

namespace {

[[nodiscard]] std::string
fromLatin1(std::string_view text)
{
    std::string result;
    appendLatin1AsUTF8(text, result);
    return result;
}

[[nodiscard]] std::string
toValidUTF8(std::string_view text)
{
    std::string result;
    appendValidUTF8(text, result);
    return result;
}

} // namespace

TEST_CASE("appendLatin1AsUTF8")
{
    // NOTE: MSVC warns (C4866) when a REQUIRE argument is a temporary string,
    // so each result is named first.
    SUBCASE("keeps ASCII")
    {
        const auto result = fromLatin1("abc\n");
        REQUIRE(result == "abc\n");
    }
    SUBCASE("encodes the upper half as two bytes")
    {
        // NOTE: 0xE9 is U+00E9, and 0x80 and 0xFF are the ends of the range.
        const auto accented = fromLatin1("caf\xE9");
        const auto lowest = fromLatin1("\x80");
        const auto highest = fromLatin1("\xFF");
        REQUIRE(accented == "caf\xC3\xA9");
        REQUIRE(lowest == "\xC2\x80");
        REQUIRE(highest == "\xC3\xBF");
    }
    SUBCASE("appends to the existing text")
    {
        std::string result = "x";
        appendLatin1AsUTF8("y", result);
        REQUIRE(result == "xy");
    }
}

TEST_CASE("appendValidUTF8")
{
    // NOTE: "\xEF\xBF\xBD" is U+FFFD, the replacement character.
    SUBCASE("keeps valid text")
    {
        // NOTE: U+65E5 and U+1F3AE take 3 and 4 bytes.
        const auto result = toValidUTF8("a\xE6\x97\xA5\xF0\x9F\x8E\xAE");
        REQUIRE(result == "a\xE6\x97\xA5\xF0\x9F\x8E\xAE");
    }
    SUBCASE("replaces a byte that cannot start a sequence")
    {
        const auto result = toValidUTF8("a\xFF"
                                        "b");
        REQUIRE(result == "a\xEF\xBF\xBD"
                          "b");
    }
    SUBCASE("replaces a truncated sequence once")
    {
        const auto inside = toValidUTF8("\xE6\x97!");
        const auto atEnd = toValidUTF8("!\xE6\x97");
        REQUIRE(inside == "\xEF\xBF\xBD!");
        REQUIRE(atEnd == "!\xEF\xBF\xBD");
    }
    SUBCASE("replaces overlong forms and surrogates")
    {
        // NOTE: The first encodes U+002F in two bytes, and the second is U+D800.
        const auto overlong = toValidUTF8("\xC0\xAF");
        const auto surrogate = toValidUTF8("\xED\xA0\x80");
        REQUIRE(overlong == "\xEF\xBF\xBD");
        REQUIRE(surrogate == "\xEF\xBF\xBD");
    }
    SUBCASE("appends to the existing text")
    {
        std::string result = "x";
        appendValidUTF8("\xFF", result);
        REQUIRE(result == "x\xEF\xBF\xBD");
    }
}
