//
//  Copyright (C) 2013 the Pomdog Engine authors.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <pomdog/Math/Color.hpp>

TEST(FirstTestCase, ColorTest)
{
	Pomdog::Color color{1, 1, 1, 1};
	EXPECT_EQ(color.r, 1.0f);
	EXPECT_EQ(color.g, 1.0f);
	EXPECT_EQ(color.b, 1.0f);
	EXPECT_EQ(color.a, 1.0f);
	
	color = Pomdog::Color{0, 0, 0, 0};
	EXPECT_EQ(color.r, 0.0f);
	EXPECT_EQ(color.g, 0.0f);
	EXPECT_EQ(color.b, 0.0f);
	EXPECT_EQ(color.a, 0.0f);
	
	color = Pomdog::Color{1, 2, 3, 4};
	EXPECT_EQ(color.r, 1.0f);
	EXPECT_EQ(color.g, 2.0f);
	EXPECT_EQ(color.b, 3.0f);
	EXPECT_EQ(color.a, 4.0f);
}

TEST(SecondTestCase, ColorTest)
{
	using Pomdog::Color;
	EXPECT_EQ(Color::Black,  Color(0.0f, 0.0f, 0.0f, 1.0f));
	EXPECT_EQ(Color::White,  Color(1.0f, 1.0f, 1.0f, 1.0f));
	EXPECT_EQ(Color::Red,    Color(1.0f, 0.0f, 0.0f, 1.0f));
	EXPECT_EQ(Color::Green,  Color(0.0f, 1.0f, 0.0f, 1.0f));
	EXPECT_EQ(Color::Blue,   Color(0.0f, 0.0f, 1.0f, 1.0f));
	EXPECT_EQ(Color::Yellow, Color(1.0f, 1.0f, 0.0f, 1.0f));
	EXPECT_EQ(Color::TransparentBlack, Color(0.0f, 0.0f, 0.0f, 0.0f));
	EXPECT_EQ(Color::CornflowerBlue,   Color(100.0f/255, 149.0f/255, 237.0f/255, 1.0f));
}
