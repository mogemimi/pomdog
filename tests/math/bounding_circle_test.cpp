// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/bounding_box2d.h"
#include "pomdog/math/bounding_circle.h"
#include "pomdog/math/containment_type.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::BoundingBox2D;
using pomdog::BoundingCircle;
using pomdog::ContainmentType;
using pomdog::Vector2;

TEST_CASE("BoundingCircle")
{
    SUBCASE("default constructor")
    {
        // NOTE: Default constructor does not zero-initialize members
        BoundingCircle circle = {};
        circle.center = Vector2{1.0f, 2.0f};
        circle.radius = 3.0f;
        REQUIRE(circle.center.x == 1.0f);
        REQUIRE(circle.center.y == 2.0f);
        REQUIRE(circle.radius == 3.0f);
    }
    SUBCASE("constructor with Vector2")
    {
        BoundingCircle circle{Vector2{1.0f, 2.0f}, 3.0f};
        REQUIRE(circle.center.x == 1.0f);
        REQUIRE(circle.center.y == 2.0f);
        REQUIRE(circle.radius == 3.0f);
    }
    SUBCASE("contains Vector2")
    {
        BoundingCircle circle;
        circle.center = Vector2{10.0f, 10.0f};
        circle.radius = 5.0f;

        // NOTE: Points strictly inside the circle
        REQUIRE(circle.contains(Vector2{10.0f, 10.0f}) == ContainmentType::Contains);
        REQUIRE(circle.contains(Vector2{10.0f, 14.0f}) == ContainmentType::Contains);
        REQUIRE(circle.contains(Vector2{10.0f, 6.0f}) == ContainmentType::Contains);
        REQUIRE(circle.contains(Vector2{14.0f, 10.0f}) == ContainmentType::Contains);
        REQUIRE(circle.contains(Vector2{6.0f, 10.0f}) == ContainmentType::Contains);

        // NOTE: Points exactly on the boundary return Intersects
        REQUIRE(circle.contains(Vector2{10.0f, 15.0f}) == ContainmentType::Intersects);
        REQUIRE(circle.contains(Vector2{10.0f, 5.0f}) == ContainmentType::Intersects);
        REQUIRE(circle.contains(Vector2{15.0f, 10.0f}) == ContainmentType::Intersects);
        REQUIRE(circle.contains(Vector2{5.0f, 10.0f}) == ContainmentType::Intersects);

        // NOTE: Points strictly outside the circle
        REQUIRE(circle.contains(Vector2{10.0f, 15.1f}) == ContainmentType::Disjoint);
        REQUIRE(circle.contains(Vector2{10.0f, 4.9f}) == ContainmentType::Disjoint);
        REQUIRE(circle.contains(Vector2{15.1f, 10.0f}) == ContainmentType::Disjoint);
        REQUIRE(circle.contains(Vector2{4.9f, 10.0f}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingCircle")
    {
        BoundingCircle circle;
        circle.center = Vector2{10.0f, 10.0f};
        circle.radius = 5.0f;

        // NOTE: Same circle returns Intersects (boundary touches)
        REQUIRE(circle.contains(circle) == ContainmentType::Intersects);
        REQUIRE(circle.contains(BoundingCircle{Vector2{10.0f, 10.0f}, 3.0f}) == ContainmentType::Contains);

        // NOTE: Same radius at same center: boundary touches, returns Intersects
        REQUIRE(circle.contains(BoundingCircle{Vector2{10.0f, 10.0f}, 5.0f}) == ContainmentType::Intersects);
        REQUIRE(circle.contains(BoundingCircle{Vector2{10.0f, 13.0f}, 2.0f}) == ContainmentType::Intersects);

        REQUIRE(circle.contains(BoundingCircle{Vector2{10.0f, 10.0f}, 6.0f}) == ContainmentType::Intersects);
        REQUIRE(circle.contains(BoundingCircle{Vector2{10.0f, 15.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(circle.contains(BoundingCircle{Vector2{10.0f, 5.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(circle.contains(BoundingCircle{Vector2{15.0f, 10.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(circle.contains(BoundingCircle{Vector2{5.0f, 10.0f}, 1.0f}) == ContainmentType::Intersects);

        REQUIRE(circle.contains(BoundingCircle{Vector2{0.0f, 0.0f}, 2.0f}) == ContainmentType::Disjoint);
        REQUIRE(circle.contains(BoundingCircle{Vector2{0.0f, 20.0f}, 2.0f}) == ContainmentType::Disjoint);
        REQUIRE(circle.contains(BoundingCircle{Vector2{20.0f, 0.0f}, 2.0f}) == ContainmentType::Disjoint);
        REQUIRE(circle.contains(BoundingCircle{Vector2{20.0f, 20.0f}, 2.0f}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingBox2D")
    {
        BoundingCircle circle;
        circle.center = Vector2{10.0f, 10.0f};
        circle.radius = 5.0f;

        REQUIRE(circle.contains(BoundingBox2D{Vector2{9.0f, 9.0f}, Vector2{11.0f, 11.0f}}) == ContainmentType::Contains);
        REQUIRE(circle.contains(BoundingBox2D{Vector2{7.0f, 7.0f}, Vector2{13.0f, 13.0f}}) == ContainmentType::Contains);

        REQUIRE(circle.contains(BoundingBox2D{Vector2{7.0f, 7.0f}, Vector2{15.0f, 15.0f}}) == ContainmentType::Intersects);
        REQUIRE(circle.contains(BoundingBox2D{Vector2{13.0f, 10.0f}, Vector2{20.0f, 15.0f}}) == ContainmentType::Intersects);

        REQUIRE(circle.contains(BoundingBox2D{Vector2{100.0f, 100.0f}, Vector2{105.0f, 105.0f}}) == ContainmentType::Disjoint);
    }
    SUBCASE("intersects BoundingCircle")
    {
        BoundingCircle circle;
        circle.center = Vector2{10.0f, 10.0f};
        circle.radius = 5.0f;

        REQUIRE(circle.intersects(circle));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{10.0f, 10.0f}, 3.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{10.0f, 10.0f}, 5.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{10.0f, 13.0f}, 2.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{10.0f, 10.0f}, 6.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{10.0f, 15.0f}, 1.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{10.0f, 5.0f}, 1.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{15.0f, 10.0f}, 1.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{5.0f, 10.0f}, 1.0f}));

        REQUIRE_FALSE(circle.intersects(BoundingCircle{Vector2{0.0f, 0.0f}, 2.0f}));
        REQUIRE_FALSE(circle.intersects(BoundingCircle{Vector2{0.0f, 20.0f}, 2.0f}));
        REQUIRE_FALSE(circle.intersects(BoundingCircle{Vector2{20.0f, 0.0f}, 2.0f}));
        REQUIRE_FALSE(circle.intersects(BoundingCircle{Vector2{20.0f, 20.0f}, 2.0f}));
    }
    SUBCASE("intersects BoundingBox2D")
    {
        BoundingCircle circle;
        circle.center = Vector2{10.0f, 10.0f};
        circle.radius = 5.0f;

        REQUIRE(circle.intersects(BoundingBox2D{Vector2{9.0f, 9.0f}, Vector2{11.0f, 11.0f}}));
        REQUIRE(circle.intersects(BoundingBox2D{Vector2{7.0f, 7.0f}, Vector2{13.0f, 13.0f}}));
        REQUIRE(circle.intersects(BoundingBox2D{Vector2{7.0f, 7.0f}, Vector2{15.0f, 15.0f}}));
        REQUIRE(circle.intersects(BoundingBox2D{Vector2{13.0f, 10.0f}, Vector2{20.0f, 15.0f}}));

        REQUIRE_FALSE(circle.intersects(BoundingBox2D{Vector2{100.0f, 100.0f}, Vector2{105.0f, 105.0f}}));
    }
}
