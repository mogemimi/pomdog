// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Utility/StringHelper.hpp>
#include <gtest/iutest_switch.hpp>

namespace StringHelper = Pomdog::StringHelper;

TEST(StringHelper, FirstCase)
{
    EXPECT_EQ("1 + 2 = 3", StringHelper::Format("%d + %d = %d", 1, 2, 1+2));
    EXPECT_EQ("A + B = AB", StringHelper::Format("%s + %s = %s", "A", "B", "AB"));
    EXPECT_EQ("A + B = AB", StringHelper::Format("%c + %c = %c%c", 'A', 'B', 'A', 'B'));
    EXPECT_EQ("201103", StringHelper::Format("%ld", 201103L));
    EXPECT_EQ(" 6543210", StringHelper::Format("%8d", 6543210));
    EXPECT_EQ("   43210", StringHelper::Format("%8d",   43210));
    EXPECT_EQ("06543210", StringHelper::Format("%08d", 6543210));
    EXPECT_EQ("00043210", StringHelper::Format("%08d",   43210));
    EXPECT_EQ("100", StringHelper::Format("%d", 100));
    EXPECT_EQ("64", StringHelper::Format("%x", 100));
    EXPECT_EQ("144", StringHelper::Format("%o", 100));
    EXPECT_EQ("0x64", StringHelper::Format("%#x", 100));
    EXPECT_EQ("0144", StringHelper::Format("%#o", 100));
    EXPECT_EQ("3.142", StringHelper::Format("%4.3f", 3.14159265f));
    EXPECT_EQ("3.141", StringHelper::Format("%4.3f", 3.1410f));
    EXPECT_EQ("3.142", StringHelper::Format("%4.3lf", 3.14159265));
    EXPECT_EQ("3.141", StringHelper::Format("%4.3lf", 3.1410));
    //EXPECT_EQ("42", StringHelper::Format("%*d", 3, 42));
}

TEST(StringHelper, LongString)
{
    std::string const source(1024, 'A');
    EXPECT_EQ(std::string("42+") + source + "+72", StringHelper::Format("42+%s+72", source.c_str()));
}

TEST(StringHelper, HasPrefix)
{
    EXPECT_TRUE(StringHelper::HasPrefix("", ""));
    EXPECT_TRUE(StringHelper::HasPrefix("a", "a"));
    EXPECT_TRUE(StringHelper::HasPrefix("abc", "abc"));
    EXPECT_TRUE(StringHelper::HasPrefix("abc", "ab"));
    EXPECT_TRUE(StringHelper::HasPrefix("abc", "a"));
    EXPECT_FALSE(StringHelper::HasPrefix("abc", "bc"));
    EXPECT_FALSE(StringHelper::HasPrefix("abc", "b"));
    EXPECT_FALSE(StringHelper::HasPrefix("abc", "c"));
}

TEST(StringHelper, HasSuffix)
{
    EXPECT_TRUE(StringHelper::HasSuffix("", ""));
    EXPECT_TRUE(StringHelper::HasSuffix("a", "a"));
    EXPECT_TRUE(StringHelper::HasSuffix("abc", "abc"));
    EXPECT_TRUE(StringHelper::HasSuffix("abc", "bc"));
    EXPECT_TRUE(StringHelper::HasSuffix("abc", "c"));
    EXPECT_FALSE(StringHelper::HasSuffix("abc", "ab"));
    EXPECT_FALSE(StringHelper::HasSuffix("abc", "a"));
    EXPECT_FALSE(StringHelper::HasSuffix("abc", "b"));
}

TEST(StringHelper, TrimLeft)
{
    EXPECT_EQ("", StringHelper::TrimLeft("", 'a'));
    EXPECT_EQ("BC", StringHelper::TrimLeft("BC", 'a'));
    EXPECT_EQ("BC", StringHelper::TrimLeft("aBC", 'a'));
    EXPECT_EQ("BC", StringHelper::TrimLeft("aaaaaBC", 'a'));
    EXPECT_EQ("BaaCaa", StringHelper::TrimLeft("aaBaaCaa", 'a'));
}

TEST(StringHelper, TrimLeft_Func)
{
    auto func = [](char c)-> bool { return c == 'a'; };
    EXPECT_EQ("", StringHelper::TrimLeft("", func));
    EXPECT_EQ("BC", StringHelper::TrimLeft("BC", func));
    EXPECT_EQ("BC", StringHelper::TrimLeft("aBC", func));
    EXPECT_EQ("BC", StringHelper::TrimLeft("aaaaaBC", func));
    EXPECT_EQ("BaaCaa", StringHelper::TrimLeft("aaBaaCaa", func));

    auto isSpace = [](char c)-> bool { return ::isspace(c); };
    EXPECT_EQ("ABC", StringHelper::TrimLeft("  ABC", isSpace));
    EXPECT_EQ("ABC", StringHelper::TrimLeft(" \t\n \t \nABC", isSpace));
}

TEST(StringHelper, TrimRight)
{
    EXPECT_EQ("", StringHelper::TrimRight("", 'a'));
    EXPECT_EQ("BC", StringHelper::TrimRight("BCa", 'a'));
    EXPECT_EQ("BC", StringHelper::TrimRight("BCaaaaa", 'a'));
    EXPECT_EQ("aaBaaC", StringHelper::TrimRight("aaBaaCaa", 'a'));
}

TEST(StringHelper, TrimRight_Func)
{
    auto func = [](char c)-> bool { return c == 'a'; };
    EXPECT_EQ("", StringHelper::TrimRight("", func));
    EXPECT_EQ("BC", StringHelper::TrimRight("BCa", func));
    EXPECT_EQ("BC", StringHelper::TrimRight("BCaaaaa", func));
    EXPECT_EQ("aaBaaC", StringHelper::TrimRight("aaBaaCaa", func));

    auto isSpace = [](char c)-> bool { return ::isspace(c); };
    EXPECT_EQ("ABC", StringHelper::TrimRight("ABC  ", isSpace));
    EXPECT_EQ("ABC", StringHelper::TrimRight("ABC \t\n \t \n", isSpace));
}
