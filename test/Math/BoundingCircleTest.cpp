// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/BoundingCircle.hpp>
#include <Pomdog/Math/BoundingBox2D.hpp>
#include <Pomdog/Math/ContainmentType.hpp>
#include <gtest/iutest_switch.hpp>

using namespace Pomdog;

TEST(BoundingCircle, Constructors)
{
    BoundingCircle circle;

    BoundingCircle circle2;
    circle2.Center = Vector2::Zero;
    circle2.Radius = 42.0f;

    circle = circle2;
    EXPECT_EQ(Vector2::Zero, circle.Center);
    EXPECT_EQ(42.0f, circle.Radius);

    BoundingCircle circle3 {Vector2{3.14f, 2.718f}, 17.0f};
    circle = circle3;
    EXPECT_EQ(Vector2(3.14f, 2.718f), circle.Center);
    EXPECT_EQ(Vector2(3.14f, 2.718f), circle3.Center);
    EXPECT_EQ(17.0f, circle.Radius);
    EXPECT_EQ(17.0f, circle3.Radius);
}

TEST(BoundingCircle, Contains_Vector2)
{
    BoundingCircle circle;
    circle.Center = Vector2::Zero;
    circle.Radius = 42.0f;

    EXPECT_EQ(ContainmentType::Contains, circle.Contains(Vector2::Zero));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(Vector2{41.f, 0.f}));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(Vector2{0.f, 41.f}));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(Vector2{-41.f, 0.f}));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(Vector2{0.f, -41.f}));

    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(Vector2{42.f, 0.f}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(Vector2{0.f, 42.f}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(Vector2{-42.f, 0.f}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(Vector2{0.f, -42.f}));

    EXPECT_EQ(ContainmentType::Disjoint, circle.Contains(Vector2{43.f, 0.f}));
    EXPECT_EQ(ContainmentType::Disjoint, circle.Contains(Vector2{0.f, 43.f}));
    EXPECT_EQ(ContainmentType::Disjoint, circle.Contains(Vector2{-43.f, 0.f}));
    EXPECT_EQ(ContainmentType::Disjoint, circle.Contains(Vector2{0.f, -43.f}));
}

TEST(BoundingCircle, Contains_BoundingCircle)
{
    BoundingCircle circle;
    circle.Center = Vector2::Zero;
    circle.Radius = 42.0f;

    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(circle));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingCircle{Vector2::Zero, 43.0f}));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(BoundingCircle{Vector2::Zero, 41.0f}));

    EXPECT_EQ(ContainmentType::Contains, circle.Contains(BoundingCircle{Vector2{40.f, 0.f}, 1.9f}));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(BoundingCircle{Vector2{0.f, 40.f}, 1.9f}));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(BoundingCircle{Vector2{-40.f, 0.f}, 1.9f}));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(BoundingCircle{Vector2{0.f, -40.f}, 1.9f}));

    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingCircle{Vector2{42.f, 0.f}, 1.0f}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingCircle{Vector2{0.f, 42.f}, 1.0f}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingCircle{Vector2{-42.f, 0.f}, 1.0f}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingCircle{Vector2{0.f, -42.f}, 1.0f}));

    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingCircle{Vector2{43.f, 0.f}, 2.0f}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingCircle{Vector2{0.f, 43.f}, 2.0f}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingCircle{Vector2{-43.f, 0.f}, 2.0f}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingCircle{Vector2{0.f, -43.f}, 2.0f}));

    EXPECT_EQ(ContainmentType::Disjoint, circle.Contains(BoundingCircle{Vector2{43.f, 0.f}, 0.5f}));
    EXPECT_EQ(ContainmentType::Disjoint, circle.Contains(BoundingCircle{Vector2{0.f, 43.f}, 0.5f}));
    EXPECT_EQ(ContainmentType::Disjoint, circle.Contains(BoundingCircle{Vector2{-43.f, 0.f}, 0.5f}));
    EXPECT_EQ(ContainmentType::Disjoint, circle.Contains(BoundingCircle{Vector2{0.f, -43.f}, 0.5f}));
}

TEST(BoundingCircle, Contains_BoundingBox2D)
{
    BoundingCircle circle;
    circle.Center = Vector2::Zero;
    circle.Radius = 42.0f;

    auto min = Vector2::Normalize({-1.f, -1.f}) * 42.0f;
    auto max = Vector2::Normalize({1.f, 1.f}) * 42.0f;
    auto unit = Vector2::Normalize({1.f, 1.f}) * 42.0f;

    EXPECT_EQ(ContainmentType::Contains, circle.Contains(BoundingBox2D{Vector2::Zero, max}));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(BoundingBox2D{Vector2::Zero - unit * Vector2(1, 0), max - unit * Vector2(1, 0)}));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(BoundingBox2D{Vector2::Zero - unit * Vector2(0, 1), max - unit * Vector2(0, 1)}));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(BoundingBox2D{min, Vector2::Zero}));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(BoundingBox2D{min + unit * Vector2(1, 0), Vector2::Zero + unit * Vector2(1, 0)}));
    EXPECT_EQ(ContainmentType::Contains, circle.Contains(BoundingBox2D{min + unit * Vector2(0, 1), Vector2::Zero + unit * Vector2(0, 1)}));

    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingBox2D{Vector2::Zero, max * 1.01f}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingBox2D{Vector2::Zero - unit * Vector2(1, 0), max * 1.01f - unit * Vector2(1, 0)}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingBox2D{Vector2::Zero - unit * Vector2(0, 1), max * 1.01f - unit * Vector2(0, 1)}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingBox2D{min * 1.01f, Vector2::Zero}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingBox2D{min * 1.01f + unit * Vector2(1, 0), Vector2::Zero + unit * Vector2(1, 0)}));
    EXPECT_EQ(ContainmentType::Intersects, circle.Contains(BoundingBox2D{min * 1.01f + unit * Vector2(0, 1), Vector2::Zero + unit * Vector2(0, 1)}));

    EXPECT_EQ(ContainmentType::Disjoint, circle.Contains(BoundingBox2D{max * 1.01f, max * 1.01f + unit}));
}

TEST(BoundingCircle, Intersects_BoundingBox2D)
{
    BoundingCircle circle;
    circle.Center = Vector2::Zero;
    circle.Radius = 42.0f;

    auto min = Vector2::Normalize({-1.f, -1.f}) * 42.0f;
    auto max = Vector2::Normalize({1.f, 1.f}) * 42.0f;
    auto unit = Vector2::Normalize({1.f, 1.f}) * 42.0f;

    EXPECT_TRUE(circle.Intersects(BoundingBox2D{Vector2::Zero, max}));
    EXPECT_TRUE(circle.Intersects(BoundingBox2D{Vector2::Zero - unit * Vector2(1, 0), max - unit * Vector2(1, 0)}));
    EXPECT_TRUE(circle.Intersects(BoundingBox2D{Vector2::Zero - unit * Vector2(0, 1), max - unit * Vector2(0, 1)}));
    EXPECT_TRUE(circle.Intersects(BoundingBox2D{min, Vector2::Zero}));
    EXPECT_TRUE(circle.Intersects(BoundingBox2D{min + unit * Vector2(1, 0), Vector2::Zero + unit * Vector2(1, 0)}));
    EXPECT_TRUE(circle.Intersects(BoundingBox2D{min + unit * Vector2(0, 1), Vector2::Zero + unit * Vector2(0, 1)}));

    EXPECT_TRUE(circle.Intersects(BoundingBox2D{Vector2::Zero, max * 1.01f}));
    EXPECT_TRUE(circle.Intersects(BoundingBox2D{Vector2::Zero - unit * Vector2(1, 0), max * 1.01f - unit * Vector2(1, 0)}));
    EXPECT_TRUE(circle.Intersects(BoundingBox2D{Vector2::Zero - unit * Vector2(0, 1), max * 1.01f - unit * Vector2(0, 1)}));
    EXPECT_TRUE(circle.Intersects(BoundingBox2D{min * 1.01f, Vector2::Zero}));
    EXPECT_TRUE(circle.Intersects(BoundingBox2D{min * 1.01f + unit * Vector2(1, 0), Vector2::Zero + unit * Vector2(1, 0)}));
    EXPECT_TRUE(circle.Intersects(BoundingBox2D{min * 1.01f + unit * Vector2(0, 1), Vector2::Zero + unit * Vector2(0, 1)}));

    EXPECT_FALSE(circle.Intersects(BoundingBox2D{max * 1.01f, max * 1.01f + unit}));
}

TEST(BoundingCircle, Intersects_BoundingCircle)
{
    BoundingCircle circle;
    circle.Center = Vector2::Zero;
    circle.Radius = 42.0f;

    EXPECT_TRUE(circle.Intersects(circle));
    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2::Zero, 43.0f}));
    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2::Zero, 41.0f}));

    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2{40.f, 0.f}, 1.9f}));
    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2{0.f, 40.f}, 1.9f}));
    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2{-40.f, 0.f}, 1.9f}));
    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2{0.f, -40.f}, 1.9f}));

    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2{42.f, 0.f}, 1.0f}));
    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2{0.f, 42.f}, 1.0f}));
    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2{-42.f, 0.f}, 1.0f}));
    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2{0.f, -42.f}, 1.0f}));

    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2{43.f, 0.f}, 2.0f}));
    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2{0.f, 43.f}, 2.0f}));
    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2{-43.f, 0.f}, 2.0f}));
    EXPECT_TRUE(circle.Intersects(BoundingCircle{Vector2{0.f, -43.f}, 2.0f}));

    EXPECT_FALSE(circle.Intersects(BoundingCircle{Vector2{43.f, 0.f}, 0.5f}));
    EXPECT_FALSE(circle.Intersects(BoundingCircle{Vector2{0.f, 43.f}, 0.5f}));
    EXPECT_FALSE(circle.Intersects(BoundingCircle{Vector2{-43.f, 0.f}, 0.5f}));
    EXPECT_FALSE(circle.Intersects(BoundingCircle{Vector2{0.f, -43.f}, 0.5f}));
}
