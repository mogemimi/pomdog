//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <Pomdog/Math/Color.hpp>

using Pomdog::Color;

TEST(FirstTestCase, ColorTest)
{
	Color color{1, 1, 1, 1};
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
	EXPECT_EQ(Color::Black,  Color(0.0f, 0.0f, 0.0f, 1.0f));
	EXPECT_EQ(Color::White,  Color(1.0f, 1.0f, 1.0f, 1.0f));
	EXPECT_EQ(Color::Red,    Color(1.0f, 0.0f, 0.0f, 1.0f));
	EXPECT_EQ(Color::Green,  Color(0.0f, 1.0f, 0.0f, 1.0f));
	EXPECT_EQ(Color::Blue,   Color(0.0f, 0.0f, 1.0f, 1.0f));
	EXPECT_EQ(Color::Yellow, Color(1.0f, 1.0f, 0.0f, 1.0f));
	EXPECT_EQ(Color::TransparentBlack, Color(0.0f, 0.0f, 0.0f, 0.0f));
	EXPECT_EQ(Color::CornflowerBlue,   Color(100.0f/255, 149.0f/255, 237.0f/255, 1.0f));
}

TEST(DoubleColor, ColorTest)
{
	typedef Pomdog::Details::FloatingPointColor<double> Color;
	EXPECT_EQ(Color::Black,  Color(0, 0, 0, 1));
	EXPECT_EQ(Color::White,  Color(1, 1, 1, 1));
	EXPECT_EQ(Color::Red,    Color(1, 0, 0, 1));
	EXPECT_EQ(Color::Green,  Color(0, 1, 0, 1));
	EXPECT_EQ(Color::Blue,   Color(0, 0, 1, 1));
	EXPECT_EQ(Color::Yellow, Color(1, 1, 0, 1));
	EXPECT_EQ(Color::TransparentBlack, Color(0, 0, 0, 0));
	EXPECT_EQ(Color::CornflowerBlue,   Color(100.0/255, 149.0/255, 237.0/255, 1));
}

TEST(Addition, ColorTest)
{
	EXPECT_EQ(Color(1.f + 5.f, 2.f + 6.f, 3.f + 7.f, 4.f + 8.f), Color(1, 2, 3, 4) + Color(5, 6, 7, 8));
	EXPECT_EQ(Color(1.f - 5.f, 2.f - 6.f, 3.f - 7.f, 4.f - 8.f), Color(1, 2, 3, 4) + Color(-5, -6, -7, -8));
}

TEST(Subtraction, ColorTest)
{
	EXPECT_EQ(Color(1.f - 5.f, 2.f - 6.f, 3.f - 7.f, 4.f - 8.f), Color(1, 2, 3, 4) - Color(5, 6, 7, 8));
	EXPECT_EQ(Color(1.f + 5.f, 2.f + 6.f, 3.f + 7.f, 4.f + 8.f), Color(1, 2, 3, 4) - Color(-5, -6, -7, -8));
}

TEST(Multiply, ColorTest)
{
	EXPECT_EQ(Color(4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f), Color(4, 5, 6, 7) * 3);
	EXPECT_EQ(Color(4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f), 3.0f * Color(4, 5, 6, 7));
}

TEST(Division, ColorTest)
{
	EXPECT_EQ(Color(4.f / 3.f, 5.f / 3.f, 6.f / 3.f, 7.f / 3.f), Color(4, 5, 6, 7) / 3.0f);
}
