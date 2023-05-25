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
            {Vector3::createZero(), Vector3::createZero()},
            {Vector3::createZero(), Vector3::createUnitX()},
            {Vector3::createZero(), Vector3::createUnitY()},
            {Vector3::createZero(), Vector3::createUnitZ()},
            {Vector3::createZero(), -Vector3::createUnitX()},
            {Vector3::createZero(), -Vector3::createUnitY()},
            {Vector3::createZero(), -Vector3::createUnitZ()},
        };

        BoundingBox box;
        box.min = Vector3{-0.5f, -0.5f, -0.5f} * Epsilon;
        box.max = Vector3{0.5f, 0.5f, 0.5f} * Epsilon;
        for (auto ray : data) {
            REQUIRE(ray.intersects(box));
        }

        box.min = Vector3{-0.5f, -0.5f, -0.5f};
        box.max = Vector3{0.5f, 0.5f, 0.5f};
        for (auto ray : data) {
            REQUIRE(ray.intersects(box));
        }

        box.min = Vector3{-0.5f, -0.5f, -0.5f} * std::numeric_limits<float>::max() / 2;
        box.max = Vector3{0.5f, 0.5f, 0.5f} * std::numeric_limits<float>::max() / 2;
        for (auto ray : data) {
            REQUIRE(ray.intersects(box));
        }
    }
    SECTION("Intersects_BoundingBox_2")
    {
        BoundingBox box;
        box.min = Vector3{-0.5f, -0.5f, -0.5f};
        box.max = Vector3{0.5f, 0.5f, 0.5f};

        Ray ray;
        ray.position = Vector3::createUnitX();
        ray.direction = -Vector3::createUnitX();
        REQUIRE(ray.intersects(box));

        ray.position = -Vector3::createUnitX();
        ray.direction = Vector3::createUnitX();
        REQUIRE(ray.intersects(box));

        ray.position = Vector3::createUnitY();
        ray.direction = -Vector3::createUnitY();
        REQUIRE(ray.intersects(box));

        ray.position = -Vector3::createUnitY();
        ray.direction = Vector3::createUnitY();
        REQUIRE(ray.intersects(box));

        ray.position = Vector3::createUnitZ();
        ray.direction = -Vector3::createUnitZ();
        REQUIRE(ray.intersects(box));

        ray.position = -Vector3::createUnitZ();
        ray.direction = Vector3::createUnitZ();
        REQUIRE(ray.intersects(box));

        //
        ray.position = Vector3::createUnitX();
        ray.direction = Vector3{0.0f, 0.5f, 0.5f} - ray.position;
        REQUIRE(ray.intersects(box));

        ray.position = -Vector3::createUnitX();
        ray.direction = Vector3{0.0f, 0.5f, 0.5f} - ray.position;
        REQUIRE(ray.intersects(box));

        ray.position = Vector3::createUnitY();
        ray.direction = Vector3{0.5f, 0.0f, 0.5f} - ray.position;
        REQUIRE(ray.intersects(box));

        ray.position = -Vector3::createUnitY();
        ray.direction = Vector3{0.5f, 0.0f, 0.5f} - ray.position;
        REQUIRE(ray.intersects(box));

        ray.position = Vector3::createUnitZ();
        ray.direction = Vector3{0.5f, 0.5f, 0.0f} - ray.position;
        REQUIRE(ray.intersects(box));

        ray.position = -Vector3::createUnitZ();
        ray.direction = Vector3{0.5f, 0.5f, 0.0f} - ray.position;
        REQUIRE(ray.intersects(box));

        //
        ray.position = Vector3::createUnitX();
        ray.direction = Vector3::createUnitX();
        REQUIRE_FALSE(ray.intersects(box));

        ray.position = -Vector3::createUnitX();
        ray.direction = -Vector3::createUnitX();
        REQUIRE_FALSE(ray.intersects(box));

        ray.position = Vector3::createUnitY();
        ray.direction = Vector3::createUnitY();
        REQUIRE_FALSE(ray.intersects(box));

        ray.position = -Vector3::createUnitY();
        ray.direction = -Vector3::createUnitY();
        REQUIRE_FALSE(ray.intersects(box));

        ray.position = Vector3::createUnitZ();
        ray.direction = Vector3::createUnitZ();
        REQUIRE_FALSE(ray.intersects(box));

        ray.position = -Vector3::createUnitZ();
        ray.direction = -Vector3::createUnitZ();
        REQUIRE_FALSE(ray.intersects(box));

        //
        ray.direction = -Vector3::createUnitY();
        ray.position = Vector3{-0.5f - Epsilon, 1.0f, 0.0f};
        REQUIRE_FALSE(ray.intersects(box));
        ray.position = Vector3{-0.5f + Epsilon, 1.0f, 0.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{0.0f, 1.0f, 0.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{0.5f - Epsilon, 1.0f, 0.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{0.5f + Epsilon, 1.0f, 0.0f};
        REQUIRE_FALSE(ray.intersects(box));

        ray.direction = Vector3::createUnitY();
        ray.position = Vector3{-0.5f - Epsilon, -1.0f, 0.0f};
        REQUIRE_FALSE(ray.intersects(box));
        ray.position = Vector3{-0.5f + Epsilon, -1.0f, 0.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{0.0f, -1.0f, 0.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{0.5f - Epsilon, -1.0f, 0.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{0.5f + Epsilon, -1.0f, 0.0f};
        REQUIRE_FALSE(ray.intersects(box));

        ray.direction = -Vector3::createUnitZ();
        ray.position = Vector3{0.0f, -0.5f - Epsilon, 1.0f};
        REQUIRE_FALSE(ray.intersects(box));
        ray.position = Vector3{0.0f, -0.5f + Epsilon, 1.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{0.0f, 0.0f, 1.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{0.0f, 0.5f - Epsilon, 1.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{0.0f, 0.5f + Epsilon, 1.0f};
        REQUIRE_FALSE(ray.intersects(box));

        ray.direction = Vector3::createUnitZ();
        ray.position = Vector3{0.0f, -0.5f - Epsilon, -1.0f};
        REQUIRE_FALSE(ray.intersects(box));
        ray.position = Vector3{0.0f, -0.5f + Epsilon, -1.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{0.0f, 0.0f, -1.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{0.0f, 0.5f - Epsilon, -1.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{0.0f, 0.5f + Epsilon, -1.0f};
        REQUIRE_FALSE(ray.intersects(box));

        ray.direction = -Vector3::createUnitX();
        ray.position = Vector3{1.0f, 0.0f, -0.5f - Epsilon};
        REQUIRE_FALSE(ray.intersects(box));
        ray.position = Vector3{1.0f, 0.0f, -0.5f + Epsilon};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{1.0f, 0.0f, 0.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{1.0f, 0.0f, 0.5f - Epsilon};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{1.0f, 0.0f, 0.5f + Epsilon};
        REQUIRE_FALSE(ray.intersects(box));

        ray.direction = Vector3::createUnitX();
        ray.position = Vector3{-1.0f, 0.0f, -0.5f - Epsilon};
        REQUIRE_FALSE(ray.intersects(box));
        ray.position = Vector3{-1.0f, 0.0f, -0.5f + Epsilon};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{-1.0f, 0.0f, 0.0f};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{-1.0f, 0.0f, 0.5f - Epsilon};
        REQUIRE(ray.intersects(box));
        ray.position = Vector3{-1.0f, 0.0f, 0.5f + Epsilon};
        REQUIRE_FALSE(ray.intersects(box));
    }
    SECTION("Intersects_BoundingSphere_1")
    {
        Ray const data[] = {
            {Vector3::createZero(), Vector3::createZero()},
            {Vector3::createZero(), Vector3::createUnitX()},
            {Vector3::createZero(), Vector3::createUnitY()},
            {Vector3::createZero(), Vector3::createUnitZ()},
            {Vector3::createZero(), -Vector3::createUnitX()},
            {Vector3::createZero(), -Vector3::createUnitY()},
            {Vector3::createZero(), -Vector3::createUnitZ()},
        };

        BoundingSphere sphere;
        sphere.center = Vector3::createZero();
        sphere.radius = Epsilon;
        for (auto ray : data) {
            REQUIRE(ray.intersects(sphere));
        }

        sphere.center = Vector3::createZero();
        sphere.radius = 1.0f;
        for (auto ray : data) {
            REQUIRE(ray.intersects(sphere));
        }

        sphere.center = Vector3::createZero();
        sphere.radius = std::numeric_limits<float>::max() / 2;
        for (auto ray : data) {
            REQUIRE(ray.intersects(sphere));
        }
    }
    SECTION("Intersects_BoundingSphere_2")
    {
        Ray const data[] = {
            {Vector3{1.0f, 1.0f, 1.0f}, Vector3::createZero()},
            {Vector3{1.0f, 1.0f, 0.0f}, Vector3::createZero()},
            {Vector3{1.0f, 0.0f, 1.0f}, Vector3::createZero()},
            {Vector3{1.0f, 0.0f, 0.0f}, Vector3::createZero()},
            {Vector3{0.0f, 1.0f, 1.0f}, Vector3::createZero()},
            {Vector3{0.0f, 1.0f, 0.0f}, Vector3::createZero()},
            {Vector3{0.0f, 0.0f, 1.0f}, Vector3::createZero()},
        };

        BoundingSphere sphere;
        sphere.center = Vector3::createZero();
        sphere.radius = Epsilon;
        for (auto ray : data) {
            REQUIRE_FALSE(ray.intersects(sphere));
        }

        sphere.center = Vector3::createZero();
        sphere.radius = 1.0f - Epsilon;
        for (auto ray : data) {
            ray.position = math::normalize(ray.position);
            REQUIRE_FALSE(ray.intersects(sphere));
        }

        sphere.center = Vector3::createZero();
        sphere.radius = 1.0f + Epsilon;
        for (auto ray : data) {
            ray.position = math::normalize(ray.position);
            REQUIRE(ray.intersects(sphere));
        }
    }
    SECTION("Intersects_BoundingSphere_3")
    {
        BoundingSphere sphere;
        sphere.center = Vector3::createZero();
        sphere.radius = 0.5f;

        Ray ray;
        ray.position = Vector3::createUnitX();
        ray.direction = -Vector3::createUnitX();
        REQUIRE(ray.intersects(sphere));

        ray.position = -Vector3::createUnitX();
        ray.direction = Vector3::createUnitX();
        REQUIRE(ray.intersects(sphere));

        ray.position = Vector3::createUnitY();
        ray.direction = -Vector3::createUnitY();
        REQUIRE(ray.intersects(sphere));

        ray.position = -Vector3::createUnitY();
        ray.direction = Vector3::createUnitY();
        REQUIRE(ray.intersects(sphere));

        ray.position = Vector3::createUnitZ();
        ray.direction = -Vector3::createUnitZ();
        REQUIRE(ray.intersects(sphere));

        ray.position = -Vector3::createUnitZ();
        ray.direction = Vector3::createUnitZ();
        REQUIRE(ray.intersects(sphere));

        //
        ray.position = Vector3::createUnitX();
        ray.direction = Vector3{0.0f, 0.5f, 0.5f} - ray.position;
        REQUIRE(ray.intersects(sphere));

        ray.position = -Vector3::createUnitX();
        ray.direction = Vector3{0.0f, 0.5f, 0.5f} - ray.position;
        REQUIRE(ray.intersects(sphere));

        ray.position = Vector3::createUnitY();
        ray.direction = Vector3{0.5f, 0.0f, 0.5f} - ray.position;
        REQUIRE(ray.intersects(sphere));

        ray.position = -Vector3::createUnitY();
        ray.direction = Vector3{0.5f, 0.0f, 0.5f} - ray.position;
        REQUIRE(ray.intersects(sphere));

        ray.position = Vector3::createUnitZ();
        ray.direction = Vector3{0.5f, 0.5f, 0.0f} - ray.position;
        REQUIRE(ray.intersects(sphere));

        ray.position = -Vector3::createUnitZ();
        ray.direction = Vector3{0.5f, 0.5f, 0.0f} - ray.position;
        REQUIRE(ray.intersects(sphere));

        //
        ray.position = Vector3::createUnitX();
        ray.direction = Vector3::createUnitX();
        REQUIRE_FALSE(ray.intersects(sphere));

        ray.position = -Vector3::createUnitX();
        ray.direction = -Vector3::createUnitX();
        REQUIRE_FALSE(ray.intersects(sphere));

        ray.position = Vector3::createUnitY();
        ray.direction = Vector3::createUnitY();
        REQUIRE_FALSE(ray.intersects(sphere));

        ray.position = -Vector3::createUnitY();
        ray.direction = -Vector3::createUnitY();
        REQUIRE_FALSE(ray.intersects(sphere));

        ray.position = Vector3::createUnitZ();
        ray.direction = Vector3::createUnitZ();
        REQUIRE_FALSE(ray.intersects(sphere));

        ray.position = -Vector3::createUnitZ();
        ray.direction = -Vector3::createUnitZ();
        REQUIRE_FALSE(ray.intersects(sphere));

        //
        ray.direction = -Vector3::createUnitY();
        ray.position = Vector3{-0.5f - Epsilon, 1.0f, 0.0f};
        REQUIRE_FALSE(ray.intersects(sphere));
        ray.position = Vector3{-0.5f + Epsilon, 1.0f, 0.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{0.0f, 1.0f, 0.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{0.5f - Epsilon, 1.0f, 0.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{0.5f + Epsilon, 1.0f, 0.0f};
        REQUIRE_FALSE(ray.intersects(sphere));

        ray.direction = Vector3::createUnitY();
        ray.position = Vector3{-0.5f - Epsilon, -1.0f, 0.0f};
        REQUIRE_FALSE(ray.intersects(sphere));
        ray.position = Vector3{-0.5f + Epsilon, -1.0f, 0.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{0.0f, -1.0f, 0.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{0.5f - Epsilon, -1.0f, 0.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{0.5f + Epsilon, -1.0f, 0.0f};
        REQUIRE_FALSE(ray.intersects(sphere));

        ray.direction = -Vector3::createUnitZ();
        ray.position = Vector3{0.0f, -0.5f - Epsilon, 1.0f};
        REQUIRE_FALSE(ray.intersects(sphere));
        ray.position = Vector3{0.0f, -0.5f + Epsilon, 1.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{0.0f, 0.0f, 1.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{0.0f, 0.5f - Epsilon, 1.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{0.0f, 0.5f + Epsilon, 1.0f};
        REQUIRE_FALSE(ray.intersects(sphere));

        ray.direction = Vector3::createUnitZ();
        ray.position = Vector3{0.0f, -0.5f - Epsilon, -1.0f};
        REQUIRE_FALSE(ray.intersects(sphere));
        ray.position = Vector3{0.0f, -0.5f + Epsilon, -1.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{0.0f, 0.0f, -1.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{0.0f, 0.5f - Epsilon, -1.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{0.0f, 0.5f + Epsilon, -1.0f};
        REQUIRE_FALSE(ray.intersects(sphere));

        ray.direction = -Vector3::createUnitX();
        ray.position = Vector3{1.0f, 0.0f, -0.5f - Epsilon};
        REQUIRE_FALSE(ray.intersects(sphere));
        ray.position = Vector3{1.0f, 0.0f, -0.5f + Epsilon};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{1.0f, 0.0f, 0.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{1.0f, 0.0f, 0.5f - Epsilon};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{1.0f, 0.0f, 0.5f + Epsilon};
        REQUIRE_FALSE(ray.intersects(sphere));

        ray.direction = Vector3::createUnitX();
        ray.position = Vector3{-1.0f, 0.0f, -0.5f - Epsilon};
        REQUIRE_FALSE(ray.intersects(sphere));
        ray.position = Vector3{-1.0f, 0.0f, -0.5f + Epsilon};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{-1.0f, 0.0f, 0.0f};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{-1.0f, 0.0f, 0.5f - Epsilon};
        REQUIRE(ray.intersects(sphere));
        ray.position = Vector3{-1.0f, 0.0f, 0.5f + Epsilon};
        REQUIRE_FALSE(ray.intersects(sphere));
    }
}
