// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/containment_type.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::BoundingBox;
using pomdog::BoundingSphere;
using pomdog::ContainmentType;
using pomdog::Vector3;

TEST_CASE("BoundingSphere")
{
    SUBCASE("constructors default")
    {
        // NOTE: Default constructor does not zero-initialize members
        BoundingSphere sphere = {};
        sphere.center = Vector3{1.0f, 2.0f, 3.0f};
        sphere.radius = 4.0f;
        REQUIRE(sphere.center.x == 1.0f);
        REQUIRE(sphere.center.y == 2.0f);
        REQUIRE(sphere.center.z == 3.0f);
        REQUIRE(sphere.radius == 4.0f);
    }
    SUBCASE("constructors with Vector3")
    {
        BoundingSphere sphere{Vector3{1.0f, 2.0f, 3.0f}, 4.0f};
        REQUIRE(sphere.center.x == 1.0f);
        REQUIRE(sphere.center.y == 2.0f);
        REQUIRE(sphere.center.z == 3.0f);
        REQUIRE(sphere.radius == 4.0f);
    }
    SUBCASE("contains Vector3")
    {
        BoundingSphere sphere;
        sphere.center = Vector3{10.0f, 10.0f, 10.0f};
        sphere.radius = 5.0f;

        // NOTE: Point at center is strictly inside
        REQUIRE(sphere.contains(Vector3{10.0f, 10.0f, 10.0f}) == ContainmentType::Contains);

        // NOTE: Points on the boundary return Intersects
        REQUIRE(sphere.contains(Vector3{10.0f, 10.0f, 15.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(Vector3{10.0f, 10.0f, 5.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(Vector3{15.0f, 10.0f, 10.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(Vector3{5.0f, 10.0f, 10.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(Vector3{10.0f, 15.0f, 10.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(Vector3{10.0f, 5.0f, 10.0f}) == ContainmentType::Intersects);

        // NOTE: Points outside
        REQUIRE(sphere.contains(Vector3{10.0f, 10.0f, 15.1f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(Vector3{10.0f, 10.0f, 4.9f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(Vector3{15.1f, 10.0f, 10.0f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(Vector3{4.9f, 10.0f, 10.0f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(Vector3{10.0f, 15.1f, 10.0f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(Vector3{10.0f, 4.9f, 10.0f}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains Vector3")
    {
        BoundingSphere sphere;
        sphere.center = Vector3::createZero();
        sphere.radius = 42.0f;

        REQUIRE(sphere.contains(Vector3::createZero()) == ContainmentType::Contains);
        REQUIRE(sphere.contains(Vector3{41.0f, 0.0f, 0.0f}) == ContainmentType::Contains);
        REQUIRE(sphere.contains(Vector3{0.0f, 41.0f, 0.0f}) == ContainmentType::Contains);
        REQUIRE(sphere.contains(Vector3{0.0f, 0.0f, 41.0f}) == ContainmentType::Contains);
        REQUIRE(sphere.contains(Vector3{-41.0f, 0.0f, 0.0f}) == ContainmentType::Contains);
        REQUIRE(sphere.contains(Vector3{0.0f, -41.0f, 0.0f}) == ContainmentType::Contains);
        REQUIRE(sphere.contains(Vector3{0.0f, 0.0f, -41.0f}) == ContainmentType::Contains);

        REQUIRE(sphere.contains(Vector3{42.0f, 0.0f, 0.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(Vector3{0.0f, 42.0f, 0.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(Vector3{0.0f, 0.0f, 42.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(Vector3{-42.0f, 0.0f, 0.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(Vector3{0.0f, -42.0f, 0.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(Vector3{0.0f, 0.0f, -42.0f}) == ContainmentType::Intersects);

        REQUIRE(sphere.contains(Vector3{43.0f, 0.0f, 0.0f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(Vector3{0.0f, 43.0f, 0.0f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(Vector3{0.0f, 0.0f, 43.0f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(Vector3{-43.0f, 0.0f, 0.0f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(Vector3{0.0f, -43.0f, 0.0f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(Vector3{0.0f, 0.0f, -43.0f}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingSphere")
    {
        BoundingSphere sphere;
        sphere.center = Vector3{10.0f, 10.0f, 10.0f};
        sphere.radius = 5.0f;

        // NOTE: Same sphere: boundary touches, returns Intersects
        REQUIRE(sphere.contains(sphere) == ContainmentType::Intersects);

        REQUIRE(sphere.contains(BoundingSphere{Vector3{10.0f, 10.0f, 10.0f}, 3.0f}) == ContainmentType::Contains);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{10.0f, 10.0f, 10.0f}, 5.0f}) == ContainmentType::Intersects);

        REQUIRE(sphere.contains(BoundingSphere{Vector3{10.0f, 10.0f, 13.0f}, 2.0f}) == ContainmentType::Intersects);

        REQUIRE(sphere.contains(BoundingSphere{Vector3{10.0f, 10.0f, 10.0f}, 6.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{10.0f, 10.0f, 15.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{10.0f, 10.0f, 5.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{15.0f, 10.0f, 10.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{5.0f, 10.0f, 10.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{10.0f, 15.0f, 10.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{10.0f, 5.0f, 10.0f}, 1.0f}) == ContainmentType::Intersects);

        REQUIRE(sphere.contains(BoundingSphere{Vector3{100.0f, 100.0f, 100.0f}, 1.0f}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingSphere")
    {
        BoundingSphere sphere;
        sphere.center = Vector3::createZero();
        sphere.radius = 42.0f;

        REQUIRE(sphere.contains(sphere) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3::createZero(), 43.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3::createZero(), 41.0f}) == ContainmentType::Contains);

        REQUIRE(sphere.contains(BoundingSphere{Vector3{40.0f, 0.0f, 0.0f}, 1.9f}) == ContainmentType::Contains);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, 40.0f, 0.0f}, 1.9f}) == ContainmentType::Contains);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, 0.0f, 40.0f}, 1.9f}) == ContainmentType::Contains);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{-40.0f, 0.0f, 0.0f}, 1.9f}) == ContainmentType::Contains);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, -40.0f, 0.0f}, 1.9f}) == ContainmentType::Contains);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, 0.0f, -40.0f}, 1.9f}) == ContainmentType::Contains);

        REQUIRE(sphere.contains(BoundingSphere{Vector3{42.0f, 0.0f, 0.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, 42.0f, 0.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, 0.0f, 42.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{-42.0f, 0.0f, 0.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, -42.0f, 0.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, 0.0f, -42.0f}, 1.0f}) == ContainmentType::Intersects);

        REQUIRE(sphere.contains(BoundingSphere{Vector3{43.0f, 0.0f, 0.0f}, 2.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, 43.0f, 0.0f}, 2.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, 0.0f, 43.0f}, 2.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{-43.0f, 0.0f, 0.0f}, 2.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, -43.0f, 0.0f}, 2.0f}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, 0.0f, -43.0f}, 2.0f}) == ContainmentType::Intersects);

        REQUIRE(sphere.contains(BoundingSphere{Vector3{43.0f, 0.0f, 0.0f}, 0.5f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, 43.0f, 0.0f}, 0.5f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, 0.0f, 43.0f}, 0.5f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{-43.0f, 0.0f, 0.0f}, 0.5f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, -43.0f, 0.0f}, 0.5f}) == ContainmentType::Disjoint);
        REQUIRE(sphere.contains(BoundingSphere{Vector3{0.0f, 0.0f, -43.0f}, 0.5f}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingBox")
    {
        BoundingSphere sphere;
        sphere.center = Vector3{10.0f, 10.0f, 10.0f};
        sphere.radius = 5.0f;

        REQUIRE(sphere.contains(BoundingBox{Vector3{9.0f, 9.0f, 9.0f}, Vector3{11.0f, 11.0f, 11.0f}}) == ContainmentType::Contains);

        REQUIRE(sphere.contains(BoundingBox{Vector3{7.0f, 7.0f, 7.0f}, Vector3{13.0f, 13.0f, 13.0f}}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingBox{Vector3{7.0f, 7.0f, 7.0f}, Vector3{15.0f, 15.0f, 15.0f}}) == ContainmentType::Intersects);
        REQUIRE(sphere.contains(BoundingBox{Vector3{13.0f, 10.0f, 10.0f}, Vector3{20.0f, 15.0f, 15.0f}}) == ContainmentType::Intersects);

        REQUIRE(sphere.contains(BoundingBox{Vector3{100.0f, 100.0f, 100.0f}, Vector3{105.0f, 105.0f, 105.0f}}) == ContainmentType::Disjoint);
    }
    SUBCASE("intersects BoundingSphere")
    {
        BoundingSphere sphere;
        sphere.center = Vector3{10.0f, 10.0f, 10.0f};
        sphere.radius = 5.0f;

        REQUIRE(sphere.intersects(sphere));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{10.0f, 10.0f, 10.0f}, 3.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{10.0f, 10.0f, 10.0f}, 5.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{10.0f, 10.0f, 13.0f}, 2.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{10.0f, 10.0f, 10.0f}, 6.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{10.0f, 10.0f, 15.0f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{10.0f, 10.0f, 5.0f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{15.0f, 10.0f, 10.0f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{5.0f, 10.0f, 10.0f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{10.0f, 15.0f, 10.0f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{10.0f, 5.0f, 10.0f}, 1.0f}));

        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{100.0f, 100.0f, 100.0f}, 1.0f}));
    }
    SUBCASE("intersects BoundingSphere")
    {
        BoundingSphere sphere;
        sphere.center = Vector3::createZero();
        sphere.radius = 42.0f;

        REQUIRE(sphere.intersects(sphere));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3::createZero(), 43.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3::createZero(), 41.0f}));

        REQUIRE(sphere.intersects(BoundingSphere{Vector3{40.0f, 0.0f, 0.0f}, 1.9f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.0f, 40.0f, 0.0f}, 1.9f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.0f, 0.0f, 40.0f}, 1.9f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{-40.0f, 0.0f, 0.0f}, 1.9f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.0f, -40.0f, 0.0f}, 1.9f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.0f, 0.0f, -40.0f}, 1.9f}));

        REQUIRE(sphere.intersects(BoundingSphere{Vector3{42.0f, 0.0f, 0.0f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.0f, 42.0f, 0.0f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.0f, 0.0f, 42.0f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{-42.0f, 0.0f, 0.0f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.0f, -42.0f, 0.0f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.0f, 0.0f, -42.0f}, 1.0f}));

        REQUIRE(sphere.intersects(BoundingSphere{Vector3{43.0f, 0.0f, 0.0f}, 2.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.0f, 43.0f, 0.0f}, 2.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.0f, 0.0f, 43.0f}, 2.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{-43.0f, 0.0f, 0.0f}, 2.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.0f, -43.0f, 0.0f}, 2.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.0f, 0.0f, -43.0f}, 2.0f}));

        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{43.0f, 0.0f, 0.0f}, 0.5f}));
        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{0.0f, 43.0f, 0.0f}, 0.5f}));
        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{0.0f, 0.0f, 43.0f}, 0.5f}));
        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{-43.0f, 0.0f, 0.0f}, 0.5f}));
        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{0.0f, -43.0f, 0.0f}, 0.5f}));
        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{0.0f, 0.0f, -43.0f}, 0.5f}));
    }
    SUBCASE("intersects BoundingBox")
    {
        BoundingSphere sphere;
        sphere.center = Vector3{10.0f, 10.0f, 10.0f};
        sphere.radius = 5.0f;

        REQUIRE(sphere.intersects(BoundingBox{Vector3{9.0f, 9.0f, 9.0f}, Vector3{11.0f, 11.0f, 11.0f}}));
        REQUIRE(sphere.intersects(BoundingBox{Vector3{7.0f, 7.0f, 7.0f}, Vector3{13.0f, 13.0f, 13.0f}}));
        REQUIRE(sphere.intersects(BoundingBox{Vector3{7.0f, 7.0f, 7.0f}, Vector3{15.0f, 15.0f, 15.0f}}));
        REQUIRE(sphere.intersects(BoundingBox{Vector3{13.0f, 10.0f, 10.0f}, Vector3{20.0f, 15.0f, 15.0f}}));

        REQUIRE_FALSE(sphere.intersects(BoundingBox{Vector3{100.0f, 100.0f, 100.0f}, Vector3{105.0f, 105.0f, 105.0f}}));
    }
}
