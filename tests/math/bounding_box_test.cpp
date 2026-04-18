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

TEST_CASE("BoundingBox")
{
    SUBCASE("default constructor")
    {
        // NOTE: Default constructor does not zero-initialize members
        BoundingBox box = {};
        box.min = Vector3{1.0f, 2.0f, 3.0f};
        box.max = Vector3{4.0f, 5.0f, 6.0f};
        REQUIRE(box.min.x == 1.0f);
        REQUIRE(box.min.y == 2.0f);
        REQUIRE(box.min.z == 3.0f);
        REQUIRE(box.max.x == 4.0f);
        REQUIRE(box.max.y == 5.0f);
        REQUIRE(box.max.z == 6.0f);
    }
    SUBCASE("constructor with Vector3")
    {
        BoundingBox box{Vector3{1.0f, 2.0f, 3.0f}, Vector3{4.0f, 5.0f, 6.0f}};
        REQUIRE(box.min.x == 1.0f);
        REQUIRE(box.min.y == 2.0f);
        REQUIRE(box.min.z == 3.0f);
        REQUIRE(box.max.x == 4.0f);
        REQUIRE(box.max.y == 5.0f);
        REQUIRE(box.max.z == 6.0f);
    }
    SUBCASE("getCorners")
    {
        BoundingBox box;
        box.min = Vector3{12.0f, 13.0f, 14.0f};
        box.max = Vector3{15.0f, 16.0f, 17.0f};

        auto corners = box.getCorners();
        REQUIRE_FALSE(corners.empty());
        static_assert(BoundingBox::CornerCount == 8, "CornerCount should be 8");

        REQUIRE(corners[0] == Vector3{12.0f, 16.0f, 17.0f});
        REQUIRE(corners[1] == Vector3{15.0f, 16.0f, 17.0f});
        REQUIRE(corners[2] == Vector3{15.0f, 13.0f, 17.0f});
        REQUIRE(corners[3] == Vector3{12.0f, 13.0f, 17.0f});
        REQUIRE(corners[4] == Vector3{12.0f, 16.0f, 14.0f});
        REQUIRE(corners[5] == Vector3{15.0f, 16.0f, 14.0f});
        REQUIRE(corners[6] == Vector3{15.0f, 13.0f, 14.0f});
        REQUIRE(corners[7] == Vector3{12.0f, 13.0f, 14.0f});

        REQUIRE(corners[0] == Vector3(box.min.x, box.max.y, box.max.z));
        REQUIRE(corners[1] == Vector3(box.max.x, box.max.y, box.max.z));
        REQUIRE(corners[2] == Vector3(box.max.x, box.min.y, box.max.z));
        REQUIRE(corners[3] == Vector3(box.min.x, box.min.y, box.max.z));
        REQUIRE(corners[4] == Vector3(box.min.x, box.max.y, box.min.z));
        REQUIRE(corners[5] == Vector3(box.max.x, box.max.y, box.min.z));
        REQUIRE(corners[6] == Vector3(box.max.x, box.min.y, box.min.z));
        REQUIRE(corners[7] == Vector3(box.min.x, box.min.y, box.min.z));
    }
    SUBCASE("contains Vector3")
    {
        BoundingBox box;
        box.min = Vector3{12.0f, 13.0f, 14.0f};
        box.max = Vector3{15.0f, 16.0f, 17.0f};

        REQUIRE(box.contains(Vector3{12.0f, 13.0f, 14.0f}));
        REQUIRE(box.contains(Vector3{15.0f, 16.0f, 17.0f}));
        REQUIRE(box.contains(Vector3{12.0f, 13.0f, 17.0f}));
        REQUIRE(box.contains(Vector3{15.0f, 16.0f, 14.0f}));

        REQUIRE(box.contains(Vector3{13.5f, 14.5f, 15.5f}));

        REQUIRE_FALSE(box.contains(Vector3{11.9f, 13.0f, 14.0f}));
        REQUIRE_FALSE(box.contains(Vector3{12.0f, 12.9f, 14.0f}));
        REQUIRE_FALSE(box.contains(Vector3{12.0f, 13.0f, 13.9f}));
        REQUIRE_FALSE(box.contains(Vector3{15.1f, 16.0f, 17.0f}));
        REQUIRE_FALSE(box.contains(Vector3{15.0f, 16.1f, 17.0f}));
        REQUIRE_FALSE(box.contains(Vector3{15.0f, 16.0f, 17.1f}));
    }
    SUBCASE("contains Vector3 with boundary classification")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto c = 24.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;
        constexpr auto f = 96.0f;

        BoundingBox box;
        box.min = Vector3{a, b, c};
        box.max = Vector3{d, e, f};

        REQUIRE(box.contains(Vector3(a + 1, b + 1, c + 1)));
        REQUIRE(box.contains(Vector3(d - 1, b + 1, c + 1)));
        REQUIRE(box.contains(Vector3(a + 1, e - 1, c + 1)));
        REQUIRE(box.contains(Vector3(d - 1, e - 1, c + 1)));
        REQUIRE(box.contains(Vector3(a + 1, b + 1, f - 1)));
        REQUIRE(box.contains(Vector3(d - 1, b + 1, f - 1)));
        REQUIRE(box.contains(Vector3(a + 1, e - 1, f - 1)));
        REQUIRE(box.contains(Vector3(d - 1, e - 1, f - 1)));

        REQUIRE(box.contains(Vector3(a, b, c)));
        REQUIRE(box.contains(Vector3(d, b, c)));
        REQUIRE(box.contains(Vector3(a, e, c)));
        REQUIRE(box.contains(Vector3(d, e, c)));
        REQUIRE(box.contains(Vector3(a, b, f)));
        REQUIRE(box.contains(Vector3(d, b, f)));
        REQUIRE(box.contains(Vector3(a, e, f)));
        REQUIRE(box.contains(Vector3(d, e, f)));

        REQUIRE(box.contains(Vector3((d + a) / 2, b, c)));
        REQUIRE(box.contains(Vector3((d + a) / 2, b, f)));
        REQUIRE(box.contains(Vector3((d + a) / 2, e, c)));
        REQUIRE(box.contains(Vector3((d + a) / 2, e, f)));
        REQUIRE(box.contains(Vector3(a, (e + b) / 2, c)));
        REQUIRE(box.contains(Vector3(a, (e + b) / 2, f)));
        REQUIRE(box.contains(Vector3(d, (e + b) / 2, c)));
        REQUIRE(box.contains(Vector3(d, (e + b) / 2, f)));
        REQUIRE(box.contains(Vector3(a, b, (f + c) / 2)));
        REQUIRE(box.contains(Vector3(a, e, (f + c) / 2)));
        REQUIRE(box.contains(Vector3(d, b, (f + c) / 2)));
        REQUIRE(box.contains(Vector3(d, e, (f + c) / 2)));

        REQUIRE_FALSE(box.contains(Vector3(a - 1, b, c)));
        REQUIRE_FALSE(box.contains(Vector3(d + 1, b, c)));
        REQUIRE_FALSE(box.contains(Vector3(a, b - 1, c)));
        REQUIRE_FALSE(box.contains(Vector3(d, e + 1, c)));
        REQUIRE_FALSE(box.contains(Vector3(a, b, c - 1)));
        REQUIRE_FALSE(box.contains(Vector3(d, b, f + 1)));

        REQUIRE_FALSE(box.contains(Vector3(a - 1, b - 1, c)));
        REQUIRE_FALSE(box.contains(Vector3(d + 1, e + 1, c)));
        REQUIRE_FALSE(box.contains(Vector3(a - 1, e + 1, c)));
        REQUIRE_FALSE(box.contains(Vector3(d + 1, b - 1, c)));

        REQUIRE_FALSE(box.contains(Vector3(a - 1, b - 1, c - 1)));
        REQUIRE_FALSE(box.contains(Vector3(d + 1, e + 1, c - 1)));
        REQUIRE_FALSE(box.contains(Vector3(a - 1, e + 1, c - 1)));
        REQUIRE_FALSE(box.contains(Vector3(d + 1, b - 1, c - 1)));
        REQUIRE_FALSE(box.contains(Vector3(a - 1, b - 1, f + 1)));
        REQUIRE_FALSE(box.contains(Vector3(d + 1, e + 1, f + 1)));
        REQUIRE_FALSE(box.contains(Vector3(a - 1, e + 1, f + 1)));
        REQUIRE_FALSE(box.contains(Vector3(d + 1, b - 1, f + 1)));
    }
    SUBCASE("contains BoundingBox")
    {
        BoundingBox box;
        box.min = Vector3{10.0f, 11.0f, 12.0f};
        box.max = Vector3{20.0f, 21.0f, 22.0f};

        REQUIRE(box.contains(box) == ContainmentType::Contains);

        REQUIRE(box.contains(BoundingBox{Vector3{10.0f, 11.0f, 12.0f}, Vector3{20.0f, 21.0f, 22.0f}}) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingBox{Vector3{11.0f, 12.0f, 13.0f}, Vector3{19.0f, 20.0f, 21.0f}}) == ContainmentType::Contains);

        REQUIRE(box.contains(BoundingBox{Vector3{11.0f, 12.0f, 13.0f}, Vector3{21.0f, 22.0f, 23.0f}}) == ContainmentType::Intersects);

        REQUIRE(box.contains(BoundingBox{Vector3{100.0f, 101.0f, 102.0f}, Vector3{110.0f, 111.0f, 112.0f}}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingSphere")
    {
        BoundingBox box;
        box.min = Vector3{10.0f, 10.0f, 10.0f};
        box.max = Vector3{20.0f, 20.0f, 20.0f};

        REQUIRE(box.contains(BoundingSphere{Vector3{15.0f, 15.0f, 15.0f}, 1.0f}) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingSphere{Vector3{15.0f, 15.0f, 15.0f}, 5.0f}) == ContainmentType::Contains);

        REQUIRE(box.contains(BoundingSphere{Vector3{15.0f, 15.0f, 15.0f}, 6.0f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingSphere{Vector3{10.0f, 15.0f, 15.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingSphere{Vector3{20.0f, 15.0f, 15.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingSphere{Vector3{15.0f, 10.0f, 15.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingSphere{Vector3{15.0f, 20.0f, 15.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingSphere{Vector3{15.0f, 15.0f, 10.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingSphere{Vector3{15.0f, 15.0f, 20.0f}, 1.0f}) == ContainmentType::Intersects);

        REQUIRE(box.contains(BoundingSphere{Vector3{8.0f, 8.0f, 8.0f}, 1.0f}) == ContainmentType::Disjoint);
        REQUIRE(box.contains(BoundingSphere{Vector3{100.0f, 100.0f, 100.0f}, 5.0f}) == ContainmentType::Disjoint);
    }
    SUBCASE("intersects BoundingBox")
    {
        BoundingBox box;
        box.min = Vector3{10.0f, 11.0f, 12.0f};
        box.max = Vector3{20.0f, 21.0f, 22.0f};

        REQUIRE(box.intersects(box));

        REQUIRE(box.intersects(BoundingBox{Vector3{10.0f, 11.0f, 12.0f}, Vector3{20.0f, 21.0f, 22.0f}}));
        REQUIRE(box.intersects(BoundingBox{Vector3{11.0f, 12.0f, 13.0f}, Vector3{19.0f, 20.0f, 21.0f}}));
        REQUIRE(box.intersects(BoundingBox{Vector3{11.0f, 12.0f, 13.0f}, Vector3{21.0f, 22.0f, 23.0f}}));

        REQUIRE_FALSE(box.intersects(BoundingBox{Vector3{100.0f, 101.0f, 102.0f}, Vector3{110.0f, 111.0f, 112.0f}}));
    }
    SUBCASE("intersects BoundingSphere")
    {
        BoundingBox box;
        box.min = Vector3{10.0f, 10.0f, 10.0f};
        box.max = Vector3{20.0f, 20.0f, 20.0f};

        REQUIRE(box.intersects(BoundingSphere{Vector3{15.0f, 15.0f, 15.0f}, 1.0f}));
        REQUIRE(box.intersects(BoundingSphere{Vector3{15.0f, 15.0f, 15.0f}, 5.0f}));
        REQUIRE(box.intersects(BoundingSphere{Vector3{15.0f, 15.0f, 15.0f}, 6.0f}));
        REQUIRE(box.intersects(BoundingSphere{Vector3{10.0f, 15.0f, 15.0f}, 1.0f}));
        REQUIRE(box.intersects(BoundingSphere{Vector3{20.0f, 15.0f, 15.0f}, 1.0f}));
        REQUIRE(box.intersects(BoundingSphere{Vector3{15.0f, 10.0f, 15.0f}, 1.0f}));
        REQUIRE(box.intersects(BoundingSphere{Vector3{15.0f, 20.0f, 15.0f}, 1.0f}));
        REQUIRE(box.intersects(BoundingSphere{Vector3{15.0f, 15.0f, 10.0f}, 1.0f}));
        REQUIRE(box.intersects(BoundingSphere{Vector3{15.0f, 15.0f, 20.0f}, 1.0f}));

        REQUIRE_FALSE(box.intersects(BoundingSphere{Vector3{8.0f, 8.0f, 8.0f}, 1.0f}));
        REQUIRE_FALSE(box.intersects(BoundingSphere{Vector3{100.0f, 100.0f, 100.0f}, 5.0f}));
    }
}
