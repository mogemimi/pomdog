// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/BoundingSphere.hpp>
#include <Pomdog/Math/BoundingBox.hpp>
#include <Pomdog/Math/ContainmentType.hpp>
#include <gtest/iutest_switch.hpp>

using namespace Pomdog;

TEST(BoundingSphere, Constructors)
{
    BoundingSphere sphere;

    BoundingSphere sphere2;
    sphere2.Center = Vector3::Zero;
    sphere2.Radius = 42.0f;

    sphere = sphere2;
    EXPECT_EQ(Vector3::Zero, sphere.Center);
    EXPECT_EQ(42.0f, sphere.Radius);

    BoundingSphere sphere3 {Vector3::UnitX, 17.0f};
    sphere = sphere3;
    EXPECT_EQ(Vector3::UnitX, sphere.Center);
    EXPECT_EQ(Vector3::UnitX, sphere3.Center);
    EXPECT_EQ(17.0f, sphere.Radius);
    EXPECT_EQ(17.0f, sphere3.Radius);
}

TEST(BoundingSphere, Contains_Vector3)
{
    BoundingSphere sphere;
    sphere.Center = Vector3::Zero;
    sphere.Radius = 42.0f;

    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(Vector3::Zero));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(Vector3{41.f, 0.f, 0.f}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(Vector3{0.f, 41.f, 0.f}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(Vector3{0.f, 0.f, 41.f}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(Vector3{-41.f, 0.f, 0.f}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(Vector3{0.f, -41.f, 0.f}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(Vector3{0.f, 0.f, -41.f}));

    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(Vector3{42.f, 0.f, 0.f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(Vector3{0.f, 42.f, 0.f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(Vector3{0.f, 0.f, 42.f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(Vector3{-42.f, 0.f, 0.f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(Vector3{0.f, -42.f, 0.f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(Vector3{0.f, 0.f, -42.f}));

    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(Vector3{43.f, 0.f, 0.f}));
    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(Vector3{0.f, 43.f, 0.f}));
    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(Vector3{0.f, 0.f, 43.f}));
    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(Vector3{-43.f, 0.f, 0.f}));
    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(Vector3{0.f, -43.f, 0.f}));
    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(Vector3{0.f, 0.f, -43.f}));
}

TEST(BoundingSphere, Contains_BoundingSphere)
{
    BoundingSphere sphere;
    sphere.Center = Vector3::Zero;
    sphere.Radius = 42.0f;

    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(sphere));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3::Zero, 43.0f}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingSphere{Vector3::Zero, 41.0f}));

    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingSphere{Vector3{40.f, 0.f, 0.f}, 1.9f}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingSphere{Vector3{0.f, 40.f, 0.f}, 1.9f}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, 40.f}, 1.9f}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingSphere{Vector3{-40.f, 0.f, 0.f}, 1.9f}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingSphere{Vector3{0.f, -40.f, 0.f}, 1.9f}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, -40.f}, 1.9f}));

    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3{42.f, 0.f, 0.f}, 1.0f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3{0.f, 42.f, 0.f}, 1.0f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, 42.f}, 1.0f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3{-42.f, 0.f, 0.f}, 1.0f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3{0.f, -42.f, 0.f}, 1.0f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, -42.f}, 1.0f}));

    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3{43.f, 0.f, 0.f}, 2.0f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3{0.f, 43.f, 0.f}, 2.0f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, 43.f}, 2.0f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3{-43.f, 0.f, 0.f}, 2.0f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3{0.f, -43.f, 0.f}, 2.0f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, -43.f}, 2.0f}));

    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(BoundingSphere{Vector3{43.f, 0.f, 0.f}, 0.5f}));
    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(BoundingSphere{Vector3{0.f, 43.f, 0.f}, 0.5f}));
    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, 43.f}, 0.5f}));
    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(BoundingSphere{Vector3{-43.f, 0.f, 0.f}, 0.5f}));
    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(BoundingSphere{Vector3{0.f, -43.f, 0.f}, 0.5f}));
    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, -43.f}, 0.5f}));
}

TEST(BoundingSphere, Contains_BoundingBox)
{
    BoundingSphere sphere;
    sphere.Center = Vector3::Zero;
    sphere.Radius = 42.0f;

    auto min = Vector3::Normalize({-1.f, -1.f, -1.f}) * 42.0f;
    auto max = Vector3::Normalize({1.f, 1.f, 1.f}) * 42.0f;
    auto unit = Vector3::Normalize({1.f, 1.f, 1.f}) * 42.0f;

    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingBox{Vector3::Zero, max}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingBox{Vector3::Zero - unit * Vector3::UnitX, max - unit * Vector3::UnitX}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingBox{Vector3::Zero - unit * Vector3::UnitY, max - unit * Vector3::UnitY}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingBox{Vector3::Zero - unit * Vector3::UnitZ, max - unit * Vector3::UnitZ}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingBox{min, Vector3::Zero}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingBox{min + unit * Vector3::UnitX, Vector3::Zero + unit * Vector3::UnitX}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingBox{min + unit * Vector3::UnitY, Vector3::Zero + unit * Vector3::UnitY}));
    EXPECT_EQ(ContainmentType::Contains, sphere.Contains(BoundingBox{min + unit * Vector3::UnitZ, Vector3::Zero + unit * Vector3::UnitZ}));

    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingBox{Vector3::Zero, max * 1.01f}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingBox{Vector3::Zero - unit * Vector3::UnitX, max * 1.01f - unit * Vector3::UnitX}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingBox{Vector3::Zero - unit * Vector3::UnitY, max * 1.01f - unit * Vector3::UnitY}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingBox{Vector3::Zero - unit * Vector3::UnitZ, max * 1.01f - unit * Vector3::UnitZ}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingBox{min * 1.01f, Vector3::Zero}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingBox{min * 1.01f + unit * Vector3::UnitX, Vector3::Zero + unit * Vector3::UnitX}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingBox{min * 1.01f + unit * Vector3::UnitY, Vector3::Zero + unit * Vector3::UnitY}));
    EXPECT_EQ(ContainmentType::Intersects, sphere.Contains(BoundingBox{min * 1.01f + unit * Vector3::UnitZ, Vector3::Zero + unit * Vector3::UnitZ}));

    EXPECT_EQ(ContainmentType::Disjoint, sphere.Contains(BoundingBox{max * 1.01f, max * 1.01f + unit}));
}

TEST(BoundingSphere, Intersects_BoundingBox)
{
    BoundingSphere sphere;
    sphere.Center = Vector3::Zero;
    sphere.Radius = 42.0f;

    auto min = Vector3::Normalize({-1.f, -1.f, -1.f}) * 42.0f;
    auto max = Vector3::Normalize({1.f, 1.f, 1.f}) * 42.0f;
    auto unit = Vector3::Normalize({1.f, 1.f, 1.f}) * 42.0f;

    EXPECT_TRUE(sphere.Intersects(BoundingBox{Vector3::Zero, max}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{Vector3::Zero - unit * Vector3::UnitX, max - unit * Vector3::UnitX}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{Vector3::Zero - unit * Vector3::UnitY, max - unit * Vector3::UnitY}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{Vector3::Zero - unit * Vector3::UnitZ, max - unit * Vector3::UnitZ}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{min, Vector3::Zero}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{min + unit * Vector3::UnitX, Vector3::Zero + unit * Vector3::UnitX}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{min + unit * Vector3::UnitY, Vector3::Zero + unit * Vector3::UnitY}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{min + unit * Vector3::UnitZ, Vector3::Zero + unit * Vector3::UnitZ}));

    EXPECT_TRUE(sphere.Intersects(BoundingBox{Vector3::Zero, max * 1.01f}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{Vector3::Zero - unit * Vector3::UnitX, max * 1.01f - unit * Vector3::UnitX}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{Vector3::Zero - unit * Vector3::UnitY, max * 1.01f - unit * Vector3::UnitY}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{Vector3::Zero - unit * Vector3::UnitZ, max * 1.01f - unit * Vector3::UnitZ}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{min * 1.01f, Vector3::Zero}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{min * 1.01f + unit * Vector3::UnitX, Vector3::Zero + unit * Vector3::UnitX}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{min * 1.01f + unit * Vector3::UnitY, Vector3::Zero + unit * Vector3::UnitY}));
    EXPECT_TRUE(sphere.Intersects(BoundingBox{min * 1.01f + unit * Vector3::UnitZ, Vector3::Zero + unit * Vector3::UnitZ}));

    EXPECT_FALSE(sphere.Intersects(BoundingBox{max * 1.01f, max * 1.01f + unit}));
}

TEST(BoundingSphere, Intersects_BoundingSphere)
{
    BoundingSphere sphere;
    sphere.Center = Vector3::Zero;
    sphere.Radius = 42.0f;

    EXPECT_TRUE(sphere.Intersects(sphere));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3::Zero, 43.0f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3::Zero, 41.0f}));

    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{40.f, 0.f, 0.f}, 1.9f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{0.f, 40.f, 0.f}, 1.9f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, 40.f}, 1.9f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{-40.f, 0.f, 0.f}, 1.9f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{0.f, -40.f, 0.f}, 1.9f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, -40.f}, 1.9f}));

    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{42.f, 0.f, 0.f}, 1.0f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{0.f, 42.f, 0.f}, 1.0f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, 42.f}, 1.0f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{-42.f, 0.f, 0.f}, 1.0f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{0.f, -42.f, 0.f}, 1.0f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, -42.f}, 1.0f}));

    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{43.f, 0.f, 0.f}, 2.0f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{0.f, 43.f, 0.f}, 2.0f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, 43.f}, 2.0f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{-43.f, 0.f, 0.f}, 2.0f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{0.f, -43.f, 0.f}, 2.0f}));
    EXPECT_TRUE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, -43.f}, 2.0f}));

    EXPECT_FALSE(sphere.Intersects(BoundingSphere{Vector3{43.f, 0.f, 0.f}, 0.5f}));
    EXPECT_FALSE(sphere.Intersects(BoundingSphere{Vector3{0.f, 43.f, 0.f}, 0.5f}));
    EXPECT_FALSE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, 43.f}, 0.5f}));
    EXPECT_FALSE(sphere.Intersects(BoundingSphere{Vector3{-43.f, 0.f, 0.f}, 0.5f}));
    EXPECT_FALSE(sphere.Intersects(BoundingSphere{Vector3{0.f, -43.f, 0.f}, 0.5f}));
    EXPECT_FALSE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, -43.f}, 0.5f}));
}
