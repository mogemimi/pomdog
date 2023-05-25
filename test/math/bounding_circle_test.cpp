// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_box2d.h"
#include "pomdog/math/bounding_circle.h"
#include "pomdog/math/containment_type.h"
#include <catch_amalgamated.hpp>

using namespace pomdog;

TEST_CASE("BoundingCircle", "[BoundingCircle]")
{
    SECTION("Constructors")
    {
        BoundingCircle circle;

        BoundingCircle circle2;
        circle2.center = Vector2::createZero();
        circle2.radius = 42.0f;

        circle = circle2;
        REQUIRE(Vector2::createZero() == circle.center);
        REQUIRE(circle.radius == 42.0f);

        BoundingCircle circle3{Vector2{3.14f, 2.718f}, 17.0f};
        circle = circle3;
        REQUIRE(Vector2{3.14f, 2.718f} == circle.center);
        REQUIRE(Vector2{3.14f, 2.718f} == circle3.center);
        REQUIRE(circle.radius == 17.0f);
        REQUIRE(circle3.radius == 17.0f);
    }
    SECTION("Contains_Vector2")
    {
        BoundingCircle circle;
        circle.center = Vector2::createZero();
        circle.radius = 42.0f;

        REQUIRE(ContainmentType::Contains == circle.contains(Vector2::createZero()));
        REQUIRE(ContainmentType::Contains == circle.contains(Vector2{41.f, 0.f}));
        REQUIRE(ContainmentType::Contains == circle.contains(Vector2{0.f, 41.f}));
        REQUIRE(ContainmentType::Contains == circle.contains(Vector2{-41.f, 0.f}));
        REQUIRE(ContainmentType::Contains == circle.contains(Vector2{0.f, -41.f}));

        REQUIRE(ContainmentType::Intersects == circle.contains(Vector2{42.f, 0.f}));
        REQUIRE(ContainmentType::Intersects == circle.contains(Vector2{0.f, 42.f}));
        REQUIRE(ContainmentType::Intersects == circle.contains(Vector2{-42.f, 0.f}));
        REQUIRE(ContainmentType::Intersects == circle.contains(Vector2{0.f, -42.f}));

        REQUIRE(ContainmentType::Disjoint == circle.contains(Vector2{43.f, 0.f}));
        REQUIRE(ContainmentType::Disjoint == circle.contains(Vector2{0.f, 43.f}));
        REQUIRE(ContainmentType::Disjoint == circle.contains(Vector2{-43.f, 0.f}));
        REQUIRE(ContainmentType::Disjoint == circle.contains(Vector2{0.f, -43.f}));
    }
    SECTION("Contains_BoundingCircle")
    {
        BoundingCircle circle;
        circle.center = Vector2::createZero();
        circle.radius = 42.0f;

        REQUIRE(ContainmentType::Intersects == circle.contains(circle));
        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingCircle{Vector2::createZero(), 43.0f}));
        REQUIRE(ContainmentType::Contains == circle.contains(BoundingCircle{Vector2::createZero(), 41.0f}));

        REQUIRE(ContainmentType::Contains == circle.contains(BoundingCircle{Vector2{40.f, 0.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == circle.contains(BoundingCircle{Vector2{0.f, 40.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == circle.contains(BoundingCircle{Vector2{-40.f, 0.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == circle.contains(BoundingCircle{Vector2{0.f, -40.f}, 1.9f}));

        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingCircle{Vector2{42.f, 0.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingCircle{Vector2{0.f, 42.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingCircle{Vector2{-42.f, 0.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingCircle{Vector2{0.f, -42.f}, 1.0f}));

        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingCircle{Vector2{43.f, 0.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingCircle{Vector2{0.f, 43.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingCircle{Vector2{-43.f, 0.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingCircle{Vector2{0.f, -43.f}, 2.0f}));

        REQUIRE(ContainmentType::Disjoint == circle.contains(BoundingCircle{Vector2{43.f, 0.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == circle.contains(BoundingCircle{Vector2{0.f, 43.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == circle.contains(BoundingCircle{Vector2{-43.f, 0.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == circle.contains(BoundingCircle{Vector2{0.f, -43.f}, 0.5f}));
    }
    SECTION("Contains_BoundingBox2D")
    {
        BoundingCircle circle;
        circle.center = Vector2::createZero();
        circle.radius = 42.0f;

        const auto min = pomdog::math::normalize({-1.f, -1.f}) * 42.0f;
        const auto max = pomdog::math::normalize({1.f, 1.f}) * 42.0f;
        const auto unit = pomdog::math::normalize({1.f, 1.f}) * 42.0f;

        REQUIRE(ContainmentType::Contains == circle.contains(BoundingBox2D{Vector2::createZero(), max}));
        REQUIRE(ContainmentType::Contains == circle.contains(BoundingBox2D{Vector2::createZero() - unit * Vector2{1, 0}, max - unit * Vector2{1, 0}}));
        REQUIRE(ContainmentType::Contains == circle.contains(BoundingBox2D{Vector2::createZero() - unit * Vector2{0, 1}, max - unit * Vector2{0, 1}}));
        REQUIRE(ContainmentType::Contains == circle.contains(BoundingBox2D{min, Vector2::createZero()}));
        REQUIRE(ContainmentType::Contains == circle.contains(BoundingBox2D{min + unit * Vector2{1, 0}, Vector2::createZero() + unit * Vector2{1, 0}}));
        REQUIRE(ContainmentType::Contains == circle.contains(BoundingBox2D{min + unit * Vector2{0, 1}, Vector2::createZero() + unit * Vector2{0, 1}}));

        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingBox2D{Vector2::createZero(), max * 1.01f}));
        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingBox2D{Vector2::createZero() - unit * Vector2{1, 0}, max * 1.01f - unit * Vector2{1, 0}}));
        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingBox2D{Vector2::createZero() - unit * Vector2{0, 1}, max * 1.01f - unit * Vector2{0, 1}}));
        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingBox2D{min * 1.01f, Vector2::createZero()}));
        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingBox2D{min * 1.01f + unit * Vector2{1, 0}, Vector2::createZero() + unit * Vector2{1, 0}}));
        REQUIRE(ContainmentType::Intersects == circle.contains(BoundingBox2D{min * 1.01f + unit * Vector2{0, 1}, Vector2::createZero() + unit * Vector2{0, 1}}));

        REQUIRE(ContainmentType::Disjoint == circle.contains(BoundingBox2D{max * 1.01f, max * 1.01f + unit}));
    }
    SECTION("Intersects_BoundingBox2D")
    {
        BoundingCircle circle;
        circle.center = Vector2::createZero();
        circle.radius = 42.0f;

        const auto min = pomdog::math::normalize({-1.f, -1.f}) * 42.0f;
        const auto max = pomdog::math::normalize({1.f, 1.f}) * 42.0f;
        const auto unit = pomdog::math::normalize({1.f, 1.f}) * 42.0f;

        REQUIRE(circle.intersects(BoundingBox2D{Vector2::createZero(), max}));
        REQUIRE(circle.intersects(BoundingBox2D{Vector2::createZero() - unit * Vector2{1, 0}, max - unit * Vector2{1, 0}}));
        REQUIRE(circle.intersects(BoundingBox2D{Vector2::createZero() - unit * Vector2{0, 1}, max - unit * Vector2{0, 1}}));
        REQUIRE(circle.intersects(BoundingBox2D{min, Vector2::createZero()}));
        REQUIRE(circle.intersects(BoundingBox2D{min + unit * Vector2{1, 0}, Vector2::createZero() + unit * Vector2{1, 0}}));
        REQUIRE(circle.intersects(BoundingBox2D{min + unit * Vector2{0, 1}, Vector2::createZero() + unit * Vector2{0, 1}}));

        REQUIRE(circle.intersects(BoundingBox2D{Vector2::createZero(), max * 1.01f}));
        REQUIRE(circle.intersects(BoundingBox2D{Vector2::createZero() - unit * Vector2{1, 0}, max * 1.01f - unit * Vector2{1, 0}}));
        REQUIRE(circle.intersects(BoundingBox2D{Vector2::createZero() - unit * Vector2{0, 1}, max * 1.01f - unit * Vector2{0, 1}}));
        REQUIRE(circle.intersects(BoundingBox2D{min * 1.01f, Vector2::createZero()}));
        REQUIRE(circle.intersects(BoundingBox2D{min * 1.01f + unit * Vector2{1, 0}, Vector2::createZero() + unit * Vector2{1, 0}}));
        REQUIRE(circle.intersects(BoundingBox2D{min * 1.01f + unit * Vector2{0, 1}, Vector2::createZero() + unit * Vector2{0, 1}}));

        REQUIRE_FALSE(circle.intersects(BoundingBox2D{max * 1.01f, max * 1.01f + unit}));
    }
    SECTION("Intersects_BoundingCircle")
    {
        BoundingCircle circle;
        circle.center = Vector2::createZero();
        circle.radius = 42.0f;

        REQUIRE(circle.intersects(circle));
        REQUIRE(circle.intersects(BoundingCircle{Vector2::createZero(), 43.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2::createZero(), 41.0f}));

        REQUIRE(circle.intersects(BoundingCircle{Vector2{40.f, 0.f}, 1.9f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{0.f, 40.f}, 1.9f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{-40.f, 0.f}, 1.9f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{0.f, -40.f}, 1.9f}));

        REQUIRE(circle.intersects(BoundingCircle{Vector2{42.f, 0.f}, 1.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{0.f, 42.f}, 1.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{-42.f, 0.f}, 1.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{0.f, -42.f}, 1.0f}));

        REQUIRE(circle.intersects(BoundingCircle{Vector2{43.f, 0.f}, 2.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{0.f, 43.f}, 2.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{-43.f, 0.f}, 2.0f}));
        REQUIRE(circle.intersects(BoundingCircle{Vector2{0.f, -43.f}, 2.0f}));

        REQUIRE_FALSE(circle.intersects(BoundingCircle{Vector2{43.f, 0.f}, 0.5f}));
        REQUIRE_FALSE(circle.intersects(BoundingCircle{Vector2{0.f, 43.f}, 0.5f}));
        REQUIRE_FALSE(circle.intersects(BoundingCircle{Vector2{-43.f, 0.f}, 0.5f}));
        REQUIRE_FALSE(circle.intersects(BoundingCircle{Vector2{0.f, -43.f}, 0.5f}));
    }
}
