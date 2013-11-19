//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iutest/gtest/iutest_switch.hpp>
#include <Pomdog/Math/Point3D.hpp>

using Pomdog::Point3D;

TEST(FirstTestCase, Point3DTest)
{
	Point3D coodinate{0, 0, 0};
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

TEST(Addition, Point3DTest)
{
	EXPECT_EQ(Point3D(0, 0, 0), Point3D(0, 0, 0) + Point3D(0, 0, 0));
	EXPECT_EQ(Point3D(7, 9, 11), Point3D(2, 3, 4) + Point3D(5, 6, 7));
}

TEST(Subtraction, Point3DTest)
{
	EXPECT_EQ(Point3D(0, 0, 0), Point3D(0, 0, 0) - Point3D(0, 0, 0));
	EXPECT_EQ(Point3D(2 - 5, 3 - 6, 4 - 7), Point3D(2, 3, 4) - Point3D(5, 6, 7));
}

TEST(Multiply, Point3DTest)
{
	Point3D const coordinate{3, 4, 5};
	
	EXPECT_EQ(Point3D(9, 16, 25), coordinate * coordinate);
	EXPECT_EQ(Point3D(6, 8, 10), coordinate * 2);
	EXPECT_EQ(Point3D(6, 8, 10), 2 * coordinate);
}

TEST(Division, Point3DTest)
{
	Point3D const coordinate{6, 8, 10};
	
	EXPECT_EQ(Point3D(1, 1, 1), coordinate / coordinate);
	EXPECT_EQ(Point3D(3, 4, 5), coordinate / 2);
}
