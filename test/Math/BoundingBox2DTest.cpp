// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/BoundingBox2D.hpp"
#include "Pomdog/Math/BoundingCircle.hpp"
#include "Pomdog/Math/ContainmentType.hpp"
#include "catch.hpp"

using namespace Pomdog;

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
        box2.Min = Vector2::Zero;
        box2.Max = Vector2{42.0f, 3.14f};

        box = box2;
        REQUIRE(Vector2::Zero == box.Min);
        REQUIRE(Vector2{42.0f, 3.14f} == box.Max);

        BoundingBox2D box3{Vector2{-1.0f, 2.718f}, Vector2{17.0f, 8.0f}};
        box = box3;
        REQUIRE(Vector2{-1.0f, 2.718f} == box.Min);
        REQUIRE(Vector2{-1.0f, 2.718f} == box3.Min);
        REQUIRE(Vector2{17.0f, 8.0f} == box.Max);
        REQUIRE(Vector2{17.0f, 8.0f} == box3.Max);
    }
    SECTION("GetCorners")
    {
        box.Min = Vector2{12, 13};
        box.Max = Vector2{15, 16};

        auto corners = box.GetCorners();
        REQUIRE_FALSE(corners.empty());
        REQUIRE(BoundingBox2D::CornerCount == corners.size());
        REQUIRE(Vector2{12, 13} == box.Min);
        REQUIRE(Vector2{15, 16} == box.Max);
        REQUIRE(corners[0] == Vector2(box.Min.X, box.Max.Y));
        REQUIRE(corners[1] == Vector2(box.Max.X, box.Max.Y));
        REQUIRE(corners[2] == Vector2(box.Max.X, box.Min.Y));
        REQUIRE(corners[3] == Vector2(box.Min.X, box.Min.Y));
    }
    SECTION("Contains_Vector3")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;

        box.Min = Vector2{a, b};
        box.Max = Vector2{d, e};

        REQUIRE(ContainmentType::Contains == box.Contains(Vector2(a + 1, b + 1)));
        REQUIRE(ContainmentType::Contains == box.Contains(Vector2(d - 1, b + 1)));
        REQUIRE(ContainmentType::Contains == box.Contains(Vector2(a + 1, e - 1)));
        REQUIRE(ContainmentType::Contains == box.Contains(Vector2(d - 1, e - 1)));

        REQUIRE(ContainmentType::Intersects == box.Contains(Vector2(a, b)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector2(d, b)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector2(a, e)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector2(d, e)));

        REQUIRE(ContainmentType::Intersects == box.Contains(Vector2((d + a) / 2, b)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector2((d + a) / 2, e)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector2(a, (e + b) / 2)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector2(d, (e + b) / 2)));

        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector2(a - 1, b)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector2(d + 1, b)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector2(a, b - 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector2(d, e + 1)));

        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector2(a - 1, b - 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector2(d + 1, e + 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector2(a - 1, e + 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector2(d + 1, b - 1)));

        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector2(a - 1, b - 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector2(d + 1, e + 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector2(a - 1, e + 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector2(d + 1, b - 1)));
    }
    SECTION("Contains_BoundingBox2D")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;

        box.Min = Vector2{a, b};
        box.Max = Vector2{d, e};

        REQUIRE(ContainmentType::Contains == box.Contains(box));

        REQUIRE(ContainmentType::Contains == box.Contains(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 0.01f)}));
        REQUIRE(ContainmentType::Contains == box.Contains(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 0.5f)}));
        REQUIRE(ContainmentType::Contains == box.Contains(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 0.99f)}));
        REQUIRE(ContainmentType::Intersects == box.Contains(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 1.1f)}));
        REQUIRE(ContainmentType::Intersects == box.Contains(BoundingBox2D{Vector2::Zero, Vector2(a, b)}));
        REQUIRE(ContainmentType::Disjoint == box.Contains(BoundingBox2D{Vector2::Zero, Vector2(a - 1, b)}));
    }
    SECTION("Contains_BoundingCircle")
    {
        box.Min = Vector2{12.0f, 16.0f};
        box.Max = Vector2{32.0f, 64.0f};
        auto center = Vector2::Lerp(box.Min, box.Max, 0.5f);

        REQUIRE(ContainmentType::Contains == box.Contains(BoundingCircle{center, 1.0f}));
        REQUIRE(ContainmentType::Contains == box.Contains(BoundingCircle{center, (box.Max - box.Min).X / 2}));
        REQUIRE(ContainmentType::Intersects == box.Contains(BoundingCircle{center, (box.Max - box.Min).Y / 2}));

        REQUIRE(ContainmentType::Intersects == box.Contains(BoundingCircle{Vector2{0.0f, 16.0f}, 12.01f}));
        REQUIRE(ContainmentType::Intersects == box.Contains(BoundingCircle{Vector2{12.0f, 0.0f,}, 16.01f}));
        REQUIRE(ContainmentType::Disjoint == box.Contains(BoundingCircle{Vector2{0.0f, 16.0f}, 11.99f}));
        REQUIRE(ContainmentType::Disjoint == box.Contains(BoundingCircle{Vector2{12.0f, 0.0f}, 15.99f}));
    }
    SECTION("Intersects_BoundingBox2D")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;

        box.Min = Vector2{a, b};
        box.Max = Vector2{d, e};

        REQUIRE(box.Intersects(box));
        REQUIRE(box.Intersects(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 0.01f)}));
        REQUIRE(box.Intersects(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 0.5f)}));
        REQUIRE(box.Intersects(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 0.99f)}));
        REQUIRE(box.Intersects(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 1.1f)}));
        REQUIRE(box.Intersects(BoundingBox2D{Vector2::Zero, Vector2(a, b)}));
        REQUIRE_FALSE(box.Intersects(BoundingBox2D{Vector2::Zero, Vector2(a - 1, b)}));
    }
    SECTION("Intersects_BoundingCircle")
    {
        box.Min = Vector2{12.0f, 16.0f};
        box.Max = Vector2{32.0f, 64.0f};
        auto center = Vector2::Lerp(box.Min, box.Max, 0.5f);

        REQUIRE(box.Intersects(BoundingCircle{center, 1.0f}));
        REQUIRE(box.Intersects(BoundingCircle{center, (box.Max - box.Min).X / 2}));
        REQUIRE(box.Intersects(BoundingCircle{center, (box.Max - box.Min).Y / 2}));

        REQUIRE(box.Intersects(BoundingCircle{Vector2{0.0f, 16.0f}, 12.01f}));
        REQUIRE(box.Intersects(BoundingCircle{Vector2{12.0f, 0.0f}, 16.01f}));
        REQUIRE_FALSE(box.Intersects(BoundingCircle{Vector2{0.0f, 16.0f}, 11.99f}));
        REQUIRE_FALSE(box.Intersects(BoundingCircle{Vector2{12.0f, 0.0f}, 15.99f}));
    }
}
