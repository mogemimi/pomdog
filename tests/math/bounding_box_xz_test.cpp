// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/bounding_box_xz.h"
#include "pomdog/math/bounding_circle_xz.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/math/vector_xz.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::BoundingBoxXZ;
using pomdog::BoundingCircleXZ;
using pomdog::ContainmentType;
using pomdog::VectorXZ;

TEST_CASE("BoundingBoxXZ")
{
    SUBCASE("constructor")
    {
        BoundingBoxXZ box{VectorXZ{1.0f, 2.0f}, VectorXZ{3.0f, 4.0f}};
        REQUIRE(box.min.x == 1.0f);
        REQUIRE(box.min.z == 2.0f);
        REQUIRE(box.max.x == 3.0f);
        REQUIRE(box.max.z == 4.0f);
    }
    SUBCASE("getCorners")
    {
        BoundingBoxXZ box;
        box.min = VectorXZ{12.0f, 13.0f};
        box.max = VectorXZ{15.0f, 16.0f};

        auto corners = box.getCorners();
        REQUIRE_FALSE(corners.empty());
        static_assert(BoundingBoxXZ::CornerCount == 4);

        REQUIRE(corners[0] == VectorXZ{12.0f, 16.0f});
        REQUIRE(corners[1] == VectorXZ{15.0f, 16.0f});
        REQUIRE(corners[2] == VectorXZ{15.0f, 13.0f});
        REQUIRE(corners[3] == VectorXZ{12.0f, 13.0f});
    }
    SUBCASE("contains VectorXZ")
    {
        BoundingBoxXZ box;
        box.min = VectorXZ{12.0f, 13.0f};
        box.max = VectorXZ{15.0f, 16.0f};

        REQUIRE(box.contains(VectorXZ{13.5f, 14.5f}));
        REQUIRE(box.contains(VectorXZ{12.0f, 13.0f}));
        REQUIRE(box.contains(VectorXZ{15.0f, 16.0f}));

        REQUIRE_FALSE(box.contains(VectorXZ{11.9f, 13.0f}));
        REQUIRE_FALSE(box.contains(VectorXZ{15.1f, 16.0f}));
    }
    SUBCASE("contains BoundingBoxXZ")
    {
        BoundingBoxXZ box;
        box.min = VectorXZ{10.0f, 11.0f};
        box.max = VectorXZ{20.0f, 21.0f};

        REQUIRE(box.contains(box) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingBoxXZ{VectorXZ{11.0f, 12.0f}, VectorXZ{19.0f, 20.0f}}) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingBoxXZ{VectorXZ{100.0f, 101.0f}, VectorXZ{110.0f, 111.0f}}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingCircleXZ")
    {
        BoundingBoxXZ box;
        box.min = VectorXZ{10.0f, 10.0f};
        box.max = VectorXZ{20.0f, 20.0f};

        REQUIRE(box.contains(BoundingCircleXZ{VectorXZ{15.0f, 15.0f}, 1.0f}) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingCircleXZ{VectorXZ{15.0f, 15.0f}, 5.0f}) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingCircleXZ{VectorXZ{15.0f, 15.0f}, 6.0f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingCircleXZ{VectorXZ{100.0f, 100.0f}, 1.0f}) == ContainmentType::Disjoint);
    }
    SUBCASE("intersects BoundingBoxXZ")
    {
        BoundingBoxXZ box;
        box.min = VectorXZ{10.0f, 11.0f};
        box.max = VectorXZ{20.0f, 21.0f};

        REQUIRE(box.intersects(box));
        REQUIRE(box.intersects(BoundingBoxXZ{VectorXZ{11.0f, 12.0f}, VectorXZ{19.0f, 20.0f}}));
        REQUIRE_FALSE(box.intersects(BoundingBoxXZ{VectorXZ{100.0f, 101.0f}, VectorXZ{110.0f, 111.0f}}));
    }
    SUBCASE("intersects BoundingCircleXZ")
    {
        BoundingBoxXZ box;
        box.min = VectorXZ{10.0f, 10.0f};
        box.max = VectorXZ{20.0f, 20.0f};

        REQUIRE(box.intersects(BoundingCircleXZ{VectorXZ{15.0f, 15.0f}, 1.0f}));
        REQUIRE_FALSE(box.intersects(BoundingCircleXZ{VectorXZ{100.0f, 100.0f}, 1.0f}));
    }
}
