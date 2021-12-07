// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_box.hpp"
#include "pomdog/math/bounding_sphere.hpp"
#include "pomdog/math/containment_type.hpp"
#include <catch_amalgamated.hpp>

using namespace Pomdog;

TEST_CASE("BoundingBox", "[BoundingBox]")
{
    BoundingBox box;

    SECTION("CornerCount")
    {
        STATIC_REQUIRE(BoundingBox::CornerCount == 8);
    }
    SECTION("Constructors")
    {
        BoundingBox box2;
        box2.Min = Vector3::Zero;
        box2.Max = Vector3::UnitY;

        box = box2;
        REQUIRE(Vector3::Zero == box.Min);
        REQUIRE(Vector3::UnitY == box.Max);

        BoundingBox box3{Vector3::UnitX, Vector3::UnitZ};
        box = box3;
        REQUIRE(Vector3::UnitX == box.Min);
        REQUIRE(Vector3::UnitX == box3.Min);
        REQUIRE(Vector3::UnitZ == box.Max);
        REQUIRE(Vector3::UnitZ == box3.Max);
    }
    SECTION("GetCorners")
    {
        box.Min = Vector3{12.0f, 13.0f, 14.0f};
        box.Max = Vector3{15.0f, 16.0f, 17.0f};

        auto corners = box.GetCorners();
        REQUIRE_FALSE(corners.empty());
        REQUIRE(BoundingBox::CornerCount == corners.size());
        REQUIRE(Vector3{12.0f, 13.0f, 14.0f} == box.Min);
        REQUIRE(Vector3{15.0f, 16.0f, 17.0f} == box.Max);
        REQUIRE(corners[0] == Vector3(box.Min.X, box.Max.Y, box.Max.Z));
        REQUIRE(corners[1] == Vector3(box.Max.X, box.Max.Y, box.Max.Z));
        REQUIRE(corners[2] == Vector3(box.Max.X, box.Min.Y, box.Max.Z));
        REQUIRE(corners[3] == Vector3(box.Min.X, box.Min.Y, box.Max.Z));
        REQUIRE(corners[4] == Vector3(box.Min.X, box.Max.Y, box.Min.Z));
        REQUIRE(corners[5] == Vector3(box.Max.X, box.Max.Y, box.Min.Z));
        REQUIRE(corners[6] == Vector3(box.Max.X, box.Min.Y, box.Min.Z));
        REQUIRE(corners[7] == Vector3(box.Min.X, box.Min.Y, box.Min.Z));
    }
    SECTION("Contains_Vector3")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto c = 24.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;
        constexpr auto f = 96.0f;

        box.Min = Vector3{a, b, c};
        box.Max = Vector3{d, e, f};

        REQUIRE(ContainmentType::Contains == box.Contains(Vector3(a + 1, b + 1, c + 1)));
        REQUIRE(ContainmentType::Contains == box.Contains(Vector3(d - 1, b + 1, c + 1)));
        REQUIRE(ContainmentType::Contains == box.Contains(Vector3(a + 1, e - 1, c + 1)));
        REQUIRE(ContainmentType::Contains == box.Contains(Vector3(d - 1, e - 1, c + 1)));
        REQUIRE(ContainmentType::Contains == box.Contains(Vector3(a + 1, b + 1, f - 1)));
        REQUIRE(ContainmentType::Contains == box.Contains(Vector3(d - 1, b + 1, f - 1)));
        REQUIRE(ContainmentType::Contains == box.Contains(Vector3(a + 1, e - 1, f - 1)));
        REQUIRE(ContainmentType::Contains == box.Contains(Vector3(d - 1, e - 1, f - 1)));

        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(a, b, c)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(d, b, c)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(a, e, c)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(d, e, c)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(a, b, f)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(d, b, f)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(a, e, f)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(d, e, f)));

        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3((d + a) / 2, b, c)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3((d + a) / 2, b, f)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3((d + a) / 2, e, c)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3((d + a) / 2, e, f)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(a, (e + b) / 2, c)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(a, (e + b) / 2, f)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(d, (e + b) / 2, c)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(d, (e + b) / 2, f)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(a, b, (f + c) / 2)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(a, e, (f + c) / 2)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(d, b, (f + c) / 2)));
        REQUIRE(ContainmentType::Intersects == box.Contains(Vector3(d, e, (f + c) / 2)));

        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(a - 1, b, c)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(d + 1, b, c)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(a, b - 1, c)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(d, e + 1, c)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(a, b, c - 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(d, b, f + 1)));

        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(a - 1, b - 1, c)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(d + 1, e + 1, c)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(a - 1, e + 1, c)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(d + 1, b - 1, c)));

        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(a - 1, b - 1, c - 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(d + 1, e + 1, c - 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(a - 1, e + 1, c - 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(d + 1, b - 1, c - 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(a - 1, b - 1, f + 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(d + 1, e + 1, f + 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(a - 1, e + 1, f + 1)));
        REQUIRE(ContainmentType::Disjoint == box.Contains(Vector3(d + 1, b - 1, f + 1)));
    }
    SECTION("Contains_BoundingBox")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto c = 24.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;
        constexpr auto f = 96.0f;

        box.Min = Vector3{a, b, c};
        box.Max = Vector3{d, e, f};

        REQUIRE(ContainmentType::Contains == box.Contains(box));

        REQUIRE(ContainmentType::Contains == box.Contains(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 0.01f)}));
        REQUIRE(ContainmentType::Contains == box.Contains(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 0.5f)}));
        REQUIRE(ContainmentType::Contains == box.Contains(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 0.99f)}));
        REQUIRE(ContainmentType::Intersects == box.Contains(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 1.1f)}));
        REQUIRE(ContainmentType::Intersects == box.Contains(BoundingBox{Vector3::Zero, Vector3(a, b, c)}));
        REQUIRE(ContainmentType::Disjoint == box.Contains(BoundingBox{Vector3::Zero, Vector3(a - 1, b, c)}));
    }
    SECTION("Contains_BoundingSphere")
    {
        box.Min = Vector3{12.0f, 16.0f, 24.0f};
        box.Max = Vector3{32.0f, 64.0f, 96.0f};
        auto center = Vector3::Lerp(box.Min, box.Max, 0.5f);

        REQUIRE(ContainmentType::Contains == box.Contains(BoundingSphere{center, 1.0f}));
        REQUIRE(ContainmentType::Contains == box.Contains(BoundingSphere{center, (box.Max - box.Min).X / 2}));
        REQUIRE(ContainmentType::Intersects == box.Contains(BoundingSphere{center, (box.Max - box.Min).Y / 2}));
        REQUIRE(ContainmentType::Intersects == box.Contains(BoundingSphere{center, (box.Max - box.Min).Z / 2}));

        REQUIRE(ContainmentType::Intersects == box.Contains(BoundingSphere{Vector3{0.0f, 16.0f, 24.0f}, 12.01f}));
        REQUIRE(ContainmentType::Intersects == box.Contains(BoundingSphere{Vector3{12.0f, 0.0f, 24.0f}, 16.01f}));
        REQUIRE(ContainmentType::Intersects == box.Contains(BoundingSphere{Vector3{12.0f, 16.0f, 0.0f}, 24.01f}));
        REQUIRE(ContainmentType::Disjoint == box.Contains(BoundingSphere{Vector3{0.0f, 16.0f, 24.0f}, 11.99f}));
        REQUIRE(ContainmentType::Disjoint == box.Contains(BoundingSphere{Vector3{12.0f, 0.0f, 24.0f}, 15.99f}));
        REQUIRE(ContainmentType::Disjoint == box.Contains(BoundingSphere{Vector3{12.0f, 16.0f, 0.0f}, 23.99f}));
    }
    SECTION("Intersects_BoundingBox")
    {
        constexpr auto a = 12.0f;
        constexpr auto b = 16.0f;
        constexpr auto c = 24.0f;
        constexpr auto d = 32.0f;
        constexpr auto e = 64.0f;
        constexpr auto f = 96.0f;

        box.Min = Vector3{a, b, c};
        box.Max = Vector3{d, e, f};

        REQUIRE(box.Intersects(box));
        REQUIRE(box.Intersects(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 0.01f)}));
        REQUIRE(box.Intersects(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 0.5f)}));
        REQUIRE(box.Intersects(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 0.99f)}));
        REQUIRE(box.Intersects(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 1.1f)}));
        REQUIRE(box.Intersects(BoundingBox{Vector3::Zero, Vector3(a, b, c)}));
        REQUIRE_FALSE(box.Intersects(BoundingBox{Vector3::Zero, Vector3(a - 1, b, c)}));
    }
    SECTION("Intersects_BoundingSphere")
    {
        box.Min = Vector3{12.0f, 16.0f, 24.0f};
        box.Max = Vector3{32.0f, 64.0f, 96.0f};
        auto center = Vector3::Lerp(box.Min, box.Max, 0.5f);

        REQUIRE(box.Intersects(BoundingSphere{center, 1.0f}));
        REQUIRE(box.Intersects(BoundingSphere{center, (box.Max - box.Min).X / 2}));
        REQUIRE(box.Intersects(BoundingSphere{center, (box.Max - box.Min).Y / 2}));
        REQUIRE(box.Intersects(BoundingSphere{center, (box.Max - box.Min).Z / 2}));

        REQUIRE(box.Intersects(BoundingSphere{Vector3{0.0f, 16.0f, 24.0f}, 12.01f}));
        REQUIRE(box.Intersects(BoundingSphere{Vector3{12.0f, 0.0f, 24.0f}, 16.01f}));
        REQUIRE(box.Intersects(BoundingSphere{Vector3{12.0f, 16.0f, 0.0f}, 24.01f}));
        REQUIRE_FALSE(box.Intersects(BoundingSphere{Vector3{0.0f, 16.0f, 24.0f}, 11.99f}));
        REQUIRE_FALSE(box.Intersects(BoundingSphere{Vector3{12.0f, 0.0f, 24.0f}, 15.99f}));
        REQUIRE_FALSE(box.Intersects(BoundingSphere{Vector3{12.0f, 16.0f, 0.0f}, 23.99f}));
    }
}
