// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/bounding_box2d.h"
#include "pomdog/math/bounding_circle.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::BoundingBox2D;
using pomdog::BoundingCircle;
using pomdog::ContainmentType;
using pomdog::Vector2;
using pomdog::math::lerp;

TEST_CASE("BoundingBox2D")
{
    SUBCASE("default constructor")
    {
        // NOTE: Default constructor does not zero-initialize members
        BoundingBox2D box = {};
        box.min = Vector2{1.0f, 2.0f};
        box.max = Vector2{3.0f, 4.0f};
        REQUIRE(box.min.x == 1.0f);
        REQUIRE(box.min.y == 2.0f);
        REQUIRE(box.max.x == 3.0f);
        REQUIRE(box.max.y == 4.0f);
    }
    SUBCASE("constructor with Vector2")
    {
        BoundingBox2D box{Vector2{1.0f, 2.0f}, Vector2{3.0f, 4.0f}};
        REQUIRE(box.min.x == 1.0f);
        REQUIRE(box.min.y == 2.0f);
        REQUIRE(box.max.x == 3.0f);
        REQUIRE(box.max.y == 4.0f);
    }
    SUBCASE("getCorners")
    {
        BoundingBox2D box;
        box.min = Vector2{12.0f, 13.0f};
        box.max = Vector2{15.0f, 16.0f};
        REQUIRE(box.min == Vector2{12.0f, 13.0f});
        REQUIRE(box.max == Vector2{15.0f, 16.0f});

        auto corners = box.getCorners();
        REQUIRE_FALSE(corners.empty());
        static_assert(BoundingBox2D::CornerCount == 4, "CornerCount should be 4");

        // NOTE: Corner order: top-left, top-right, bottom-right, bottom-left
        REQUIRE(corners[0] == Vector2{12.0f, 16.0f});
        REQUIRE(corners[1] == Vector2{15.0f, 16.0f});
        REQUIRE(corners[2] == Vector2{15.0f, 13.0f});
        REQUIRE(corners[3] == Vector2{12.0f, 13.0f});

        REQUIRE(corners[0] == Vector2(box.min.x, box.max.y));
        REQUIRE(corners[1] == Vector2(box.max.x, box.max.y));
        REQUIRE(corners[2] == Vector2(box.max.x, box.min.y));
        REQUIRE(corners[3] == Vector2(box.min.x, box.min.y));
    }
    SUBCASE("contains Vector2")
    {
        BoundingBox2D box;
        box.min = Vector2{12.0f, 13.0f};
        box.max = Vector2{15.0f, 16.0f};

        REQUIRE(box.contains(Vector2{12.0f, 13.0f}));
        REQUIRE(box.contains(Vector2{15.0f, 16.0f}));
        REQUIRE(box.contains(Vector2{12.0f, 16.0f}));
        REQUIRE(box.contains(Vector2{15.0f, 13.0f}));

        REQUIRE(box.contains(Vector2{13.5f, 14.5f}));

        REQUIRE_FALSE(box.contains(Vector2{11.9f, 13.0f}));
        REQUIRE_FALSE(box.contains(Vector2{12.0f, 12.9f}));
        REQUIRE_FALSE(box.contains(Vector2{15.1f, 16.0f}));
        REQUIRE_FALSE(box.contains(Vector2{15.0f, 16.1f}));
    }
    SUBCASE("contains Vector2 with boundary classification")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;

        BoundingBox2D box;
        box.min = Vector2{a, b};
        box.max = Vector2{d, e};

        REQUIRE(box.contains(Vector2(a + 1, b + 1)));
        REQUIRE(box.contains(Vector2(d - 1, b + 1)));
        REQUIRE(box.contains(Vector2(a + 1, e - 1)));
        REQUIRE(box.contains(Vector2(d - 1, e - 1)));

        REQUIRE(box.contains(Vector2(a, b)));
        REQUIRE(box.contains(Vector2(d, b)));
        REQUIRE(box.contains(Vector2(a, e)));
        REQUIRE(box.contains(Vector2(d, e)));

        REQUIRE(box.contains(Vector2((d + a) / 2, b)));
        REQUIRE(box.contains(Vector2((d + a) / 2, e)));
        REQUIRE(box.contains(Vector2(a, (e + b) / 2)));
        REQUIRE(box.contains(Vector2(d, (e + b) / 2)));

        REQUIRE_FALSE(box.contains(Vector2(a - 1, b)));
        REQUIRE_FALSE(box.contains(Vector2(d + 1, b)));
        REQUIRE_FALSE(box.contains(Vector2(a, b - 1)));
        REQUIRE_FALSE(box.contains(Vector2(d, e + 1)));

        REQUIRE_FALSE(box.contains(Vector2(a - 1, b - 1)));
        REQUIRE_FALSE(box.contains(Vector2(d + 1, e + 1)));
        REQUIRE_FALSE(box.contains(Vector2(a - 1, e + 1)));
        REQUIRE_FALSE(box.contains(Vector2(d + 1, b - 1)));

        REQUIRE_FALSE(box.contains(Vector2(a - 1, b - 1)));
        REQUIRE_FALSE(box.contains(Vector2(d + 1, e + 1)));
        REQUIRE_FALSE(box.contains(Vector2(a - 1, e + 1)));
        REQUIRE_FALSE(box.contains(Vector2(d + 1, b - 1)));
    }
    SUBCASE("contains BoundingBox2D")
    {
        BoundingBox2D box;
        box.min = Vector2{10.0f, 11.0f};
        box.max = Vector2{20.0f, 21.0f};

        REQUIRE(box.contains(box) == ContainmentType::Contains);

        REQUIRE(box.contains(BoundingBox2D{Vector2{10.0f, 11.0f}, Vector2{20.0f, 21.0f}}) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingBox2D{Vector2{11.0f, 12.0f}, Vector2{19.0f, 20.0f}}) == ContainmentType::Contains);

        REQUIRE(box.contains(BoundingBox2D{Vector2{100.0f, 101.0f}, Vector2{110.0f, 111.0f}}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingBox2D with interpolated ranges")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;

        BoundingBox2D box;
        box.min = Vector2{a, b};
        box.max = Vector2{d, e};

        REQUIRE(box.contains(box) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingBox2D{box.min, lerp(box.min, box.max, 0.01f)}) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingBox2D{box.min, lerp(box.min, box.max, 0.5f)}) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingBox2D{box.min, lerp(box.min, box.max, 0.99f)}) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingBox2D{box.min, lerp(box.min, box.max, 1.1f)}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingBox2D{Vector2::createZero(), Vector2(a, b)}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingBox2D{Vector2::createZero(), Vector2(a - 1, b)}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingCircle")
    {
        BoundingBox2D box;
        box.min = Vector2{10.0f, 10.0f};
        box.max = Vector2{20.0f, 20.0f};

        REQUIRE(box.contains(BoundingCircle{Vector2{15.0f, 15.0f}, 1.0f}) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingCircle{Vector2{15.0f, 15.0f}, 5.0f}) == ContainmentType::Contains);

        REQUIRE(box.contains(BoundingCircle{Vector2{15.0f, 15.0f}, 6.0f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingCircle{Vector2{10.0f, 15.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingCircle{Vector2{20.0f, 15.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingCircle{Vector2{15.0f, 10.0f}, 1.0f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingCircle{Vector2{15.0f, 20.0f}, 1.0f}) == ContainmentType::Intersects);

        REQUIRE(box.contains(BoundingCircle{Vector2{100.0f, 100.0f}, 1.0f}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingCircle with asymmetric box")
    {
        BoundingBox2D box;
        box.min = Vector2{12.0f, 16.0f};
        box.max = Vector2{32.0f, 64.0f};
        auto center = lerp(box.min, box.max, 0.5f);

        REQUIRE(box.contains(BoundingCircle{center, 1.0f}) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingCircle{center, (box.max - box.min).x / 2}) == ContainmentType::Contains);
        REQUIRE(box.contains(BoundingCircle{center, (box.max - box.min).y / 2}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingCircle{Vector2{0.0f, 16.0f}, 12.01f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingCircle{Vector2{12.0f, 0.0f}, 16.01f}) == ContainmentType::Intersects);
        REQUIRE(box.contains(BoundingCircle{Vector2{0.0f, 16.0f}, 11.99f}) == ContainmentType::Disjoint);
        REQUIRE(box.contains(BoundingCircle{Vector2{12.0f, 0.0f}, 15.99f}) == ContainmentType::Disjoint);
    }
    SUBCASE("intersects BoundingBox2D")
    {
        BoundingBox2D box;
        box.min = Vector2{10.0f, 11.0f};
        box.max = Vector2{20.0f, 21.0f};

        REQUIRE(box.intersects(box));

        REQUIRE(box.intersects(BoundingBox2D{Vector2{10.0f, 11.0f}, Vector2{20.0f, 21.0f}}));
        REQUIRE(box.intersects(BoundingBox2D{Vector2{11.0f, 12.0f}, Vector2{19.0f, 20.0f}}));

        REQUIRE_FALSE(box.intersects(BoundingBox2D{Vector2{100.0f, 101.0f}, Vector2{110.0f, 111.0f}}));
    }
    SUBCASE("intersects BoundingBox2D with interpolated ranges")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;

        BoundingBox2D box;
        box.min = Vector2{a, b};
        box.max = Vector2{d, e};

        REQUIRE(box.intersects(box));
        REQUIRE(box.intersects(BoundingBox2D{box.min, lerp(box.min, box.max, 0.01f)}));
        REQUIRE(box.intersects(BoundingBox2D{box.min, lerp(box.min, box.max, 0.5f)}));
        REQUIRE(box.intersects(BoundingBox2D{box.min, lerp(box.min, box.max, 0.99f)}));
        REQUIRE(box.intersects(BoundingBox2D{box.min, lerp(box.min, box.max, 1.1f)}));
        REQUIRE(box.intersects(BoundingBox2D{Vector2::createZero(), Vector2(a, b)}));
        REQUIRE_FALSE(box.intersects(BoundingBox2D{Vector2::createZero(), Vector2(a - 1, b)}));
    }
    SUBCASE("intersects BoundingCircle")
    {
        BoundingBox2D box;
        box.min = Vector2{10.0f, 10.0f};
        box.max = Vector2{20.0f, 20.0f};

        REQUIRE(box.intersects(BoundingCircle{Vector2{15.0f, 15.0f}, 1.0f}));
        REQUIRE(box.intersects(BoundingCircle{Vector2{15.0f, 15.0f}, 5.0f}));
        REQUIRE(box.intersects(BoundingCircle{Vector2{15.0f, 15.0f}, 6.0f}));
        REQUIRE(box.intersects(BoundingCircle{Vector2{10.0f, 15.0f}, 1.0f}));
        REQUIRE(box.intersects(BoundingCircle{Vector2{20.0f, 15.0f}, 1.0f}));
        REQUIRE(box.intersects(BoundingCircle{Vector2{15.0f, 10.0f}, 1.0f}));
        REQUIRE(box.intersects(BoundingCircle{Vector2{15.0f, 20.0f}, 1.0f}));

        REQUIRE_FALSE(box.intersects(BoundingCircle{Vector2{100.0f, 100.0f}, 1.0f}));
    }
    SUBCASE("intersects BoundingCircle with asymmetric box")
    {
        BoundingBox2D box;
        box.min = Vector2{12.0f, 16.0f};
        box.max = Vector2{32.0f, 64.0f};
        const auto center = lerp(box.min, box.max, 0.5f);

        REQUIRE(box.intersects(BoundingCircle{center, 1.0f}));
        REQUIRE(box.intersects(BoundingCircle{center, (box.max - box.min).x / 2}));
        REQUIRE(box.intersects(BoundingCircle{center, (box.max - box.min).y / 2}));

        REQUIRE(box.intersects(BoundingCircle{Vector2{0.0f, 16.0f}, 12.01f}));
        REQUIRE(box.intersects(BoundingCircle{Vector2{12.0f, 0.0f}, 16.01f}));
        REQUIRE_FALSE(box.intersects(BoundingCircle{Vector2{0.0f, 16.0f}, 11.99f}));
        REQUIRE_FALSE(box.intersects(BoundingCircle{Vector2{12.0f, 0.0f}, 15.99f}));
    }
}
