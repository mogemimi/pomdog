// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/ray.h"
#include <catch_amalgamated.hpp>
#include <limits>

using pomdog::Ray;
using namespace pomdog;

namespace {
static constexpr auto Epsilon = std::numeric_limits<float>::epsilon();
} // namespace

TEST_CASE("Ray", "[Ray]")
{
    SECTION("Ray")
    {
        Ray ray(Vector3{3.0f, 4.0f, 7.0f}, Vector3{5.0f, 8.0f, 9.0f});
        REQUIRE(Vector3{3.0f, 4.0f, 7.0f} == ray.position);
        REQUIRE(Vector3{5.0f, 8.0f, 9.0f} == ray.direction);
    }
    SECTION("Intersects_BoundingBox_1")
    {
        Ray const data[] = {
            {Vector3::Zero(), Vector3::Zero()},
            {Vector3::Zero(), Vector3::UnitX()},
            {Vector3::Zero(), Vector3::UnitY()},
            {Vector3::Zero(), Vector3::UnitZ()},
            {Vector3::Zero(), -Vector3::UnitX()},
            {Vector3::Zero(), -Vector3::UnitY()},
            {Vector3::Zero(), -Vector3::UnitZ()},
        };

        BoundingBox box;
        box.min = Vector3{-0.5f, -0.5f, -0.5f} * Epsilon;
        box.max = Vector3{0.5f, 0.5f, 0.5f} * Epsilon;
        for (auto ray : data) {
            REQUIRE(ray.Intersects(box));
        }

        box.min = Vector3{-0.5f, -0.5f, -0.5f};
        box.max = Vector3{0.5f, 0.5f, 0.5f};
        for (auto ray : data) {
            REQUIRE(ray.Intersects(box));
        }

        box.min = Vector3{-0.5f, -0.5f, -0.5f} * std::numeric_limits<float>::max() / 2;
        box.max = Vector3{0.5f, 0.5f, 0.5f} * std::numeric_limits<float>::max() / 2;
        for (auto ray : data) {
            REQUIRE(ray.Intersects(box));
        }
    }
    SECTION("Intersects_BoundingBox_2")
    {
        BoundingBox box;
        box.min = Vector3{-0.5f, -0.5f, -0.5f};
        box.max = Vector3{0.5f, 0.5f, 0.5f};

        Ray ray;
        ray.position = Vector3::UnitX();
        ray.direction = -Vector3::UnitX();
        REQUIRE(ray.Intersects(box));

        ray.position = -Vector3::UnitX();
        ray.direction = Vector3::UnitX();
        REQUIRE(ray.Intersects(box));

        ray.position = Vector3::UnitY();
        ray.direction = -Vector3::UnitY();
        REQUIRE(ray.Intersects(box));

        ray.position = -Vector3::UnitY();
        ray.direction = Vector3::UnitY();
        REQUIRE(ray.Intersects(box));

        ray.position = Vector3::UnitZ();
        ray.direction = -Vector3::UnitZ();
        REQUIRE(ray.Intersects(box));

        ray.position = -Vector3::UnitZ();
        ray.direction = Vector3::UnitZ();
        REQUIRE(ray.Intersects(box));

        //
        ray.position = Vector3::UnitX();
        ray.direction = Vector3{0.0f, 0.5f, 0.5f} - ray.position;
        REQUIRE(ray.Intersects(box));

        ray.position = -Vector3::UnitX();
        ray.direction = Vector3{0.0f, 0.5f, 0.5f} - ray.position;
        REQUIRE(ray.Intersects(box));

        ray.position = Vector3::UnitY();
        ray.direction = Vector3{0.5f, 0.0f, 0.5f} - ray.position;
        REQUIRE(ray.Intersects(box));

        ray.position = -Vector3::UnitY();
        ray.direction = Vector3{0.5f, 0.0f, 0.5f} - ray.position;
        REQUIRE(ray.Intersects(box));

        ray.position = Vector3::UnitZ();
        ray.direction = Vector3{0.5f, 0.5f, 0.0f} - ray.position;
        REQUIRE(ray.Intersects(box));

        ray.position = -Vector3::UnitZ();
        ray.direction = Vector3{0.5f, 0.5f, 0.0f} - ray.position;
        REQUIRE(ray.Intersects(box));

        //
        ray.position = Vector3::UnitX();
        ray.direction = Vector3::UnitX();
        REQUIRE_FALSE(ray.Intersects(box));

        ray.position = -Vector3::UnitX();
        ray.direction = -Vector3::UnitX();
        REQUIRE_FALSE(ray.Intersects(box));

        ray.position = Vector3::UnitY();
        ray.direction = Vector3::UnitY();
        REQUIRE_FALSE(ray.Intersects(box));

        ray.position = -Vector3::UnitY();
        ray.direction = -Vector3::UnitY();
        REQUIRE_FALSE(ray.Intersects(box));

        ray.position = Vector3::UnitZ();
        ray.direction = Vector3::UnitZ();
        REQUIRE_FALSE(ray.Intersects(box));

        ray.position = -Vector3::UnitZ();
        ray.direction = -Vector3::UnitZ();
        REQUIRE_FALSE(ray.Intersects(box));

        //
        ray.direction = -Vector3::UnitY();
        ray.position = Vector3{-0.5f - Epsilon, 1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(box));
        ray.position = Vector3{-0.5f + Epsilon, 1.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{0.0f, 1.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{0.5f - Epsilon, 1.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{0.5f + Epsilon, 1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(box));

        ray.direction = Vector3::UnitY();
        ray.position = Vector3{-0.5f - Epsilon, -1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(box));
        ray.position = Vector3{-0.5f + Epsilon, -1.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{0.0f, -1.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{0.5f - Epsilon, -1.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{0.5f + Epsilon, -1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(box));

        ray.direction = -Vector3::UnitZ();
        ray.position = Vector3{0.0f, -0.5f - Epsilon, 1.0f};
        REQUIRE_FALSE(ray.Intersects(box));
        ray.position = Vector3{0.0f, -0.5f + Epsilon, 1.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{0.0f, 0.0f, 1.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{0.0f, 0.5f - Epsilon, 1.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{0.0f, 0.5f + Epsilon, 1.0f};
        REQUIRE_FALSE(ray.Intersects(box));

        ray.direction = Vector3::UnitZ();
        ray.position = Vector3{0.0f, -0.5f - Epsilon, -1.0f};
        REQUIRE_FALSE(ray.Intersects(box));
        ray.position = Vector3{0.0f, -0.5f + Epsilon, -1.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{0.0f, 0.0f, -1.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{0.0f, 0.5f - Epsilon, -1.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{0.0f, 0.5f + Epsilon, -1.0f};
        REQUIRE_FALSE(ray.Intersects(box));

        ray.direction = -Vector3::UnitX();
        ray.position = Vector3{1.0f, 0.0f, -0.5f - Epsilon};
        REQUIRE_FALSE(ray.Intersects(box));
        ray.position = Vector3{1.0f, 0.0f, -0.5f + Epsilon};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{1.0f, 0.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{1.0f, 0.0f, 0.5f - Epsilon};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{1.0f, 0.0f, 0.5f + Epsilon};
        REQUIRE_FALSE(ray.Intersects(box));

        ray.direction = Vector3::UnitX();
        ray.position = Vector3{-1.0f, 0.0f, -0.5f - Epsilon};
        REQUIRE_FALSE(ray.Intersects(box));
        ray.position = Vector3{-1.0f, 0.0f, -0.5f + Epsilon};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{-1.0f, 0.0f, 0.0f};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{-1.0f, 0.0f, 0.5f - Epsilon};
        REQUIRE(ray.Intersects(box));
        ray.position = Vector3{-1.0f, 0.0f, 0.5f + Epsilon};
        REQUIRE_FALSE(ray.Intersects(box));
    }
    SECTION("Intersects_BoundingSphere_1")
    {
        Ray const data[] = {
            {Vector3::Zero(), Vector3::Zero()},
            {Vector3::Zero(), Vector3::UnitX()},
            {Vector3::Zero(), Vector3::UnitY()},
            {Vector3::Zero(), Vector3::UnitZ()},
            {Vector3::Zero(), -Vector3::UnitX()},
            {Vector3::Zero(), -Vector3::UnitY()},
            {Vector3::Zero(), -Vector3::UnitZ()},
        };

        BoundingSphere sphere;
        sphere.center = Vector3::Zero();
        sphere.radius = Epsilon;
        for (auto ray : data) {
            REQUIRE(ray.Intersects(sphere));
        }

        sphere.center = Vector3::Zero();
        sphere.radius = 1.0f;
        for (auto ray : data) {
            REQUIRE(ray.Intersects(sphere));
        }

        sphere.center = Vector3::Zero();
        sphere.radius = std::numeric_limits<float>::max() / 2;
        for (auto ray : data) {
            REQUIRE(ray.Intersects(sphere));
        }
    }
    SECTION("Intersects_BoundingSphere_2")
    {
        Ray const data[] = {
            {Vector3{1.0f, 1.0f, 1.0f}, Vector3::Zero()},
            {Vector3{1.0f, 1.0f, 0.0f}, Vector3::Zero()},
            {Vector3{1.0f, 0.0f, 1.0f}, Vector3::Zero()},
            {Vector3{1.0f, 0.0f, 0.0f}, Vector3::Zero()},
            {Vector3{0.0f, 1.0f, 1.0f}, Vector3::Zero()},
            {Vector3{0.0f, 1.0f, 0.0f}, Vector3::Zero()},
            {Vector3{0.0f, 0.0f, 1.0f}, Vector3::Zero()},
        };

        BoundingSphere sphere;
        sphere.center = Vector3::Zero();
        sphere.radius = Epsilon;
        for (auto ray : data) {
            REQUIRE_FALSE(ray.Intersects(sphere));
        }

        sphere.center = Vector3::Zero();
        sphere.radius = 1.0f - Epsilon;
        for (auto ray : data) {
            ray.position = math::Normalize(ray.position);
            REQUIRE_FALSE(ray.Intersects(sphere));
        }

        sphere.center = Vector3::Zero();
        sphere.radius = 1.0f + Epsilon;
        for (auto ray : data) {
            ray.position = math::Normalize(ray.position);
            REQUIRE(ray.Intersects(sphere));
        }
    }
    SECTION("Intersects_BoundingSphere_3")
    {
        BoundingSphere sphere;
        sphere.center = Vector3::Zero();
        sphere.radius = 0.5f;

        Ray ray;
        ray.position = Vector3::UnitX();
        ray.direction = -Vector3::UnitX();
        REQUIRE(ray.Intersects(sphere));

        ray.position = -Vector3::UnitX();
        ray.direction = Vector3::UnitX();
        REQUIRE(ray.Intersects(sphere));

        ray.position = Vector3::UnitY();
        ray.direction = -Vector3::UnitY();
        REQUIRE(ray.Intersects(sphere));

        ray.position = -Vector3::UnitY();
        ray.direction = Vector3::UnitY();
        REQUIRE(ray.Intersects(sphere));

        ray.position = Vector3::UnitZ();
        ray.direction = -Vector3::UnitZ();
        REQUIRE(ray.Intersects(sphere));

        ray.position = -Vector3::UnitZ();
        ray.direction = Vector3::UnitZ();
        REQUIRE(ray.Intersects(sphere));

        //
        ray.position = Vector3::UnitX();
        ray.direction = Vector3{0.0f, 0.5f, 0.5f} - ray.position;
        REQUIRE(ray.Intersects(sphere));

        ray.position = -Vector3::UnitX();
        ray.direction = Vector3{0.0f, 0.5f, 0.5f} - ray.position;
        REQUIRE(ray.Intersects(sphere));

        ray.position = Vector3::UnitY();
        ray.direction = Vector3{0.5f, 0.0f, 0.5f} - ray.position;
        REQUIRE(ray.Intersects(sphere));

        ray.position = -Vector3::UnitY();
        ray.direction = Vector3{0.5f, 0.0f, 0.5f} - ray.position;
        REQUIRE(ray.Intersects(sphere));

        ray.position = Vector3::UnitZ();
        ray.direction = Vector3{0.5f, 0.5f, 0.0f} - ray.position;
        REQUIRE(ray.Intersects(sphere));

        ray.position = -Vector3::UnitZ();
        ray.direction = Vector3{0.5f, 0.5f, 0.0f} - ray.position;
        REQUIRE(ray.Intersects(sphere));

        //
        ray.position = Vector3::UnitX();
        ray.direction = Vector3::UnitX();
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.position = -Vector3::UnitX();
        ray.direction = -Vector3::UnitX();
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.position = Vector3::UnitY();
        ray.direction = Vector3::UnitY();
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.position = -Vector3::UnitY();
        ray.direction = -Vector3::UnitY();
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.position = Vector3::UnitZ();
        ray.direction = Vector3::UnitZ();
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.position = -Vector3::UnitZ();
        ray.direction = -Vector3::UnitZ();
        REQUIRE_FALSE(ray.Intersects(sphere));

        //
        ray.direction = -Vector3::UnitY();
        ray.position = Vector3{-0.5f - Epsilon, 1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));
        ray.position = Vector3{-0.5f + Epsilon, 1.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{0.0f, 1.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{0.5f - Epsilon, 1.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{0.5f + Epsilon, 1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.direction = Vector3::UnitY();
        ray.position = Vector3{-0.5f - Epsilon, -1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));
        ray.position = Vector3{-0.5f + Epsilon, -1.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{0.0f, -1.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{0.5f - Epsilon, -1.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{0.5f + Epsilon, -1.0f, 0.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.direction = -Vector3::UnitZ();
        ray.position = Vector3{0.0f, -0.5f - Epsilon, 1.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));
        ray.position = Vector3{0.0f, -0.5f + Epsilon, 1.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{0.0f, 0.0f, 1.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{0.0f, 0.5f - Epsilon, 1.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{0.0f, 0.5f + Epsilon, 1.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.direction = Vector3::UnitZ();
        ray.position = Vector3{0.0f, -0.5f - Epsilon, -1.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));
        ray.position = Vector3{0.0f, -0.5f + Epsilon, -1.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{0.0f, 0.0f, -1.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{0.0f, 0.5f - Epsilon, -1.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{0.0f, 0.5f + Epsilon, -1.0f};
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.direction = -Vector3::UnitX();
        ray.position = Vector3{1.0f, 0.0f, -0.5f - Epsilon};
        REQUIRE_FALSE(ray.Intersects(sphere));
        ray.position = Vector3{1.0f, 0.0f, -0.5f + Epsilon};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{1.0f, 0.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{1.0f, 0.0f, 0.5f - Epsilon};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{1.0f, 0.0f, 0.5f + Epsilon};
        REQUIRE_FALSE(ray.Intersects(sphere));

        ray.direction = Vector3::UnitX();
        ray.position = Vector3{-1.0f, 0.0f, -0.5f - Epsilon};
        REQUIRE_FALSE(ray.Intersects(sphere));
        ray.position = Vector3{-1.0f, 0.0f, -0.5f + Epsilon};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{-1.0f, 0.0f, 0.0f};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{-1.0f, 0.0f, 0.5f - Epsilon};
        REQUIRE(ray.Intersects(sphere));
        ray.position = Vector3{-1.0f, 0.0f, 0.5f + Epsilon};
        REQUIRE_FALSE(ray.Intersects(sphere));
    }
}
