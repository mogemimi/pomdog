// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_box2d.h"
#include "pomdog/math/bounding_circle.h"
#include "pomdog/math/containment_type.h"
#include <catch_amalgamated.hpp>

using namespace pomdog;

TEST_CASE("BoundingBox2D", "[BoundingBox2D]")
{
    BoundingBox2D box;

    SECTION("CornerCount")
    {
        STATIC_REQUIRE(BoundingBox2D::CornerCount == 4);
    }
    SECTION("Constructors")
    {
        BoundingBox2D box2;
        box2.min = Vector2::createZero();
        box2.max = Vector2{42.0f, 3.14f};

        box = box2;
        REQUIRE(Vector2::createZero() == box.min);
        REQUIRE(Vector2{42.0f, 3.14f} == box.max);

        BoundingBox2D box3{Vector2{-1.0f, 2.718f}, Vector2{17.0f, 8.0f}};
        box = box3;
        REQUIRE(Vector2{-1.0f, 2.718f} == box.min);
        REQUIRE(Vector2{-1.0f, 2.718f} == box3.min);
        REQUIRE(Vector2{17.0f, 8.0f} == box.max);
        REQUIRE(Vector2{17.0f, 8.0f} == box3.max);
    }
    SECTION("GetCorners")
    {
        box.min = Vector2{12, 13};
        box.max = Vector2{15, 16};

        auto corners = box.getCorners();
        REQUIRE_FALSE(corners.empty());
        REQUIRE(BoundingBox2D::CornerCount == corners.size());
        REQUIRE(Vector2{12, 13} == box.min);
        REQUIRE(Vector2{15, 16} == box.max);
        REQUIRE(corners[0] == Vector2(box.min.x, box.max.y));
        REQUIRE(corners[1] == Vector2(box.max.x, box.max.y));
        REQUIRE(corners[2] == Vector2(box.max.x, box.min.y));
        REQUIRE(corners[3] == Vector2(box.min.x, box.min.y));
    }
    SECTION("Contains_Vector3")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;

        box.min = Vector2{a, b};
        box.max = Vector2{d, e};

        REQUIRE(ContainmentType::Contains == box.contains(Vector2(a + 1, b + 1)));
        REQUIRE(ContainmentType::Contains == box.contains(Vector2(d - 1, b + 1)));
        REQUIRE(ContainmentType::Contains == box.contains(Vector2(a + 1, e - 1)));
        REQUIRE(ContainmentType::Contains == box.contains(Vector2(d - 1, e - 1)));

        REQUIRE(ContainmentType::Intersects == box.contains(Vector2(a, b)));
        REQUIRE(ContainmentType::Intersects == box.contains(Vector2(d, b)));
        REQUIRE(ContainmentType::Intersects == box.contains(Vector2(a, e)));
        REQUIRE(ContainmentType::Intersects == box.contains(Vector2(d, e)));

        REQUIRE(ContainmentType::Intersects == box.contains(Vector2((d + a) / 2, b)));
        REQUIRE(ContainmentType::Intersects == box.contains(Vector2((d + a) / 2, e)));
        REQUIRE(ContainmentType::Intersects == box.contains(Vector2(a, (e + b) / 2)));
        REQUIRE(ContainmentType::Intersects == box.contains(Vector2(d, (e + b) / 2)));

        REQUIRE(ContainmentType::Disjoint == box.contains(Vector2(a - 1, b)));
        REQUIRE(ContainmentType::Disjoint == box.contains(Vector2(d + 1, b)));
        REQUIRE(ContainmentType::Disjoint == box.contains(Vector2(a, b - 1)));
        REQUIRE(ContainmentType::Disjoint == box.contains(Vector2(d, e + 1)));

        REQUIRE(ContainmentType::Disjoint == box.contains(Vector2(a - 1, b - 1)));
        REQUIRE(ContainmentType::Disjoint == box.contains(Vector2(d + 1, e + 1)));
        REQUIRE(ContainmentType::Disjoint == box.contains(Vector2(a - 1, e + 1)));
        REQUIRE(ContainmentType::Disjoint == box.contains(Vector2(d + 1, b - 1)));

        REQUIRE(ContainmentType::Disjoint == box.contains(Vector2(a - 1, b - 1)));
        REQUIRE(ContainmentType::Disjoint == box.contains(Vector2(d + 1, e + 1)));
        REQUIRE(ContainmentType::Disjoint == box.contains(Vector2(a - 1, e + 1)));
        REQUIRE(ContainmentType::Disjoint == box.contains(Vector2(d + 1, b - 1)));
    }
    SECTION("Contains_BoundingBox2D")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;

        box.min = Vector2{a, b};
        box.max = Vector2{d, e};

        REQUIRE(ContainmentType::Contains == box.contains(box));

        REQUIRE(ContainmentType::Contains == box.contains(BoundingBox2D{box.min, pomdog::math::lerp(box.min, box.max, 0.01f)}));
        REQUIRE(ContainmentType::Contains == box.contains(BoundingBox2D{box.min, pomdog::math::lerp(box.min, box.max, 0.5f)}));
        REQUIRE(ContainmentType::Contains == box.contains(BoundingBox2D{box.min, pomdog::math::lerp(box.min, box.max, 0.99f)}));
        REQUIRE(ContainmentType::Intersects == box.contains(BoundingBox2D{box.min, pomdog::math::lerp(box.min, box.max, 1.1f)}));
        REQUIRE(ContainmentType::Intersects == box.contains(BoundingBox2D{Vector2::createZero(), Vector2(a, b)}));
        REQUIRE(ContainmentType::Disjoint == box.contains(BoundingBox2D{Vector2::createZero(), Vector2(a - 1, b)}));
    }
    SECTION("Contains_BoundingCircle")
    {
        box.min = Vector2{12.0f, 16.0f};
        box.max = Vector2{32.0f, 64.0f};
        auto center = pomdog::math::lerp(box.min, box.max, 0.5f);

        REQUIRE(ContainmentType::Contains == box.contains(BoundingCircle{center, 1.0f}));
        REQUIRE(ContainmentType::Contains == box.contains(BoundingCircle{center, (box.max - box.min).x / 2}));
        REQUIRE(ContainmentType::Intersects == box.contains(BoundingCircle{center, (box.max - box.min).y / 2}));

        REQUIRE(ContainmentType::Intersects == box.contains(BoundingCircle{Vector2{0.0f, 16.0f}, 12.01f}));
        REQUIRE(ContainmentType::Intersects == box.contains(BoundingCircle{Vector2{12.0f, 0.0f}, 16.01f}));
        REQUIRE(ContainmentType::Disjoint == box.contains(BoundingCircle{Vector2{0.0f, 16.0f}, 11.99f}));
        REQUIRE(ContainmentType::Disjoint == box.contains(BoundingCircle{Vector2{12.0f, 0.0f}, 15.99f}));
    }
    SECTION("Intersects_BoundingBox2D")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;

        box.min = Vector2{a, b};
        box.max = Vector2{d, e};

        REQUIRE(box.intersects(box));
        REQUIRE(box.intersects(BoundingBox2D{box.min, pomdog::math::lerp(box.min, box.max, 0.01f)}));
        REQUIRE(box.intersects(BoundingBox2D{box.min, pomdog::math::lerp(box.min, box.max, 0.5f)}));
        REQUIRE(box.intersects(BoundingBox2D{box.min, pomdog::math::lerp(box.min, box.max, 0.99f)}));
        REQUIRE(box.intersects(BoundingBox2D{box.min, pomdog::math::lerp(box.min, box.max, 1.1f)}));
        REQUIRE(box.intersects(BoundingBox2D{Vector2::createZero(), Vector2(a, b)}));
        REQUIRE_FALSE(box.intersects(BoundingBox2D{Vector2::createZero(), Vector2(a - 1, b)}));
    }
    SECTION("Intersects_BoundingCircle")
    {
        box.min = Vector2{12.0f, 16.0f};
        box.max = Vector2{32.0f, 64.0f};
        const auto center = pomdog::math::lerp(box.min, box.max, 0.5f);

        REQUIRE(box.intersects(BoundingCircle{center, 1.0f}));
        REQUIRE(box.intersects(BoundingCircle{center, (box.max - box.min).x / 2}));
        REQUIRE(box.intersects(BoundingCircle{center, (box.max - box.min).y / 2}));

        REQUIRE(box.intersects(BoundingCircle{Vector2{0.0f, 16.0f}, 12.01f}));
        REQUIRE(box.intersects(BoundingCircle{Vector2{12.0f, 0.0f}, 16.01f}));
        REQUIRE_FALSE(box.intersects(BoundingCircle{Vector2{0.0f, 16.0f}, 11.99f}));
        REQUIRE_FALSE(box.intersects(BoundingCircle{Vector2{12.0f, 0.0f}, 15.99f}));
    }
}
