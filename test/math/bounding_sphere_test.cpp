// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/containment_type.h"
#include <catch_amalgamated.hpp>

using namespace pomdog;

TEST_CASE("BoundingSphere", "[BoundingSphere]")
{
    SECTION("Constructors")
    {
        BoundingSphere sphere;

        BoundingSphere sphere2;
        sphere2.center = Vector3::createZero();
        sphere2.radius = 42.0f;

        sphere = sphere2;
        REQUIRE(Vector3::createZero() == sphere.center);
        REQUIRE(sphere.radius == 42.0f);

        BoundingSphere sphere3{Vector3::createUnitX(), 17.0f};
        sphere = sphere3;
        REQUIRE(Vector3::createUnitX() == sphere.center);
        REQUIRE(Vector3::createUnitX() == sphere3.center);
        REQUIRE(sphere.radius == 17.0f);
        REQUIRE(sphere3.radius == 17.0f);
    }
    SECTION("Contains_Vector3")
    {
        BoundingSphere sphere;
        sphere.center = Vector3::createZero();
        sphere.radius = 42.0f;

        REQUIRE(ContainmentType::Contains == sphere.contains(Vector3::createZero()));
        REQUIRE(ContainmentType::Contains == sphere.contains(Vector3{41.f, 0.f, 0.f}));
        REQUIRE(ContainmentType::Contains == sphere.contains(Vector3{0.f, 41.f, 0.f}));
        REQUIRE(ContainmentType::Contains == sphere.contains(Vector3{0.f, 0.f, 41.f}));
        REQUIRE(ContainmentType::Contains == sphere.contains(Vector3{-41.f, 0.f, 0.f}));
        REQUIRE(ContainmentType::Contains == sphere.contains(Vector3{0.f, -41.f, 0.f}));
        REQUIRE(ContainmentType::Contains == sphere.contains(Vector3{0.f, 0.f, -41.f}));

        REQUIRE(ContainmentType::Intersects == sphere.contains(Vector3{42.f, 0.f, 0.f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(Vector3{0.f, 42.f, 0.f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(Vector3{0.f, 0.f, 42.f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(Vector3{-42.f, 0.f, 0.f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(Vector3{0.f, -42.f, 0.f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(Vector3{0.f, 0.f, -42.f}));

        REQUIRE(ContainmentType::Disjoint == sphere.contains(Vector3{43.f, 0.f, 0.f}));
        REQUIRE(ContainmentType::Disjoint == sphere.contains(Vector3{0.f, 43.f, 0.f}));
        REQUIRE(ContainmentType::Disjoint == sphere.contains(Vector3{0.f, 0.f, 43.f}));
        REQUIRE(ContainmentType::Disjoint == sphere.contains(Vector3{-43.f, 0.f, 0.f}));
        REQUIRE(ContainmentType::Disjoint == sphere.contains(Vector3{0.f, -43.f, 0.f}));
        REQUIRE(ContainmentType::Disjoint == sphere.contains(Vector3{0.f, 0.f, -43.f}));
    }
    SECTION("Contains_BoundingSphere")
    {
        BoundingSphere sphere;
        sphere.center = Vector3::createZero();
        sphere.radius = 42.0f;

        REQUIRE(ContainmentType::Intersects == sphere.contains(sphere));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3::createZero(), 43.0f}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingSphere{Vector3::createZero(), 41.0f}));

        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingSphere{Vector3{40.f, 0.f, 0.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingSphere{Vector3{0.f, 40.f, 0.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingSphere{Vector3{0.f, 0.f, 40.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingSphere{Vector3{-40.f, 0.f, 0.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingSphere{Vector3{0.f, -40.f, 0.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingSphere{Vector3{0.f, 0.f, -40.f}, 1.9f}));

        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3{42.f, 0.f, 0.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3{0.f, 42.f, 0.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3{0.f, 0.f, 42.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3{-42.f, 0.f, 0.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3{0.f, -42.f, 0.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3{0.f, 0.f, -42.f}, 1.0f}));

        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3{43.f, 0.f, 0.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3{0.f, 43.f, 0.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3{0.f, 0.f, 43.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3{-43.f, 0.f, 0.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3{0.f, -43.f, 0.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingSphere{Vector3{0.f, 0.f, -43.f}, 2.0f}));

        REQUIRE(ContainmentType::Disjoint == sphere.contains(BoundingSphere{Vector3{43.f, 0.f, 0.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == sphere.contains(BoundingSphere{Vector3{0.f, 43.f, 0.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == sphere.contains(BoundingSphere{Vector3{0.f, 0.f, 43.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == sphere.contains(BoundingSphere{Vector3{-43.f, 0.f, 0.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == sphere.contains(BoundingSphere{Vector3{0.f, -43.f, 0.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == sphere.contains(BoundingSphere{Vector3{0.f, 0.f, -43.f}, 0.5f}));
    }
    SECTION("Contains_BoundingBox")
    {
        using pomdog::math::normalize;

        BoundingSphere sphere;
        sphere.center = Vector3::createZero();
        sphere.radius = 42.0f;

        const auto min = normalize({-1.f, -1.f, -1.f}) * 42.0f;
        const auto max = normalize({1.f, 1.f, 1.f}) * 42.0f;
        const auto unit = normalize({1.f, 1.f, 1.f}) * 42.0f;

        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingBox{Vector3::createZero(), max}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingBox{Vector3::createZero() - unit * Vector3::createUnitX(), max - unit * Vector3::createUnitX()}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingBox{Vector3::createZero() - unit * Vector3::createUnitY(), max - unit * Vector3::createUnitY()}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingBox{Vector3::createZero() - unit * Vector3::createUnitZ(), max - unit * Vector3::createUnitZ()}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingBox{min, Vector3::createZero()}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingBox{min + unit * Vector3::createUnitX(), Vector3::createZero() + unit * Vector3::createUnitX()}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingBox{min + unit * Vector3::createUnitY(), Vector3::createZero() + unit * Vector3::createUnitY()}));
        REQUIRE(ContainmentType::Contains == sphere.contains(BoundingBox{min + unit * Vector3::createUnitZ(), Vector3::createZero() + unit * Vector3::createUnitZ()}));

        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingBox{Vector3::createZero(), max * 1.01f}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingBox{Vector3::createZero() - unit * Vector3::createUnitX(), max * 1.01f - unit * Vector3::createUnitX()}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingBox{Vector3::createZero() - unit * Vector3::createUnitY(), max * 1.01f - unit * Vector3::createUnitY()}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingBox{Vector3::createZero() - unit * Vector3::createUnitZ(), max * 1.01f - unit * Vector3::createUnitZ()}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingBox{min * 1.01f, Vector3::createZero()}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingBox{min * 1.01f + unit * Vector3::createUnitX(), Vector3::createZero() + unit * Vector3::createUnitX()}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingBox{min * 1.01f + unit * Vector3::createUnitY(), Vector3::createZero() + unit * Vector3::createUnitY()}));
        REQUIRE(ContainmentType::Intersects == sphere.contains(BoundingBox{min * 1.01f + unit * Vector3::createUnitZ(), Vector3::createZero() + unit * Vector3::createUnitZ()}));

        REQUIRE(ContainmentType::Disjoint == sphere.contains(BoundingBox{max * 1.01f, max * 1.01f + unit}));
    }
    SECTION("Intersects_BoundingBox")
    {
        using pomdog::math::normalize;

        BoundingSphere sphere;
        sphere.center = Vector3::createZero();
        sphere.radius = 42.0f;

        const auto min = normalize({-1.f, -1.f, -1.f}) * 42.0f;
        const auto max = normalize({1.f, 1.f, 1.f}) * 42.0f;
        const auto unit = normalize({1.f, 1.f, 1.f}) * 42.0f;

        REQUIRE(sphere.intersects(BoundingBox{Vector3::createZero(), max}));
        REQUIRE(sphere.intersects(BoundingBox{Vector3::createZero() - unit * Vector3::createUnitX(), max - unit * Vector3::createUnitX()}));
        REQUIRE(sphere.intersects(BoundingBox{Vector3::createZero() - unit * Vector3::createUnitY(), max - unit * Vector3::createUnitY()}));
        REQUIRE(sphere.intersects(BoundingBox{Vector3::createZero() - unit * Vector3::createUnitZ(), max - unit * Vector3::createUnitZ()}));
        REQUIRE(sphere.intersects(BoundingBox{min, Vector3::createZero()}));
        REQUIRE(sphere.intersects(BoundingBox{min + unit * Vector3::createUnitX(), Vector3::createZero() + unit * Vector3::createUnitX()}));
        REQUIRE(sphere.intersects(BoundingBox{min + unit * Vector3::createUnitY(), Vector3::createZero() + unit * Vector3::createUnitY()}));
        REQUIRE(sphere.intersects(BoundingBox{min + unit * Vector3::createUnitZ(), Vector3::createZero() + unit * Vector3::createUnitZ()}));

        REQUIRE(sphere.intersects(BoundingBox{Vector3::createZero(), max * 1.01f}));
        REQUIRE(sphere.intersects(BoundingBox{Vector3::createZero() - unit * Vector3::createUnitX(), max * 1.01f - unit * Vector3::createUnitX()}));
        REQUIRE(sphere.intersects(BoundingBox{Vector3::createZero() - unit * Vector3::createUnitY(), max * 1.01f - unit * Vector3::createUnitY()}));
        REQUIRE(sphere.intersects(BoundingBox{Vector3::createZero() - unit * Vector3::createUnitZ(), max * 1.01f - unit * Vector3::createUnitZ()}));
        REQUIRE(sphere.intersects(BoundingBox{min * 1.01f, Vector3::createZero()}));
        REQUIRE(sphere.intersects(BoundingBox{min * 1.01f + unit * Vector3::createUnitX(), Vector3::createZero() + unit * Vector3::createUnitX()}));
        REQUIRE(sphere.intersects(BoundingBox{min * 1.01f + unit * Vector3::createUnitY(), Vector3::createZero() + unit * Vector3::createUnitY()}));
        REQUIRE(sphere.intersects(BoundingBox{min * 1.01f + unit * Vector3::createUnitZ(), Vector3::createZero() + unit * Vector3::createUnitZ()}));

        REQUIRE_FALSE(sphere.intersects(BoundingBox{max * 1.01f, max * 1.01f + unit}));
    }
    SECTION("Intersects_BoundingSphere")
    {
        BoundingSphere sphere;
        sphere.center = Vector3::createZero();
        sphere.radius = 42.0f;

        REQUIRE(sphere.intersects(sphere));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3::createZero(), 43.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3::createZero(), 41.0f}));

        REQUIRE(sphere.intersects(BoundingSphere{Vector3{40.f, 0.f, 0.f}, 1.9f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.f, 40.f, 0.f}, 1.9f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.f, 0.f, 40.f}, 1.9f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{-40.f, 0.f, 0.f}, 1.9f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.f, -40.f, 0.f}, 1.9f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.f, 0.f, -40.f}, 1.9f}));

        REQUIRE(sphere.intersects(BoundingSphere{Vector3{42.f, 0.f, 0.f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.f, 42.f, 0.f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.f, 0.f, 42.f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{-42.f, 0.f, 0.f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.f, -42.f, 0.f}, 1.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.f, 0.f, -42.f}, 1.0f}));

        REQUIRE(sphere.intersects(BoundingSphere{Vector3{43.f, 0.f, 0.f}, 2.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.f, 43.f, 0.f}, 2.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.f, 0.f, 43.f}, 2.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{-43.f, 0.f, 0.f}, 2.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.f, -43.f, 0.f}, 2.0f}));
        REQUIRE(sphere.intersects(BoundingSphere{Vector3{0.f, 0.f, -43.f}, 2.0f}));

        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{43.f, 0.f, 0.f}, 0.5f}));
        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{0.f, 43.f, 0.f}, 0.5f}));
        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{0.f, 0.f, 43.f}, 0.5f}));
        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{-43.f, 0.f, 0.f}, 0.5f}));
        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{0.f, -43.f, 0.f}, 0.5f}));
        REQUIRE_FALSE(sphere.intersects(BoundingSphere{Vector3{0.f, 0.f, -43.f}, 0.5f}));
    }
}
