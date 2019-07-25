// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Ray.hpp"
#include "Pomdog/Math/BoundingBox.hpp"
#include "Pomdog/Math/BoundingSphere.hpp"
#include "catch.hpp"
#include <limits>

using Pomdog::Ray;
using namespace Pomdog;

namespace {
static constexpr auto Epsilon = std::numeric_limits<float>::epsilon();
} // unnamed namespace

TEST_CASE("Ray", "[Ray]")
{
    SECTION("Ray")
    {
        Ray ray(Vector3{3.0f, 4.0f, 7.0f}, Vector3{5.0f, 8.0f, 9.0f});
        REQUIRE(Vector3{3.0f, 4.0f, 7.0f} == ray.Position);
        REQUIRE(Vector3{5.0f, 8.0f, 9.0f} == ray.Direction);
    }
    SECTION("Intersects_BoundingBox_1")
    {
        Ray const data[] = {
            {Vector3::Zero, Vector3::Zero},
            {Vector3::Zero, Vector3::UnitX},
            {Vector3::Zero, Vector3::UnitY},
            {Vector3::Zero, Vector3::UnitZ},
            {Vector3::Zero, -Vector3::UnitX},
            {Vector3::Zero, -Vector3::UnitY},
            {Vector3::Zero, -Vector3::UnitZ},
        };

        BoundingBox box;
        box.Min = Vector3{-0.5f, -0.5f, -0.5f} * Epsilon;
        box.Max = Vector3{0.5f, 0.5f, 0.5f} * Epsilon;
        for (auto ray : data) {
            REQUIRE(ray.Intersects(box));
        }

        box.Min = Vector3{-0.5f, -0.5f, -0.5f};
        box.Max = Vector3{0.5f, 0.5f, 0.5f};
        for (auto ray : data) {
            REQUIRE(ray.Intersects(box));
        }

        box.Min = Vector3{-0.5f, -0.5f, -0.5f} * std::numeric_limits<float>::max() / 2;
        box.Max = Vector3{0.5f, 0.5f, 0.5f} * std::numeric_limits<float>::max() / 2;
        for (auto ray : data) {
            REQUIRE(ray.Intersects(box));
        }
    }
    SECTION("Intersects_BoundingBox_2")
    {
        BoundingBox box;
        box.Min = Vector3{-0.5f, -0.5f, -0.5f};
        box.Max = Vector3{0.5f, 0.5f, 0.5f};

        Ray ray;
        ray.Position = Vector3::UnitX;
        ray.Direction = -Vector3::UnitX;
        REQUIRE(ray.Intersects(box));

        ray.Position = -Vector3::UnitX;
        ray.Direction = Vector3::UnitX;
        REQUIRE(ray.Intersects(box));

        ray.Position = Vector3::UnitY;
        ray.Direction = -Vector3::UnitY;
        REQUIRE(ray.Intersects(box));

        ray.Position = -Vector3::UnitY;
        ray.Direction = Vector3::UnitY;
        REQUIRE(ray.Intersects(box));

        ray.Position = Vector3::UnitZ;
        ray.Direction = -Vector3::UnitZ;
        REQUIRE(ray.Intersects(box));

        ray.Position = -Vector3::UnitZ;
        ray.Direction = Vector3::UnitZ;
        REQUIRE(ray.Intersects(box));

        //
        ray.Position = Vector3::UnitX;
        ray.Direction = Vector3{0.0f, 0.5f, 0.5f} - ray.Position;
        REQUIRE(ray.Intersects(box));

        ray.Position = -Vector3::UnitX;
        ray.Direction = Vector3{0.0f, 0.5f, 0.5f} - ray.Position;
        REQUIRE(ray.Intersects(box));

        ray.Position = Vector3::UnitY;
        ray.Direction = Vector3{0.5f, 0.0f, 0.5f} - ray.Position;
        REQUIRE(ray.Intersects(box));

        ray.Position = -Vector3::UnitY;
        ray.Direction = Vector3{0.5f, 0.0f, 0.5f} - ray.Position;
        REQUIRE(ray.Intersects(box));

        ray.Position = Vector3::UnitZ;
        ray.Direction = Vector3{0.5f, 0.5f, 0.0f} - ray.Position;
        REQUIRE(ray.Intersects(box));

        ray.Position = -Vector3::UnitZ;
        ray.Direction = Vector3{0.5f, 0.5f, 0.0f} - ray.Position;
        REQUIRE(ray.Intersects(box));

        //
        ray.Position = Vector3::UnitX;
        ray.Direction = Vector3::UnitX;
        REQUIRE_FALSE(ray.Intersects(box));

        ray.Position = -Vector3::UnitX;
        ray.Direction = -Vector3::UnitX;
        REQUIRE_FALSE(ray.Intersects(box));

        ray.Position = Vector3::UnitY;
        ray.Direction = Vector3::UnitY;
        REQUIRE_FALSE(ray.Intersects(box));

        ray.Position = -Vector3::UnitY;
        ray.Direction = -Vector3::UnitY;
        REQUIRE_FALSE(ray.Intersects(box));

        ray.Position = Vector3::UnitZ;
        ray.Direction = Vector3::UnitZ;
        REQUIRE_FALSE(ray.Intersects(box));

        ray.Position = -Vector3::UnitZ;
        ray.Direction = -Vector3::UnitZ;
        REQUIRE_FALSE(ray.Intersects(box));

        //
        ray.Direction = -Vector3::UnitY;
        ray.Position = Vector3{-0.5f - Epsilon, 1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(box));
        ray.Position = Vector3{-0.5f + Epsilon, 1.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{0.0f, 1.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{0.5f - Epsilon, 1.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{0.5f + Epsilon, 1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(box));

        ray.Direction = Vector3::UnitY;
        ray.Position = Vector3{-0.5f - Epsilon, -1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(box));
        ray.Position = Vector3{-0.5f + Epsilon, -1.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{0.0f, -1.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{0.5f - Epsilon, -1.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{0.5f + Epsilon, -1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(box));

        ray.Direction = -Vector3::UnitZ;
        ray.Position = Vector3{0.0f, -0.5f - Epsilon, 1.0f};
        REQUIRE_FALSE(ray.Intersects(box));
        ray.Position = Vector3{0.0f, -0.5f + Epsilon, 1.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{0.0f, 0.0f, 1.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{0.0f, 0.5f - Epsilon, 1.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{0.0f, 0.5f + Epsilon, 1.0f};
        REQUIRE_FALSE(ray.Intersects(box));

        ray.Direction = Vector3::UnitZ;
        ray.Position = Vector3{0.0f, -0.5f - Epsilon, -1.0f};
        REQUIRE_FALSE(ray.Intersects(box));
        ray.Position = Vector3{0.0f, -0.5f + Epsilon, -1.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{0.0f, 0.0f, -1.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{0.0f, 0.5f - Epsilon, -1.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{0.0f, 0.5f + Epsilon, -1.0f};
        REQUIRE_FALSE(ray.Intersects(box));

        ray.Direction = -Vector3::UnitX;
        ray.Position = Vector3{1.0f, 0.0f, -0.5f - Epsilon};
        REQUIRE_FALSE(ray.Intersects(box));
        ray.Position = Vector3{1.0f, 0.0f, -0.5f + Epsilon};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{1.0f, 0.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{1.0f, 0.0f, 0.5f - Epsilon};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{1.0f, 0.0f, 0.5f + Epsilon};
        REQUIRE_FALSE(ray.Intersects(box));

        ray.Direction = Vector3::UnitX;
        ray.Position = Vector3{-1.0f, 0.0f, -0.5f - Epsilon};
        REQUIRE_FALSE(ray.Intersects(box));
        ray.Position = Vector3{-1.0f, 0.0f, -0.5f + Epsilon};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{-1.0f, 0.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{-1.0f, 0.0f, 0.5f - Epsilon};
        REQUIRE(ray.Intersects(box));
        ray.Position = Vector3{-1.0f, 0.0f, 0.5f + Epsilon};
        REQUIRE_FALSE(ray.Intersects(box));
    }
    SECTION("Intersects_BoundingSphere_1")
    {
        Ray const data[] = {
            {Vector3::Zero, Vector3::Zero},
            {Vector3::Zero, Vector3::UnitX},
            {Vector3::Zero, Vector3::UnitY},
            {Vector3::Zero, Vector3::UnitZ},
            {Vector3::Zero, -Vector3::UnitX},
            {Vector3::Zero, -Vector3::UnitY},
            {Vector3::Zero, -Vector3::UnitZ},
        };

        BoundingSphere sphere;
        sphere.Center = Vector3::Zero;
        sphere.Radius = Epsilon;
        for (auto ray : data) {
            REQUIRE(ray.Intersects(sphere));
        }

        sphere.Center = Vector3::Zero;
        sphere.Radius = 1.0f;
        for (auto ray : data) {
            REQUIRE(ray.Intersects(sphere));
        }

        sphere.Center = Vector3::Zero;
        sphere.Radius = std::numeric_limits<float>::max() / 2;
        for (auto ray : data) {
            REQUIRE(ray.Intersects(sphere));
        }
    }
    SECTION("Intersects_BoundingSphere_2")
    {
        Ray const data[] = {
            {Vector3{1.0f, 1.0f, 1.0f}, Vector3::Zero},
            {Vector3{1.0f, 1.0f, 0.0f}, Vector3::Zero},
            {Vector3{1.0f, 0.0f, 1.0f}, Vector3::Zero},
            {Vector3{1.0f, 0.0f, 0.0f}, Vector3::Zero},
            {Vector3{0.0f, 1.0f, 1.0f}, Vector3::Zero},
            {Vector3{0.0f, 1.0f, 0.0f}, Vector3::Zero},
            {Vector3{0.0f, 0.0f, 1.0f}, Vector3::Zero},
        };

        BoundingSphere sphere;
        sphere.Center = Vector3::Zero;
        sphere.Radius = Epsilon;
        for (auto ray : data) {
            REQUIRE_FALSE(ray.Intersects(sphere));
        }

        sphere.Center = Vector3::Zero;
        sphere.Radius = 1.0f - Epsilon;
        for (auto ray : data) {
            ray.Position = Vector3::Normalize(ray.Position);
            REQUIRE_FALSE(ray.Intersects(sphere));
        }

        sphere.Center = Vector3::Zero;
        sphere.Radius = 1.0f + Epsilon;
        for (auto ray : data) {
            ray.Position = Vector3::Normalize(ray.Position);
            REQUIRE(ray.Intersects(sphere));
        }
    }
    SECTION("Intersects_BoundingSphere_3")
    {
        BoundingSphere sphere;
        sphere.Center = Vector3::Zero;
        sphere.Radius = 0.5f;

        Ray ray;
        ray.Position = Vector3::UnitX;
        ray.Direction = -Vector3::UnitX;
        REQUIRE(ray.Intersects(sphere));

        ray.Position = -Vector3::UnitX;
        ray.Direction = Vector3::UnitX;
        REQUIRE(ray.Intersects(sphere));

        ray.Position = Vector3::UnitY;
        ray.Direction = -Vector3::UnitY;
        REQUIRE(ray.Intersects(sphere));

        ray.Position = -Vector3::UnitY;
        ray.Direction = Vector3::UnitY;
        REQUIRE(ray.Intersects(sphere));

        ray.Position = Vector3::UnitZ;
        ray.Direction = -Vector3::UnitZ;
        REQUIRE(ray.Intersects(sphere));

        ray.Position = -Vector3::UnitZ;
        ray.Direction = Vector3::UnitZ;
        REQUIRE(ray.Intersects(sphere));

        //
        ray.Position = Vector3::UnitX;
        ray.Direction = Vector3{0.0f, 0.5f, 0.5f} - ray.Position;
        REQUIRE(ray.Intersects(sphere));

        ray.Position = -Vector3::UnitX;
        ray.Direction = Vector3{0.0f, 0.5f, 0.5f} - ray.Position;
        REQUIRE(ray.Intersects(sphere));

        ray.Position = Vector3::UnitY;
        ray.Direction = Vector3{0.5f, 0.0f, 0.5f} - ray.Position;
        REQUIRE(ray.Intersects(sphere));

        ray.Position = -Vector3::UnitY;
        ray.Direction = Vector3{0.5f, 0.0f, 0.5f} - ray.Position;
        REQUIRE(ray.Intersects(sphere));

        ray.Position = Vector3::UnitZ;
        ray.Direction = Vector3{0.5f, 0.5f, 0.0f} - ray.Position;
        REQUIRE(ray.Intersects(sphere));

        ray.Position = -Vector3::UnitZ;
        ray.Direction = Vector3{0.5f, 0.5f, 0.0f} - ray.Position;
        REQUIRE(ray.Intersects(sphere));

        //
        ray.Position = Vector3::UnitX;
        ray.Direction = Vector3::UnitX;
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.Position = -Vector3::UnitX;
        ray.Direction = -Vector3::UnitX;
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.Position = Vector3::UnitY;
        ray.Direction = Vector3::UnitY;
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.Position = -Vector3::UnitY;
        ray.Direction = -Vector3::UnitY;
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.Position = Vector3::UnitZ;
        ray.Direction = Vector3::UnitZ;
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.Position = -Vector3::UnitZ;
        ray.Direction = -Vector3::UnitZ;
        REQUIRE_FALSE(ray.Intersects(sphere));

        //
        ray.Direction = -Vector3::UnitY;
        ray.Position = Vector3{-0.5f - Epsilon, 1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));
        ray.Position = Vector3{-0.5f + Epsilon, 1.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{0.0f, 1.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{0.5f - Epsilon, 1.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{0.5f + Epsilon, 1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.Direction = Vector3::UnitY;
        ray.Position = Vector3{-0.5f - Epsilon, -1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));
        ray.Position = Vector3{-0.5f + Epsilon, -1.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{0.0f, -1.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{0.5f - Epsilon, -1.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{0.5f + Epsilon, -1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.Direction = -Vector3::UnitZ;
        ray.Position = Vector3{0.0f, -0.5f - Epsilon, 1.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));
        ray.Position = Vector3{0.0f, -0.5f + Epsilon, 1.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{0.0f, 0.0f, 1.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{0.0f, 0.5f - Epsilon, 1.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{0.0f, 0.5f + Epsilon, 1.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.Direction = Vector3::UnitZ;
        ray.Position = Vector3{0.0f, -0.5f - Epsilon, -1.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));
        ray.Position = Vector3{0.0f, -0.5f + Epsilon, -1.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{0.0f, 0.0f, -1.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{0.0f, 0.5f - Epsilon, -1.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{0.0f, 0.5f + Epsilon, -1.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.Direction = -Vector3::UnitX;
        ray.Position = Vector3{1.0f, 0.0f, -0.5f - Epsilon};
        REQUIRE_FALSE(ray.Intersects(sphere));
        ray.Position = Vector3{1.0f, 0.0f, -0.5f + Epsilon};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{1.0f, 0.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{1.0f, 0.0f, 0.5f - Epsilon};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{1.0f, 0.0f, 0.5f + Epsilon};
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.Direction = Vector3::UnitX;
        ray.Position = Vector3{-1.0f, 0.0f, -0.5f - Epsilon};
        REQUIRE_FALSE(ray.Intersects(sphere));
        ray.Position = Vector3{-1.0f, 0.0f, -0.5f + Epsilon};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{-1.0f, 0.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{-1.0f, 0.0f, 0.5f - Epsilon};
        REQUIRE(ray.Intersects(sphere));
        ray.Position = Vector3{-1.0f, 0.0f, 0.5f + Epsilon};
        REQUIRE_FALSE(ray.Intersects(sphere));
    }
}
