//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <gtest/iutest_switch.hpp>
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Vector4.hpp>

using Pomdog::Color;
using Pomdog::Vector3;
using Pomdog::Vector4;

TEST(Color, FirstTestCase)
{
	Color color{255, 255, 255, 255};
	EXPECT_EQ(color.R(), 255);
	EXPECT_EQ(color.G(), 255);
	EXPECT_EQ(color.B(), 255);
	EXPECT_EQ(color.A(), 255);
	
	color = Pomdog::Color{0, 0, 0, 0};
	EXPECT_EQ(color.R(), 0);
	EXPECT_EQ(color.G(), 0);
	EXPECT_EQ(color.B(), 0);
	EXPECT_EQ(color.A(), 0);
	
	color = Pomdog::Color{1, 2, 3, 4};
	EXPECT_EQ(color.R(), 1);
	EXPECT_EQ(color.G(), 2);
	EXPECT_EQ(color.B(), 3);
	EXPECT_EQ(color.A(), 4);
}

TEST(Color, Constants)
{
	EXPECT_EQ(Color::Black,  Color(0, 0, 0, 255));
	EXPECT_EQ(Color::White,  Color(255, 255, 255, 255));
	EXPECT_EQ(Color::Red,    Color(255, 0, 0, 255));
	EXPECT_EQ(Color::Green,  Color(0, 255, 0, 255));
	EXPECT_EQ(Color::Blue,   Color(0, 0, 255, 255));
	EXPECT_EQ(Color::Yellow, Color(255, 255, 0, 255));
	EXPECT_EQ(Color::TransparentBlack, Color(0, 0, 0, 0));
	EXPECT_EQ(Color::CornflowerBlue,   Color(100, 149, 237, 255));
}

TEST(Color, Multiply)
{
	Color color(4, 5, 6, 7);
	color *= 3;
	EXPECT_EQ(Color(4*3, 5*3, 6*3, 7*3), color);

	EXPECT_EQ(Color(4*3, 5*3, 6*3, 7*3), Color(4, 5, 6, 7)*3);
	EXPECT_EQ(Color(4*3, 5*3, 6*3, 7*3), 3*Color(4, 5, 6, 7));
}

TEST(Color, ToVector)
{
	Color color(255, 255, 255, 255);
	EXPECT_EQ(Vector3(1.0f, 1.0f, 1.0f), color.ToVector3());
	EXPECT_EQ(Vector4(1.0f, 1.0f, 1.0f, 1.0f), color.ToVector4());
	
	color = Color(0, 0, 0, 0);
	EXPECT_EQ(Vector3(0.0f, 0.0f, 0.0f), color.ToVector3());
	EXPECT_EQ(Vector4(0.0f, 0.0f, 0.0f, 0.0f), color.ToVector4());
	
	color = Color(12, 34, 56, 78);
	EXPECT_NEAR(12.0f/255.0f, color.ToVector3().X, std::numeric_limits<float>::epsilon());
	EXPECT_NEAR(34.0f/255.0f, color.ToVector3().Y, std::numeric_limits<float>::epsilon());
	EXPECT_NEAR(56.0f/255.0f, color.ToVector3().Z, std::numeric_limits<float>::epsilon());
	EXPECT_NEAR(12.0f/255.0f, color.ToVector4().X, std::numeric_limits<float>::epsilon());
	EXPECT_NEAR(34.0f/255.0f, color.ToVector4().Y, std::numeric_limits<float>::epsilon());
	EXPECT_NEAR(56.0f/255.0f, color.ToVector4().Z, std::numeric_limits<float>::epsilon());
	EXPECT_NEAR(78.0f/255.0f, color.ToVector4().W, std::numeric_limits<float>::epsilon());
}

TEST(Color, FromVector)
{
	EXPECT_EQ(Color(0, 0, 0, 255), Color(Vector3{0.0f, 0.0f, 0.0f}));
	EXPECT_EQ(Color(255, 255, 255, 255), Color(Vector3{1.0f, 1.0f, 1.0f}));
	
	EXPECT_EQ(Color(0, 0, 0, 0), Color(Vector4{0.0f, 0.0f, 0.0f, 0.0f}));
	EXPECT_EQ(Color(255, 255, 255, 255), Color(Vector4{1.0f, 1.0f, 1.0f, 1.0f}));
	
	EXPECT_EQ(Color(255, 0, 0, 255), Color(Vector3{1.0f, 0.0f, 0.0f}));
	EXPECT_EQ(Color(0, 255, 0, 255), Color(Vector3{0.0f, 1.0f, 0.0f}));
	EXPECT_EQ(Color(0, 0, 255, 255), Color(Vector3{0.0f, 0.0f, 1.0f}));
	
	EXPECT_EQ(Color(255, 0, 0, 0), Color(Vector4{1.0f, 0.0f, 0.0f, 0.0f}));
	EXPECT_EQ(Color(0, 255, 0, 0), Color(Vector4{0.0f, 1.0f, 0.0f, 0.0f}));
	EXPECT_EQ(Color(0, 0, 255, 0), Color(Vector4{0.0f, 0.0f, 1.0f, 0.0f}));
	EXPECT_EQ(Color(0, 0, 0, 255), Color(Vector4{0.0f, 0.0f, 0.0f, 1.0f}));

	EXPECT_NEAR(0.3f*255, Color(Vector3{0.3f, 0.4f, 0.5f}).R(), 0.5f);
	EXPECT_NEAR(0.4f*255, Color(Vector3{0.3f, 0.4f, 0.5f}).G(), 0.5f);
	EXPECT_NEAR(0.5f*255, Color(Vector3{0.3f, 0.4f, 0.5f}).B(), 0.5f);
	
	EXPECT_NEAR(0.3f*255, Color(Vector4{0.3f, 0.4f, 0.5f, 0.6f}).R(), 0.5f);
	EXPECT_NEAR(0.4f*255, Color(Vector4{0.3f, 0.4f, 0.5f, 0.6f}).G(), 0.5f);
	EXPECT_NEAR(0.5f*255, Color(Vector4{0.3f, 0.4f, 0.5f, 0.6f}).B(), 0.5f);
	EXPECT_NEAR(0.6f*255, Color(Vector4{0.3f, 0.4f, 0.5f, 0.6f}).A(), 0.5f);
}
