//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <pomdog/Math/Coordinate3D.hpp>

TEST(FirstTestCase, Coordinate3DTest)
{
	Pomdog::Coordinate3D<int> coodinate{0, 0, 0};
	EXPECT_EQ(coodinate.x, 0);
	EXPECT_EQ(coodinate.y, 0);
	EXPECT_EQ(coodinate.z, 0);
	
	coodinate = {1, 2, 3};
	EXPECT_EQ(coodinate.x, 1);
	EXPECT_EQ(coodinate.y, 2);
	EXPECT_EQ(coodinate.z, 3);
	
	coodinate = {
		std::numeric_limits<int>::max(),
		std::numeric_limits<int>::min(),
		std::numeric_limits<int>::denorm_min()
	};
	EXPECT_EQ(coodinate.x, std::numeric_limits<int>::max());
	EXPECT_EQ(coodinate.y, std::numeric_limits<int>::min());
	EXPECT_EQ(coodinate.z, std::numeric_limits<int>::denorm_min());
}

TEST(Addition, Coordinate3DTest)
{
	typedef Pomdog::Coordinate3D<int> Coordinate3D;
	
	EXPECT_EQ(Coordinate3D(0, 0, 0), Coordinate3D(0, 0, 0) + Coordinate3D(0, 0, 0));
	EXPECT_EQ(Coordinate3D(7, 9, 11), Coordinate3D(2, 3, 4) + Coordinate3D(5, 6, 7));
}

TEST(Subtraction, Coordinate3DTest)
{
	typedef Pomdog::Coordinate3D<int> Coordinate3D;
	
	EXPECT_EQ(Coordinate3D(0, 0, 0), Coordinate3D(0, 0, 0) - Coordinate3D(0, 0, 0));
	EXPECT_EQ(Coordinate3D(2 - 5, 3 - 6, 4 - 7), Coordinate3D(2, 3, 4) - Coordinate3D(5, 6, 7));
}

TEST(Multiply, Coordinate3DTest)
{
	typedef Pomdog::Coordinate3D<int> Coordinate3D;
	Coordinate3D const coordinate{3, 4, 5};
	
	EXPECT_EQ(Coordinate3D(9, 16, 25), coordinate * coordinate);
	EXPECT_EQ(Coordinate3D(6, 8, 10), coordinate * 2);
	EXPECT_EQ(Coordinate3D(6, 8, 10), 2 * coordinate);
}

TEST(Division, Coordinate3DTest)
{
	typedef Pomdog::Coordinate3D<int> Coordinate3D;
	Coordinate3D const coordinate{6, 8, 10};
	
	EXPECT_EQ(Coordinate3D(1, 1, 1), coordinate / coordinate);
	EXPECT_EQ(Coordinate3D(3, 4, 5), coordinate / 2);
}
