// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include <Pomdog/Utility/StringHelper.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::StringHelper;

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
