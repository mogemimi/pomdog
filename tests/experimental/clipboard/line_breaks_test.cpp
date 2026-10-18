// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/clipboard/line_breaks.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::clipboard::detail::appendWithCRLF;
using pomdog::clipboard::detail::appendWithLF;

namespace {

[[nodiscard]] std::string
toLF(std::string_view text)
{
    std::string result;
    appendWithLF(text, result);
    return result;
}

[[nodiscard]] std::string
toCRLF(std::string_view text)
{
    std::string result;
    appendWithCRLF(text, result);
    return result;
}

} // namespace

TEST_CASE("appendWithLF")
{
    // NOTE: MSVC warns (C4866) when a REQUIRE argument is a temporary string,
    // so each result is named first.
    SUBCASE("turns CR LF and lone CR into LF")
    {
        const auto result = toLF("a\r\nb\rc\nd");
        REQUIRE(result == "a\nb\nc\nd");
    }
    SUBCASE("keeps text without line breaks")
    {
        const auto result = toLF("abc");
        REQUIRE(result == "abc");
    }
    SUBCASE("keeps trailing and repeated line breaks")
    {
        const auto result = toLF("\r\n\r\na\r");
        REQUIRE(result == "\n\na\n");
    }
    SUBCASE("appends to the existing text")
    {
        std::string result = "x";
        appendWithLF("y\r\n", result);
        REQUIRE(result == "xy\n");
    }
}

TEST_CASE("appendWithCRLF")
{
    SUBCASE("turns LF, lone CR, and CR LF into CR LF")
    {
        const auto result = toCRLF("a\nb\rc\r\nd");
        REQUIRE(result == "a\r\nb\r\nc\r\nd");
    }
    SUBCASE("keeps text without line breaks")
    {
        const auto result = toCRLF("abc");
        REQUIRE(result == "abc");
    }
}
