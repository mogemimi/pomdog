// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/plane.h"
#include "pomdog/math/ray.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::BoundingBox;
using pomdog::BoundingSphere;
using pomdog::Plane;
using pomdog::Ray;
using pomdog::Vector3;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("Ray")
{
    SUBCASE("constructor")
    {
        Ray ray;
        ray.position = Vector3{3.0f, 4.0f, 7.0f};
        ray.direction = Vector3{5.0f, 8.0f, 9.0f};
        REQUIRE(Vector3{3.0f, 4.0f, 7.0f} == ray.position);
        REQUIRE(Vector3{5.0f, 8.0f, 9.0f} == ray.direction);
    }
    SUBCASE("intersects BoundingBox")
    {
        BoundingBox box;
        box.min = Vector3{-1.0f, -1.0f, -1.0f};
        box.max = Vector3{1.0f, 1.0f, 1.0f};

        Ray ray;
        std::optional<float> result;

        ray = Ray{Vector3{0.0f, 0.0f, -5.0f}, Vector3{0.0f, 0.0f, 1.0f}};
        result = ray.intersects(box);
        REQUIRE(result.has_value());
        REQUIRE(*result == approx(4.0f));

        ray = Ray{Vector3{0.0f, 0.0f, 5.0f}, Vector3{0.0f, 0.0f, -1.0f}};
        result = ray.intersects(box);
        REQUIRE(result.has_value());
        REQUIRE(*result == approx(4.0f));

        // When ray origin is inside the box, tNear is negative (distance to back face)
        ray = Ray{Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 1.0f}};
        result = ray.intersects(box);
        REQUIRE(result.has_value());
        REQUIRE(*result == approx(-1.0f));

        ray = Ray{Vector3{0.0f, 0.0f, -5.0f}, Vector3{0.0f, 0.0f, -1.0f}};
        result = ray.intersects(box);
        REQUIRE_FALSE(result.has_value());
    }
    SUBCASE("intersects BoundingSphere")
    {
        BoundingSphere sphere;
        sphere.center = Vector3{0.0f, 0.0f, 0.0f};
        sphere.radius = 1.0f;

        Ray ray;
        std::optional<float> result;

        ray = Ray{Vector3{0.0f, 0.0f, -5.0f}, Vector3{0.0f, 0.0f, 1.0f}};
        result = ray.intersects(sphere);
        REQUIRE(result.has_value());
        REQUIRE(*result == approx(4.0f));

        ray = Ray{Vector3{0.0f, 0.0f, 5.0f}, Vector3{0.0f, 0.0f, -1.0f}};
        result = ray.intersects(sphere);
        REQUIRE(result.has_value());
        REQUIRE(*result == approx(4.0f));

        ray = Ray{Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 1.0f}};
        result = ray.intersects(sphere);
        REQUIRE(result.has_value());
        REQUIRE(*result == approx(0.0f));

        ray = Ray{Vector3{0.0f, 0.0f, -5.0f}, Vector3{0.0f, 0.0f, -1.0f}};
        result = ray.intersects(sphere);
        REQUIRE_FALSE(result.has_value());
    }
    SUBCASE("intersects Plane")
    {
        // Plane at z=0 with normal pointing toward -z
        Plane plane;
        plane.normal = Vector3{0.0f, 0.0f, -1.0f};
        plane.distance = 0.0f;

        Ray ray;
        std::optional<float> result;

        // Ray from z=-5 toward +z: intersects plane at z=0, distance=5
        ray = Ray{Vector3{0.0f, 0.0f, -5.0f}, Vector3{0.0f, 0.0f, 1.0f}};
        result = ray.intersects(plane);
        REQUIRE(result.has_value());
        REQUIRE(*result == approx(5.0f));

        // Ray from z=5 toward -z: intersects plane at z=0, distance=5
        ray = Ray{Vector3{0.0f, 0.0f, 5.0f}, Vector3{0.0f, 0.0f, -1.0f}};
        result = ray.intersects(plane);
        REQUIRE(result.has_value());
        REQUIRE(*result == approx(5.0f));

        // Ray starting on plane going toward +z
        ray = Ray{Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 1.0f}};
        result = ray.intersects(plane);
        REQUIRE(result.has_value());
        REQUIRE(*result == approx(0.0f));

        // Ray from z=-5 toward -z: moving away from plane (behind ray origin)
        ray = Ray{Vector3{0.0f, 0.0f, -5.0f}, Vector3{0.0f, 0.0f, -1.0f}};
        result = ray.intersects(plane);
        REQUIRE_FALSE(result.has_value());
    }
}
