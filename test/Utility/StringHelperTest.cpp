// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/StringHelper.hpp"
#include "catch.hpp"

namespace StringHelper = Pomdog::StringHelper;

TEST_CASE("StringHelper", "[StringHelper]")
{
    SECTION("FirstCase")
    {
        REQUIRE(StringHelper::Format("%d + %d = %d", 1, 2, 3) == "1 + 2 = 3");
        REQUIRE(StringHelper::Format("%s + %s = %s", "A", "B", "AB") == "A + B = AB");
        REQUIRE(StringHelper::Format("%c + %c = %c%c", 'A', 'B', 'A', 'B') == "A + B = AB");
        REQUIRE(StringHelper::Format("%ld", 201103L) == "201103");
        REQUIRE(StringHelper::Format("%8d", 6543210) == " 6543210");
        REQUIRE(StringHelper::Format("%8d",   43210) == "   43210");
        REQUIRE(StringHelper::Format("%08d", 6543210) == "06543210");
        REQUIRE(StringHelper::Format("%08d",   43210) == "00043210");
        REQUIRE(StringHelper::Format("%d", 100) == "100");
        REQUIRE(StringHelper::Format("%x", 100) == "64");
        REQUIRE(StringHelper::Format("%o", 100) == "144");
        REQUIRE(StringHelper::Format("%#x", 100) == "0x64");
        REQUIRE(StringHelper::Format("%#o", 100) == "0144");
        REQUIRE(StringHelper::Format("%4.3f", 3.14159265f) == "3.142");
        REQUIRE(StringHelper::Format("%4.3f", 3.1410f) == "3.141");
        REQUIRE(StringHelper::Format("%4.3lf", 3.14159265) == "3.142");
        REQUIRE(StringHelper::Format("%4.3lf", 3.1410) == "3.141");
        // REQUIRE(StringHelper::Format("%*d", 3, 42) == "42");
    }
    SECTION("LongString")
    {
        std::string const source(1024, 'A');
        REQUIRE(StringHelper::Format("42+%s+72", source.c_str()) == std::string{"42+"} + source + "+72");
    }
    SECTION("HasPrefix")
    {
        REQUIRE(StringHelper::HasPrefix("", ""));
        REQUIRE(StringHelper::HasPrefix("a", "a"));
        REQUIRE(StringHelper::HasPrefix("abc", "abc"));
        REQUIRE(StringHelper::HasPrefix("abc", "ab"));
        REQUIRE(StringHelper::HasPrefix("abc", "a"));
        REQUIRE_FALSE(StringHelper::HasPrefix("abc", "bc"));
        REQUIRE_FALSE(StringHelper::HasPrefix("abc", "b"));
        REQUIRE_FALSE(StringHelper::HasPrefix("abc", "c"));
    }
    SECTION("HasSuffix")
    {
        REQUIRE(StringHelper::HasSuffix("", ""));
        REQUIRE(StringHelper::HasSuffix("a", "a"));
        REQUIRE(StringHelper::HasSuffix("abc", "abc"));
        REQUIRE(StringHelper::HasSuffix("abc", "bc"));
        REQUIRE(StringHelper::HasSuffix("abc", "c"));
        REQUIRE_FALSE(StringHelper::HasSuffix("abc", "ab"));
        REQUIRE_FALSE(StringHelper::HasSuffix("abc", "a"));
        REQUIRE_FALSE(StringHelper::HasSuffix("abc", "b"));
    }
    SECTION("TrimLeft")
    {
        REQUIRE(StringHelper::TrimLeft("", 'a') == "");
        REQUIRE(StringHelper::TrimLeft("BC", 'a') == "BC");
        REQUIRE(StringHelper::TrimLeft("aBC", 'a') == "BC");
        REQUIRE(StringHelper::TrimLeft("aaaaaBC", 'a') == "BC");
        REQUIRE(StringHelper::TrimLeft("aaBaaCaa", 'a') == "BaaCaa");
    }
    SECTION("TrimLeft with func")
    {
        auto func = [](char c)-> bool { return c == 'a'; };
        REQUIRE(StringHelper::TrimLeft("", func) == "");
        REQUIRE(StringHelper::TrimLeft("BC", func) == "BC");
        REQUIRE(StringHelper::TrimLeft("aBC", func) == "BC");
        REQUIRE(StringHelper::TrimLeft("aaaaaBC", func) == "BC");
        REQUIRE(StringHelper::TrimLeft("aaBaaCaa", func) == "BaaCaa");

        auto isSpace = [](char c)-> bool { return ::isspace(c); };
        REQUIRE(StringHelper::TrimLeft("  ABC", isSpace) == "ABC");
        REQUIRE(StringHelper::TrimLeft(" \t\n \t \nABC", isSpace) == "ABC");
    }
    SECTION("TrimRight")
    {
        REQUIRE(StringHelper::TrimRight("", 'a') == "");
        REQUIRE(StringHelper::TrimRight("BCa", 'a') == "BC");
        REQUIRE(StringHelper::TrimRight("BCaaaaa", 'a') == "BC");
        REQUIRE(StringHelper::TrimRight("aaBaaCaa", 'a') == "aaBaaC");
    }
    SECTION("TrimRight with func")
    {
        auto func = [](char c)-> bool { return c == 'a'; };
        REQUIRE(StringHelper::TrimRight("", func) == "");
        REQUIRE(StringHelper::TrimRight("BCa", func) == "BC");
        REQUIRE(StringHelper::TrimRight("BCaaaaa", func) == "BC");
        REQUIRE(StringHelper::TrimRight("aaBaaCaa", func) == "aaBaaC");

        auto isSpace = [](char c)-> bool { return ::isspace(c); };
        REQUIRE(StringHelper::TrimRight("ABC  ", isSpace) == "ABC");
        REQUIRE(StringHelper::TrimRight("ABC \t\n \t \n", isSpace) == "ABC");
    }
}
