//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Utility/StringFormat.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::StringFormat;

TEST(StringFormat, FirstCase)
{
	EXPECT_EQ("1 + 2 = 3", StringFormat("%d + %d = %d", 1, 2, 1+2));
	EXPECT_EQ("A + B = AB", StringFormat("%s + %s = %s", "A", "B", "AB"));
	EXPECT_EQ("A + B = AB", StringFormat("%c + %c = %c%c", 'A', 'B', 'A', 'B'));
	EXPECT_EQ("201103", StringFormat("%ld", 201103L));
	EXPECT_EQ(" 6543210", StringFormat("%8d", 6543210));
	EXPECT_EQ("   43210", StringFormat("%8d",   43210));
	EXPECT_EQ("06543210", StringFormat("%08d", 6543210));
	EXPECT_EQ("00043210", StringFormat("%08d",   43210));
	EXPECT_EQ("100", StringFormat("%d", 100));
	EXPECT_EQ("64", StringFormat("%x", 100));
	EXPECT_EQ("144", StringFormat("%o", 100));
	EXPECT_EQ("0x64", StringFormat("%#x", 100));
	EXPECT_EQ("0144", StringFormat("%#o", 100));
	EXPECT_EQ("3.142", StringFormat("%4.3f", 3.14159265f));
	EXPECT_EQ("3.141", StringFormat("%4.3f", 3.1410f));
	EXPECT_EQ("3.142", StringFormat("%4.3lf", 3.14159265));
	EXPECT_EQ("3.141", StringFormat("%4.3lf", 3.1410));
	//EXPECT_EQ("42", StringFormat("%*d", 3, 42));
}

TEST(StringFormat, LongString)
{
	std::string const source(1024, 'A');
	EXPECT_EQ(std::string("42+") + source + "+72", StringFormat("42+%s+72", source.c_str()));
}
