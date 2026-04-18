// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/bounding_box_xz.h"
#include "pomdog/math/bounding_circle_xz.h"
#include "pomdog/math/containment_type.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::BoundingBoxXZ;
using pomdog::BoundingCircleXZ;
using pomdog::ContainmentType;
using pomdog::VectorXZ;

TEST_CASE("BoundingCircleXZ")
{
    SUBCASE("constructor")
    {
        BoundingCircleXZ circle{VectorXZ{1.0f, 2.0f}, 3.0f};
        REQUIRE(circle.center.x == 1.0f);
        REQUIRE(circle.center.z == 2.0f);
        REQUIRE(circle.radius == 3.0f);
    }
    SUBCASE("contains VectorXZ")
    {
        BoundingCircleXZ circle;
        circle.center = VectorXZ{10.0f, 10.0f};
        circle.radius = 5.0f;

        REQUIRE(circle.contains(VectorXZ{10.0f, 10.0f}) == ContainmentType::Contains);
        REQUIRE(circle.contains(VectorXZ{10.0f, 14.0f}) == ContainmentType::Contains);

        REQUIRE(circle.contains(VectorXZ{10.0f, 15.0f}) == ContainmentType::Intersects);
        REQUIRE(circle.contains(VectorXZ{15.0f, 10.0f}) == ContainmentType::Intersects);

        REQUIRE(circle.contains(VectorXZ{10.0f, 15.1f}) == ContainmentType::Disjoint);
        REQUIRE(circle.contains(VectorXZ{15.1f, 10.0f}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingCircleXZ")
    {
        BoundingCircleXZ circle;
        circle.center = VectorXZ{10.0f, 10.0f};
        circle.radius = 5.0f;

        REQUIRE(circle.contains(circle) == ContainmentType::Intersects);
        REQUIRE(circle.contains(BoundingCircleXZ{VectorXZ{10.0f, 10.0f}, 3.0f}) == ContainmentType::Contains);
        REQUIRE(circle.contains(BoundingCircleXZ{VectorXZ{0.0f, 0.0f}, 2.0f}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingBoxXZ")
    {
        BoundingCircleXZ circle;
        circle.center = VectorXZ{10.0f, 10.0f};
        circle.radius = 5.0f;

        REQUIRE(circle.contains(BoundingBoxXZ{VectorXZ{9.0f, 9.0f}, VectorXZ{11.0f, 11.0f}}) == ContainmentType::Contains);
        REQUIRE(circle.contains(BoundingBoxXZ{VectorXZ{7.0f, 7.0f}, VectorXZ{15.0f, 15.0f}}) == ContainmentType::Intersects);
        REQUIRE(circle.contains(BoundingBoxXZ{VectorXZ{100.0f, 100.0f}, VectorXZ{105.0f, 105.0f}}) == ContainmentType::Disjoint);
    }
    SUBCASE("intersects BoundingCircleXZ")
    {
        BoundingCircleXZ circle;
        circle.center = VectorXZ{10.0f, 10.0f};
        circle.radius = 5.0f;

        REQUIRE(circle.intersects(circle));
        REQUIRE(circle.intersects(BoundingCircleXZ{VectorXZ{10.0f, 10.0f}, 3.0f}));
        REQUIRE_FALSE(circle.intersects(BoundingCircleXZ{VectorXZ{0.0f, 0.0f}, 2.0f}));
    }
    SUBCASE("intersects BoundingBoxXZ")
    {
        BoundingCircleXZ circle;
        circle.center = VectorXZ{10.0f, 10.0f};
        circle.radius = 5.0f;

        REQUIRE(circle.intersects(BoundingBoxXZ{VectorXZ{9.0f, 9.0f}, VectorXZ{11.0f, 11.0f}}));
        REQUIRE_FALSE(circle.intersects(BoundingBoxXZ{VectorXZ{100.0f, 100.0f}, VectorXZ{105.0f, 105.0f}}));
    }
}
