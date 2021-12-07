// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_sphere.hpp"
#include "pomdog/math/bounding_box.hpp"
#include "pomdog/math/containment_type.hpp"
#include "catch_amalgamated.hpp"

using namespace Pomdog;

TEST_CASE("BoundingSphere", "[BoundingSphere]")
{
    SECTION("Constructors")
    {
        BoundingSphere sphere;

        BoundingSphere sphere2;
        sphere2.Center = Vector3::Zero;
        sphere2.Radius = 42.0f;

        sphere = sphere2;
        REQUIRE(Vector3::Zero == sphere.Center);
        REQUIRE(sphere.Radius == 42.0f);

        BoundingSphere sphere3{Vector3::UnitX, 17.0f};
        sphere = sphere3;
        REQUIRE(Vector3::UnitX == sphere.Center);
        REQUIRE(Vector3::UnitX == sphere3.Center);
        REQUIRE(sphere.Radius == 17.0f);
        REQUIRE(sphere3.Radius == 17.0f);
    }
    SECTION("Contains_Vector3")
    {
        BoundingSphere sphere;
        sphere.Center = Vector3::Zero;
        sphere.Radius = 42.0f;

        REQUIRE(ContainmentType::Contains == sphere.Contains(Vector3::Zero));
        REQUIRE(ContainmentType::Contains == sphere.Contains(Vector3{41.f, 0.f, 0.f}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(Vector3{0.f, 41.f, 0.f}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(Vector3{0.f, 0.f, 41.f}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(Vector3{-41.f, 0.f, 0.f}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(Vector3{0.f, -41.f, 0.f}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(Vector3{0.f, 0.f, -41.f}));

        REQUIRE(ContainmentType::Intersects == sphere.Contains(Vector3{42.f, 0.f, 0.f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(Vector3{0.f, 42.f, 0.f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(Vector3{0.f, 0.f, 42.f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(Vector3{-42.f, 0.f, 0.f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(Vector3{0.f, -42.f, 0.f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(Vector3{0.f, 0.f, -42.f}));

        REQUIRE(ContainmentType::Disjoint == sphere.Contains(Vector3{43.f, 0.f, 0.f}));
        REQUIRE(ContainmentType::Disjoint == sphere.Contains(Vector3{0.f, 43.f, 0.f}));
        REQUIRE(ContainmentType::Disjoint == sphere.Contains(Vector3{0.f, 0.f, 43.f}));
        REQUIRE(ContainmentType::Disjoint == sphere.Contains(Vector3{-43.f, 0.f, 0.f}));
        REQUIRE(ContainmentType::Disjoint == sphere.Contains(Vector3{0.f, -43.f, 0.f}));
        REQUIRE(ContainmentType::Disjoint == sphere.Contains(Vector3{0.f, 0.f, -43.f}));
    }
    SECTION("Contains_BoundingSphere")
    {
        BoundingSphere sphere;
        sphere.Center = Vector3::Zero;
        sphere.Radius = 42.0f;

        REQUIRE(ContainmentType::Intersects == sphere.Contains(sphere));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3::Zero, 43.0f}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingSphere{Vector3::Zero, 41.0f}));

        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingSphere{Vector3{40.f, 0.f, 0.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingSphere{Vector3{0.f, 40.f, 0.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, 40.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingSphere{Vector3{-40.f, 0.f, 0.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingSphere{Vector3{0.f, -40.f, 0.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, -40.f}, 1.9f}));

        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3{42.f, 0.f, 0.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3{0.f, 42.f, 0.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, 42.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3{-42.f, 0.f, 0.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3{0.f, -42.f, 0.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, -42.f}, 1.0f}));

        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3{43.f, 0.f, 0.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3{0.f, 43.f, 0.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, 43.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3{-43.f, 0.f, 0.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3{0.f, -43.f, 0.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, -43.f}, 2.0f}));

        REQUIRE(ContainmentType::Disjoint == sphere.Contains(BoundingSphere{Vector3{43.f, 0.f, 0.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == sphere.Contains(BoundingSphere{Vector3{0.f, 43.f, 0.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, 43.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == sphere.Contains(BoundingSphere{Vector3{-43.f, 0.f, 0.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == sphere.Contains(BoundingSphere{Vector3{0.f, -43.f, 0.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == sphere.Contains(BoundingSphere{Vector3{0.f, 0.f, -43.f}, 0.5f}));
    }
    SECTION("Contains_BoundingBox")
    {
        BoundingSphere sphere;
        sphere.Center = Vector3::Zero;
        sphere.Radius = 42.0f;

        auto min = Vector3::Normalize({-1.f, -1.f, -1.f}) * 42.0f;
        auto max = Vector3::Normalize({1.f, 1.f, 1.f}) * 42.0f;
        auto unit = Vector3::Normalize({1.f, 1.f, 1.f}) * 42.0f;

        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingBox{Vector3::Zero, max}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingBox{Vector3::Zero - unit * Vector3::UnitX, max - unit * Vector3::UnitX}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingBox{Vector3::Zero - unit * Vector3::UnitY, max - unit * Vector3::UnitY}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingBox{Vector3::Zero - unit * Vector3::UnitZ, max - unit * Vector3::UnitZ}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingBox{min, Vector3::Zero}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingBox{min + unit * Vector3::UnitX, Vector3::Zero + unit * Vector3::UnitX}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingBox{min + unit * Vector3::UnitY, Vector3::Zero + unit * Vector3::UnitY}));
        REQUIRE(ContainmentType::Contains == sphere.Contains(BoundingBox{min + unit * Vector3::UnitZ, Vector3::Zero + unit * Vector3::UnitZ}));

        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingBox{Vector3::Zero, max * 1.01f}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingBox{Vector3::Zero - unit * Vector3::UnitX, max * 1.01f - unit * Vector3::UnitX}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingBox{Vector3::Zero - unit * Vector3::UnitY, max * 1.01f - unit * Vector3::UnitY}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingBox{Vector3::Zero - unit * Vector3::UnitZ, max * 1.01f - unit * Vector3::UnitZ}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingBox{min * 1.01f, Vector3::Zero}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingBox{min * 1.01f + unit * Vector3::UnitX, Vector3::Zero + unit * Vector3::UnitX}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingBox{min * 1.01f + unit * Vector3::UnitY, Vector3::Zero + unit * Vector3::UnitY}));
        REQUIRE(ContainmentType::Intersects == sphere.Contains(BoundingBox{min * 1.01f + unit * Vector3::UnitZ, Vector3::Zero + unit * Vector3::UnitZ}));

        REQUIRE(ContainmentType::Disjoint == sphere.Contains(BoundingBox{max * 1.01f, max * 1.01f + unit}));
    }
    SECTION("Intersects_BoundingBox")
    {
        BoundingSphere sphere;
        sphere.Center = Vector3::Zero;
        sphere.Radius = 42.0f;

        auto min = Vector3::Normalize({-1.f, -1.f, -1.f}) * 42.0f;
        auto max = Vector3::Normalize({1.f, 1.f, 1.f}) * 42.0f;
        auto unit = Vector3::Normalize({1.f, 1.f, 1.f}) * 42.0f;

        REQUIRE(sphere.Intersects(BoundingBox{Vector3::Zero, max}));
        REQUIRE(sphere.Intersects(BoundingBox{Vector3::Zero - unit * Vector3::UnitX, max - unit * Vector3::UnitX}));
        REQUIRE(sphere.Intersects(BoundingBox{Vector3::Zero - unit * Vector3::UnitY, max - unit * Vector3::UnitY}));
        REQUIRE(sphere.Intersects(BoundingBox{Vector3::Zero - unit * Vector3::UnitZ, max - unit * Vector3::UnitZ}));
        REQUIRE(sphere.Intersects(BoundingBox{min, Vector3::Zero}));
        REQUIRE(sphere.Intersects(BoundingBox{min + unit * Vector3::UnitX, Vector3::Zero + unit * Vector3::UnitX}));
        REQUIRE(sphere.Intersects(BoundingBox{min + unit * Vector3::UnitY, Vector3::Zero + unit * Vector3::UnitY}));
        REQUIRE(sphere.Intersects(BoundingBox{min + unit * Vector3::UnitZ, Vector3::Zero + unit * Vector3::UnitZ}));

        REQUIRE(sphere.Intersects(BoundingBox{Vector3::Zero, max * 1.01f}));
        REQUIRE(sphere.Intersects(BoundingBox{Vector3::Zero - unit * Vector3::UnitX, max * 1.01f - unit * Vector3::UnitX}));
        REQUIRE(sphere.Intersects(BoundingBox{Vector3::Zero - unit * Vector3::UnitY, max * 1.01f - unit * Vector3::UnitY}));
        REQUIRE(sphere.Intersects(BoundingBox{Vector3::Zero - unit * Vector3::UnitZ, max * 1.01f - unit * Vector3::UnitZ}));
        REQUIRE(sphere.Intersects(BoundingBox{min * 1.01f, Vector3::Zero}));
        REQUIRE(sphere.Intersects(BoundingBox{min * 1.01f + unit * Vector3::UnitX, Vector3::Zero + unit * Vector3::UnitX}));
        REQUIRE(sphere.Intersects(BoundingBox{min * 1.01f + unit * Vector3::UnitY, Vector3::Zero + unit * Vector3::UnitY}));
        REQUIRE(sphere.Intersects(BoundingBox{min * 1.01f + unit * Vector3::UnitZ, Vector3::Zero + unit * Vector3::UnitZ}));

        REQUIRE_FALSE(sphere.Intersects(BoundingBox{max * 1.01f, max * 1.01f + unit}));
    }
    SECTION("Intersects_BoundingSphere")
    {
        BoundingSphere sphere;
        sphere.Center = Vector3::Zero;
        sphere.Radius = 42.0f;

        REQUIRE(sphere.Intersects(sphere));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3::Zero, 43.0f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3::Zero, 41.0f}));

        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{40.f, 0.f, 0.f}, 1.9f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{0.f, 40.f, 0.f}, 1.9f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, 40.f}, 1.9f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{-40.f, 0.f, 0.f}, 1.9f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{0.f, -40.f, 0.f}, 1.9f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, -40.f}, 1.9f}));

        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{42.f, 0.f, 0.f}, 1.0f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{0.f, 42.f, 0.f}, 1.0f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, 42.f}, 1.0f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{-42.f, 0.f, 0.f}, 1.0f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{0.f, -42.f, 0.f}, 1.0f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, -42.f}, 1.0f}));

        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{43.f, 0.f, 0.f}, 2.0f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{0.f, 43.f, 0.f}, 2.0f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, 43.f}, 2.0f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{-43.f, 0.f, 0.f}, 2.0f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{0.f, -43.f, 0.f}, 2.0f}));
        REQUIRE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, -43.f}, 2.0f}));

        REQUIRE_FALSE(sphere.Intersects(BoundingSphere{Vector3{43.f, 0.f, 0.f}, 0.5f}));
        REQUIRE_FALSE(sphere.Intersects(BoundingSphere{Vector3{0.f, 43.f, 0.f}, 0.5f}));
        REQUIRE_FALSE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, 43.f}, 0.5f}));
        REQUIRE_FALSE(sphere.Intersects(BoundingSphere{Vector3{-43.f, 0.f, 0.f}, 0.5f}));
        REQUIRE_FALSE(sphere.Intersects(BoundingSphere{Vector3{0.f, -43.f, 0.f}, 0.5f}));
        REQUIRE_FALSE(sphere.Intersects(BoundingSphere{Vector3{0.f, 0.f, -43.f}, 0.5f}));
    }
}
