// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include <Pomdog/Math/Ray.hpp>
#include <Pomdog/Math/BoundingBox.hpp>
#include <Pomdog/Math/BoundingSphere.hpp>
#include <gtest/iutest_switch.hpp>
#include <array>

using Pomdog::Ray;
using namespace Pomdog;

TEST(Ray, TrivialCase)
{
    Ray ray(Vector3(3, 4, 7), Vector3(5, 8, 9));
    EXPECT_EQ(Vector3(3, 4, 7), ray.Position);
    EXPECT_EQ(Vector3(5, 8, 9), ray.Direction);
}

class RayTest : public ::testing::Test {
protected:
    virtual void SetUp() override
    {
    }

    Ray ray;
    BoundingBox box;
    BoundingSphere sphere;
};

TEST_F(RayTest, Intersects_BoundingBox_1)
{
    Ray data[] = {
        {Vector3::Zero, Vector3::Zero},
        {Vector3::Zero, Vector3::UnitX},
        {Vector3::Zero, Vector3::UnitY},
        {Vector3::Zero, Vector3::UnitZ},
        {Vector3::Zero, -Vector3::UnitX},
        {Vector3::Zero, -Vector3::UnitY},
        {Vector3::Zero, -Vector3::UnitZ},
    };

    box.Min = Vector3{-0.5f, -0.5f, -0.5f} * std::numeric_limits<float>::epsilon();
    box.Max = Vector3{0.5f, 0.5f, 0.5f} * std::numeric_limits<float>::epsilon();
    for (auto && ray : data) {
        EXPECT_TRUE(ray.Intersects(box));
    }

    box.Min = Vector3{-0.5f, -0.5f, -0.5f};
    box.Max = Vector3{0.5f, 0.5f, 0.5f};
    for (auto && ray : data) {
        EXPECT_TRUE(ray.Intersects(box));
    }

    box.Min = Vector3{-0.5f, -0.5f, -0.5f} * std::numeric_limits<float>::max() / 2;
    box.Max = Vector3{0.5f, 0.5f, 0.5f} * std::numeric_limits<float>::max() / 2;
    for (auto && ray : data) {
        EXPECT_TRUE(ray.Intersects(box));
    }
}

TEST_F(RayTest, Intersects_BoundingBox_2)
{
    box.Min = Vector3{-0.5f, -0.5f, -0.5f};
    box.Max = Vector3{0.5f, 0.5f, 0.5f};

    ray.Position = Vector3::UnitX;
    ray.Direction = -Vector3::UnitX;
    EXPECT_TRUE(ray.Intersects(box));

    ray.Position = -Vector3::UnitX;
    ray.Direction = Vector3::UnitX;
    EXPECT_TRUE(ray.Intersects(box));

    ray.Position = Vector3::UnitY;
    ray.Direction = -Vector3::UnitY;
    EXPECT_TRUE(ray.Intersects(box));

    ray.Position = -Vector3::UnitY;
    ray.Direction = Vector3::UnitY;
    EXPECT_TRUE(ray.Intersects(box));

    ray.Position = Vector3::UnitZ;
    ray.Direction = -Vector3::UnitZ;
    EXPECT_TRUE(ray.Intersects(box));

    ray.Position = -Vector3::UnitZ;
    ray.Direction = Vector3::UnitZ;
    EXPECT_TRUE(ray.Intersects(box));
}

TEST_F(RayTest, Intersects_BoundingSphere_1)
{
    Ray data[] = {
        {Vector3::Zero, Vector3::Zero},
        {Vector3::Zero, Vector3::UnitX},
        {Vector3::Zero, Vector3::UnitY},
        {Vector3::Zero, Vector3::UnitZ},
        {Vector3::Zero, -Vector3::UnitX},
        {Vector3::Zero, -Vector3::UnitY},
        {Vector3::Zero, -Vector3::UnitZ},
    };

    sphere.Center = Vector3::Zero;
    sphere.Radius = std::numeric_limits<float>::epsilon();
    for (auto && ray : data) {
        EXPECT_TRUE(ray.Intersects(sphere));
    }

    sphere.Center = Vector3::Zero;
    sphere.Radius = 1.0f;
    for (auto && ray : data) {
        EXPECT_TRUE(ray.Intersects(sphere));
    }

    sphere.Center = Vector3::Zero;
    sphere.Radius = std::numeric_limits<float>::max() / 2;
    for (auto && ray : data) {
        EXPECT_TRUE(ray.Intersects(sphere));
    }
}

TEST_F(RayTest, Intersects_BoundingSphere_2)
{
    Ray data[] = {
        {Vector3(1, 1, 1), Vector3::Zero},
        {Vector3(1, 1, 0), Vector3::Zero},
        {Vector3(1, 0, 1), Vector3::Zero},
        {Vector3(1, 0, 0), Vector3::Zero},
        {Vector3(0, 1, 1), Vector3::Zero},
        {Vector3(0, 1, 0), Vector3::Zero},
        {Vector3(0, 0, 1), Vector3::Zero},
    };

    sphere.Center = Vector3::Zero;
    sphere.Radius = std::numeric_limits<float>::epsilon();
    for (auto && ray : data) {
        EXPECT_FALSE(ray.Intersects(sphere));
    }

    sphere.Center = Vector3::Zero;
    sphere.Radius = 1.0f - std::numeric_limits<float>::epsilon();
    for (auto && ray : data) {
        ray.Position = Vector3::Normalize(ray.Position);
        EXPECT_FALSE(ray.Intersects(sphere));
    }

    sphere.Center = Vector3::Zero;
    sphere.Radius = 1.0f + std::numeric_limits<float>::epsilon();
    for (auto && ray : data) {
        ray.Position = Vector3::Normalize(ray.Position);
        EXPECT_TRUE(ray.Intersects(sphere));
    }
}

TEST_F(RayTest, Intersects_BoundingSphere_3)
{
    sphere.Center = Vector3::Zero;
    sphere.Radius = 1.0f;

    ray.Position = Vector3::UnitX;
    ray.Direction = -Vector3::UnitX;
    EXPECT_TRUE(ray.Intersects(sphere));

    ray.Position = -Vector3::UnitX;
    ray.Direction = Vector3::UnitX;
    EXPECT_TRUE(ray.Intersects(sphere));

    ray.Position = Vector3::UnitY;
    ray.Direction = -Vector3::UnitY;
    EXPECT_TRUE(ray.Intersects(sphere));

    ray.Position = -Vector3::UnitY;
    ray.Direction = Vector3::UnitY;
    EXPECT_TRUE(ray.Intersects(sphere));

    ray.Position = Vector3::UnitZ;
    ray.Direction = -Vector3::UnitZ;
    EXPECT_TRUE(ray.Intersects(sphere));

    ray.Position = -Vector3::UnitZ;
    ray.Direction = Vector3::UnitZ;
    EXPECT_TRUE(ray.Intersects(sphere));
}
