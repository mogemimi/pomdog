//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <pomdog/Math/Coordinate2D.hpp>

TEST(FirstTestCase, Coordinate2DTest)
{
	Pomdog::Coordinate2D<int> coodinate{0, 0};
	EXPECT_EQ(coodinate.x, 0);
	EXPECT_EQ(coodinate.y, 0);
	
	coodinate = {1, 2};
	EXPECT_EQ(coodinate.x, 1);
	EXPECT_EQ(coodinate.y, 2);
	
	coodinate = {
		std::numeric_limits<int>::max(),
		std::numeric_limits<int>::min()
	};
	EXPECT_EQ(coodinate.x, std::numeric_limits<int>::max());
	EXPECT_EQ(coodinate.y, std::numeric_limits<int>::min());
}

TEST(Addition, Coordinate2DTest)
{
	typedef Pomdog::Coordinate2D<int> Coordinate2D;
	
	EXPECT_EQ(Coordinate2D(0, 0), Coordinate2D(0, 0) + Coordinate2D(0, 0));
	EXPECT_EQ(Coordinate2D(6, 8), Coordinate2D(2, 3) + Coordinate2D(4, 5));
}

TEST(Subtraction, Coordinate2DTest)
{
	typedef Pomdog::Coordinate2D<int> Coordinate2D;
	
	EXPECT_EQ(Coordinate2D(0, 0), Coordinate2D(0, 0) - Coordinate2D(0, 0));
	EXPECT_EQ(Coordinate2D(-4, -5), Coordinate2D(2, 3) - Coordinate2D(6, 8));
	EXPECT_EQ(Coordinate2D(+4, +5), Coordinate2D(6, 8) - Coordinate2D(2, 3));
}

TEST(Multiply, Coordinate2DTest)
{
	typedef Pomdog::Coordinate2D<int> Coordinate2D;
	Coordinate2D const coordinate{3, 4};
	
	EXPECT_EQ(Coordinate2D(9, 16), coordinate * coordinate);
	EXPECT_EQ(Coordinate2D(6, 8), coordinate * 2);
	EXPECT_EQ(Coordinate2D(6, 8), 2 * coordinate);
}

TEST(Division, Coordinate2DTest)
{
	typedef Pomdog::Coordinate2D<int> Coordinate2D;
	Coordinate2D const coordinate{6, 8};
	
	EXPECT_EQ(Coordinate2D(1, 1), coordinate / coordinate);
	EXPECT_EQ(Coordinate2D(3, 4), coordinate / 2);
}
