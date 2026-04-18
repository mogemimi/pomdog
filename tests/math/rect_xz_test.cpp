// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/point2d.h"
#include "pomdog/math/rect_xz.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Point2D;
using pomdog::PointXZ;
using pomdog::RectXZ;

TEST_CASE("RectXZ")
{
    SUBCASE("constructor")
    {
        RectXZ rect;
        rect.min = PointXZ{10, 20};
        rect.max = PointXZ{30, 50};
        REQUIRE(rect.min.x == 10);
        REQUIRE(rect.min.z == 20);
        REQUIRE(rect.max.x == 30);
        REQUIRE(rect.max.z == 50);
    }
    SUBCASE("dx and dz")
    {
        RectXZ rect;
        rect.min = PointXZ{10, 20};
        rect.max = PointXZ{30, 50};
        REQUIRE(rect.dx() == 20);
        REQUIRE(rect.dz() == 30);
    }
    SUBCASE("minX maxX minZ maxZ")
    {
        RectXZ rect;
        rect.min = PointXZ{10, 20};
        rect.max = PointXZ{30, 50};
        REQUIRE(rect.minX() == 10);
        REQUIRE(rect.maxX() == 30);
        REQUIRE(rect.minZ() == 20);
        REQUIRE(rect.maxZ() == 50);
    }
    SUBCASE("contains PointXZ")
    {
        RectXZ rect;
        rect.min = PointXZ{10, 20};
        rect.max = PointXZ{30, 50};

        REQUIRE(rect.contains(PointXZ{10, 20}));
        REQUIRE(rect.contains(PointXZ{30, 50}));
        REQUIRE(rect.contains(PointXZ{20, 35}));

        REQUIRE_FALSE(rect.contains(PointXZ{9, 20}));
        REQUIRE_FALSE(rect.contains(PointXZ{31, 20}));
        REQUIRE_FALSE(rect.contains(PointXZ{10, 19}));
        REQUIRE_FALSE(rect.contains(PointXZ{10, 51}));
    }
    SUBCASE("contains Point2D")
    {
        RectXZ rect;
        rect.min = PointXZ{10, 20};
        rect.max = PointXZ{30, 50};

        // Point2D.x maps to x, Point2D.y maps to z
        REQUIRE(rect.contains(Point2D{10, 20}));
        REQUIRE(rect.contains(Point2D{30, 50}));
        REQUIRE_FALSE(rect.contains(Point2D{9, 20}));
    }
    SUBCASE("intersects")
    {
        RectXZ rect;
        rect.min = PointXZ{10, 20};
        rect.max = PointXZ{30, 50};

        REQUIRE(rect.intersects(rect));

        RectXZ other;
        other.min = PointXZ{20, 30};
        other.max = PointXZ{40, 60};
        REQUIRE(rect.intersects(other));

        RectXZ disjoint;
        disjoint.min = PointXZ{100, 100};
        disjoint.max = PointXZ{200, 200};
        REQUIRE_FALSE(rect.intersects(disjoint));
    }
    SUBCASE("overlap")
    {
        RectXZ a;
        a.min = PointXZ{10, 20};
        a.max = PointXZ{30, 50};

        RectXZ b;
        b.min = PointXZ{20, 30};
        b.max = PointXZ{40, 60};

        auto result = a.overlap(b);
        REQUIRE(result.has_value());
        REQUIRE(result->min == PointXZ{20, 30});
        REQUIRE(result->max == PointXZ{30, 50});

        RectXZ disjoint;
        disjoint.min = PointXZ{100, 100};
        disjoint.max = PointXZ{200, 200};
        REQUIRE_FALSE(a.overlap(disjoint).has_value());
    }
    SUBCASE("equality")
    {
        RectXZ a;
        a.min = PointXZ{10, 20};
        a.max = PointXZ{30, 50};

        RectXZ b;
        b.min = PointXZ{10, 20};
        b.max = PointXZ{30, 50};

        REQUIRE(a == b);

        b.max = PointXZ{31, 50};
        REQUIRE(a != b);
    }
}
