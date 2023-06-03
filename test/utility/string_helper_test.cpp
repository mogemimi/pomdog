// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/string_helper.h"
#include <catch_amalgamated.hpp>

namespace strings = pomdog::strings;

TEST_CASE("strings", "[strings]")
{
    SECTION("FirstCase")
    {
        REQUIRE(strings::format("%d + %d = %d", 1, 2, 3) == "1 + 2 = 3");
        REQUIRE(strings::format("%s + %s = %s", "A", "B", "AB") == "A + B = AB");
        REQUIRE(strings::format("%c + %c = %c%c", 'A', 'B', 'A', 'B') == "A + B = AB");
        REQUIRE(strings::format("%ld", 201103L) == "201103");
        REQUIRE(strings::format("%8d", 6543210) == " 6543210");
        REQUIRE(strings::format("%8d", 43210) == "   43210");
        REQUIRE(strings::format("%08d", 6543210) == "06543210");
        REQUIRE(strings::format("%08d", 43210) == "00043210");
        REQUIRE(strings::format("%d", 100) == "100");
        REQUIRE(strings::format("%x", 100) == "64");
        REQUIRE(strings::format("%o", 100) == "144");
        REQUIRE(strings::format("%#x", 100) == "0x64");
        REQUIRE(strings::format("%#o", 100) == "0144");
        REQUIRE(strings::format("%4.3f", 3.14159265f) == "3.142");
        REQUIRE(strings::format("%4.3f", 3.1410f) == "3.141");
        REQUIRE(strings::format("%4.3lf", 3.14159265) == "3.142");
        REQUIRE(strings::format("%4.3lf", 3.1410) == "3.141");
        // REQUIRE(strings::format("%*d", 3, 42) == "42");
    }
    SECTION("LongString")
    {
        std::string const source(1024, 'A');
        REQUIRE(strings::format("42+%s+72", source.c_str()) == std::string{"42+"} + source + "+72");
    }
    SECTION("HasPrefix")
    {
        REQUIRE(strings::hasPrefix("", ""));
        REQUIRE(strings::hasPrefix("a", "a"));
        REQUIRE(strings::hasPrefix("abc", "abc"));
        REQUIRE(strings::hasPrefix("abc", "ab"));
        REQUIRE(strings::hasPrefix("abc", "a"));
        REQUIRE_FALSE(strings::hasPrefix("abc", "bc"));
        REQUIRE_FALSE(strings::hasPrefix("abc", "b"));
        REQUIRE_FALSE(strings::hasPrefix("abc", "c"));
    }
    SECTION("HasSuffix")
    {
        REQUIRE(strings::hasSuffix("", ""));
        REQUIRE(strings::hasSuffix("a", "a"));
        REQUIRE(strings::hasSuffix("abc", "abc"));
        REQUIRE(strings::hasSuffix("abc", "bc"));
        REQUIRE(strings::hasSuffix("abc", "c"));
        REQUIRE_FALSE(strings::hasSuffix("abc", "ab"));
        REQUIRE_FALSE(strings::hasSuffix("abc", "a"));
        REQUIRE_FALSE(strings::hasSuffix("abc", "b"));
    }
    SECTION("TrimLeft")
    {
        REQUIRE(strings::trimLeft("", 'a') == "");
        REQUIRE(strings::trimLeft("a", 'a') == "");
        REQUIRE(strings::trimLeft("aa", 'a') == "");
        REQUIRE(strings::trimLeft("aaa", 'a') == "");
        REQUIRE(strings::trimLeft("BC", 'a') == "BC");
        REQUIRE(strings::trimLeft("aBC", 'a') == "BC");
        REQUIRE(strings::trimLeft("aaaaaBC", 'a') == "BC");
        REQUIRE(strings::trimLeft("aaBaaCaa", 'a') == "BaaCaa");
        REQUIRE(strings::trimLeft("aaaaBaaaCaa", 'a') == "BaaaCaa");
    }
    SECTION("TrimLeft with func")
    {
        auto func = [](char c) -> bool { return c == 'a'; };
        REQUIRE(strings::trimLeft("", func) == "");
        REQUIRE(strings::trimLeft("a", func) == "");
        REQUIRE(strings::trimLeft("aa", func) == "");
        REQUIRE(strings::trimLeft("aaa", func) == "");
        REQUIRE(strings::trimLeft("BC", func) == "BC");
        REQUIRE(strings::trimLeft("aBC", func) == "BC");
        REQUIRE(strings::trimLeft("aaaaaBC", func) == "BC");
        REQUIRE(strings::trimLeft("aaBaaCaa", func) == "BaaCaa");
        REQUIRE(strings::trimLeft("aaaaBaaaCaa", func) == "BaaaCaa");

        auto isSpace = [](char c) -> bool { return ::isspace(c); };
        REQUIRE(strings::trimLeft("  ABC", isSpace) == "ABC");
        REQUIRE(strings::trimLeft(" \t\n \t \nABC", isSpace) == "ABC");
    }
    SECTION("TrimRight")
    {
        REQUIRE(strings::trimRight("", 'a') == "");
        REQUIRE(strings::trimRight("a", 'a') == "");
        REQUIRE(strings::trimRight("aa", 'a') == "");
        REQUIRE(strings::trimRight("aaa", 'a') == "");
        REQUIRE(strings::trimRight("BCa", 'a') == "BC");
        REQUIRE(strings::trimRight("BCaaaaa", 'a') == "BC");
        REQUIRE(strings::trimRight("aaBaaCaa", 'a') == "aaBaaC");
        REQUIRE(strings::trimRight("aaaaBaaaCaa", 'a') == "aaaaBaaaC");
    }
    SECTION("TrimRight with func")
    {
        auto func = [](char c) -> bool { return c == 'a'; };
        REQUIRE(strings::trimRight("", func) == "");
        REQUIRE(strings::trimRight("a", func) == "");
        REQUIRE(strings::trimRight("aa", func) == "");
        REQUIRE(strings::trimRight("aaa", func) == "");
        REQUIRE(strings::trimRight("BCa", func) == "BC");
        REQUIRE(strings::trimRight("BCaaaaa", func) == "BC");
        REQUIRE(strings::trimRight("aaBaaCaa", func) == "aaBaaC");
        REQUIRE(strings::trimRight("aaaaBaaaCaa", func) == "aaaaBaaaC");

        auto isSpace = [](char c) -> bool { return ::isspace(c); };
        REQUIRE(strings::trimRight("ABC  ", isSpace) == "ABC");
        REQUIRE(strings::trimRight("ABC \t\n \t \n", isSpace) == "ABC");
    }
    SECTION("ReplaceAll")
    {
        REQUIRE(strings::replaceAll("", "", "") == "");
        REQUIRE(strings::replaceAll("", "a", "") == "");
        REQUIRE(strings::replaceAll("", "", "a") == "");
        REQUIRE(strings::replaceAll("a", "", "") == "a");
        REQUIRE(strings::replaceAll("a", "a", "") == "");
        REQUIRE(strings::replaceAll("aa", "a", "") == "");
        REQUIRE(strings::replaceAll("a", "aa", "") == "a");
        REQUIRE(strings::replaceAll("aa", "aa", "") == "");
        REQUIRE(strings::replaceAll("/abc/abc/abc/", "/", "") == "abcabcabc");
        REQUIRE(strings::replaceAll("/abc/abc/abc/", "/", ".") == ".abc.abc.abc.");
    }
}
