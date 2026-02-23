// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <ostream>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using pomdog::strings::hasPrefix;
using pomdog::strings::hasSuffix;
using pomdog::strings::replaceAll;
using pomdog::strings::split;
using pomdog::strings::trimLeft;
using pomdog::strings::trimRight;

TEST_CASE("strings")
{
    SUBCASE("hasPrefix")
    {
        REQUIRE(hasPrefix("", ""));
        REQUIRE(hasPrefix("a", "a"));
        REQUIRE(hasPrefix("hello", ""));
        REQUIRE(hasPrefix("hello", "h"));
        REQUIRE(hasPrefix("hello", "hel"));
        REQUIRE(hasPrefix("hello", "hello"));
        REQUIRE_FALSE(hasPrefix("hello", "world"));
        REQUIRE_FALSE(hasPrefix("hello", "ello"));
        REQUIRE_FALSE(hasPrefix("hello", "helloo"));
        REQUIRE_FALSE(hasPrefix("hello", "ell"));
        REQUIRE_FALSE(hasPrefix("hello", "o"));
        REQUIRE_FALSE(hasPrefix("", "a"));
    }
    SUBCASE("hasSuffix")
    {
        REQUIRE(hasSuffix("", ""));
        REQUIRE(hasSuffix("a", "a"));
        REQUIRE(hasSuffix("hello", ""));
        REQUIRE(hasSuffix("hello", "o"));
        REQUIRE(hasSuffix("hello", "llo"));
        REQUIRE(hasSuffix("hello", "hello"));
        REQUIRE_FALSE(hasSuffix("hello", "world"));
        REQUIRE_FALSE(hasSuffix("hello", "hel"));
        REQUIRE_FALSE(hasSuffix("hello", "hhello"));
        REQUIRE_FALSE(hasSuffix("hello", "h"));
        REQUIRE_FALSE(hasSuffix("hello", "ell"));
        REQUIRE_FALSE(hasSuffix("", "a"));
    }
    SUBCASE("trimLeft")
    {
        REQUIRE(trimLeft("", 'x') == "");
        REQUIRE(trimLeft("a", 'a') == "");
        REQUIRE(trimLeft("hello", 'x') == "hello");
        REQUIRE(trimLeft("hello", 'h') == "ello");
        REQUIRE(trimLeft("hhello", 'h') == "ello");
        REQUIRE(trimLeft("hhhello", 'h') == "ello");
        REQUIRE(trimLeft("hhh", 'h') == "");
        REQUIRE(trimLeft("aaaaBaaaCaa", 'a') == "BaaaCaa");
    }
    SUBCASE("trimLeft with func")
    {
        auto func = [](char c) -> bool { return c == 'a'; };
        REQUIRE(trimLeft("", func) == "");
        REQUIRE(trimLeft("a", func) == "");
        REQUIRE(trimLeft("aa", func) == "");
        REQUIRE(trimLeft("aaa", func) == "");
        REQUIRE(trimLeft("BC", func) == "BC");
        REQUIRE(trimLeft("aBC", func) == "BC");
        REQUIRE(trimLeft("aaaaaBC", func) == "BC");
        REQUIRE(trimLeft("aaBaaCaa", func) == "BaaCaa");
        REQUIRE(trimLeft("aaaaBaaaCaa", func) == "BaaaCaa");

        auto isSpace = [](char c) -> bool { return c == ' ' || c == '\t' || c == '\n'; };
        REQUIRE(trimLeft("  ABC", isSpace) == "ABC");
        REQUIRE(trimLeft(" \t\n \t \nABC", isSpace) == "ABC");
    }
    SUBCASE("trimRight")
    {
        REQUIRE(trimRight("", 'x') == "");
        REQUIRE(trimRight("a", 'a') == "");
        REQUIRE(trimRight("hello", 'x') == "hello");
        REQUIRE(trimRight("hello", 'o') == "hell");
        REQUIRE(trimRight("helloo", 'o') == "hell");
        REQUIRE(trimRight("hellooo", 'o') == "hell");
        REQUIRE(trimRight("ooo", 'o') == "");
        REQUIRE(trimRight("aaaaBaaaCaa", 'a') == "aaaaBaaaC");
    }
    SUBCASE("trimRight with func")
    {
        auto func = [](char c) -> bool { return c == 'a'; };
        REQUIRE(trimRight("", func) == "");
        REQUIRE(trimRight("a", func) == "");
        REQUIRE(trimRight("aa", func) == "");
        REQUIRE(trimRight("aaa", func) == "");
        REQUIRE(trimRight("BC", func) == "BC");
        REQUIRE(trimRight("BCa", func) == "BC");
        REQUIRE(trimRight("BCaaaaa", func) == "BC");
        REQUIRE(trimRight("aaBaaCaa", func) == "aaBaaC");
        REQUIRE(trimRight("aaaaBaaaCaa", func) == "aaaaBaaaC");

        auto isSpace = [](char c) -> bool { return c == ' ' || c == '\t' || c == '\n'; };
        REQUIRE(trimRight("ABC  ", isSpace) == "ABC");
        REQUIRE(trimRight("ABC \t\n \t \n", isSpace) == "ABC");
    }
    SUBCASE("replaceAll")
    {
        REQUIRE(replaceAll("", "", "") == "");
        REQUIRE(replaceAll("", "a", "") == "");
        REQUIRE(replaceAll("", "", "a") == "");
        REQUIRE(replaceAll("a", "", "") == "a");
        REQUIRE(replaceAll("a", "a", "") == "");
        REQUIRE(replaceAll("aa", "a", "") == "");
        REQUIRE(replaceAll("a", "aa", "") == "a");
        REQUIRE(replaceAll("aa", "aa", "") == "");
        REQUIRE(replaceAll("hello", "", "") == "hello");
        REQUIRE(replaceAll("hello", "l", "L") == "heLLo");
        REQUIRE(replaceAll("hello", "ll", "LL") == "heLLo");
        REQUIRE(replaceAll("hello", "hello", "world") == "world");
        REQUIRE(replaceAll("hello hello", "hello", "hi") == "hi hi");
        REQUIRE(replaceAll("aaa", "a", "bb") == "bbbbbb");
    }
    SUBCASE("split")
    {
        auto result = split("", ',');
        REQUIRE(result.size() == 1);
        REQUIRE(result[0] == "");

        result = split("hello", ',');
        REQUIRE(result.size() == 1);
        REQUIRE(result[0] == "hello");

        result = split("a,b,c", ',');
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == "a");
        REQUIRE(result[1] == "b");
        REQUIRE(result[2] == "c");

        result = split("a,,c", ',');
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == "a");
        REQUIRE(result[1] == "");
        REQUIRE(result[2] == "c");

        result = split(",a,", ',');
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == "");
        REQUIRE(result[1] == "a");
        REQUIRE(result[2] == "");
    }
}
