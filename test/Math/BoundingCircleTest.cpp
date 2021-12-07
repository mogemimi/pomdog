// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_circle.hpp"
#include "pomdog/math/bounding_box2d.hpp"
#include "pomdog/math/containment_type.hpp"
#include "catch_amalgamated.hpp"

using namespace Pomdog;

TEST_CASE("BoundingCircle", "[BoundingCircle]")
{
    SECTION("Constructors")
    {
        BoundingCircle circle;

        BoundingCircle circle2;
        circle2.Center = Vector2::Zero;
        circle2.Radius = 42.0f;

        circle = circle2;
        REQUIRE(Vector2::Zero == circle.Center);
        REQUIRE(circle.Radius == 42.0f);

        BoundingCircle circle3{Vector2{3.14f, 2.718f}, 17.0f};
        circle = circle3;
        REQUIRE(Vector2{3.14f, 2.718f} == circle.Center);
        REQUIRE(Vector2{3.14f, 2.718f} == circle3.Center);
        REQUIRE(circle.Radius == 17.0f);
        REQUIRE(circle3.Radius == 17.0f);
    }
    SECTION("Contains_Vector2")
    {
        BoundingCircle circle;
        circle.Center = Vector2::Zero;
        circle.Radius = 42.0f;

        REQUIRE(ContainmentType::Contains == circle.Contains(Vector2::Zero));
        REQUIRE(ContainmentType::Contains == circle.Contains(Vector2{41.f, 0.f}));
        REQUIRE(ContainmentType::Contains == circle.Contains(Vector2{0.f, 41.f}));
        REQUIRE(ContainmentType::Contains == circle.Contains(Vector2{-41.f, 0.f}));
        REQUIRE(ContainmentType::Contains == circle.Contains(Vector2{0.f, -41.f}));

        REQUIRE(ContainmentType::Intersects == circle.Contains(Vector2{42.f, 0.f}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(Vector2{0.f, 42.f}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(Vector2{-42.f, 0.f}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(Vector2{0.f, -42.f}));

        REQUIRE(ContainmentType::Disjoint == circle.Contains(Vector2{43.f, 0.f}));
        REQUIRE(ContainmentType::Disjoint == circle.Contains(Vector2{0.f, 43.f}));
        REQUIRE(ContainmentType::Disjoint == circle.Contains(Vector2{-43.f, 0.f}));
        REQUIRE(ContainmentType::Disjoint == circle.Contains(Vector2{0.f, -43.f}));
    }
    SECTION("Contains_BoundingCircle")
    {
        BoundingCircle circle;
        circle.Center = Vector2::Zero;
        circle.Radius = 42.0f;

        REQUIRE(ContainmentType::Intersects == circle.Contains(circle));
        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingCircle{Vector2::Zero, 43.0f}));
        REQUIRE(ContainmentType::Contains == circle.Contains(BoundingCircle{Vector2::Zero, 41.0f}));

        REQUIRE(ContainmentType::Contains == circle.Contains(BoundingCircle{Vector2{40.f, 0.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == circle.Contains(BoundingCircle{Vector2{0.f, 40.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == circle.Contains(BoundingCircle{Vector2{-40.f, 0.f}, 1.9f}));
        REQUIRE(ContainmentType::Contains == circle.Contains(BoundingCircle{Vector2{0.f, -40.f}, 1.9f}));

        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingCircle{Vector2{42.f, 0.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingCircle{Vector2{0.f, 42.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingCircle{Vector2{-42.f, 0.f}, 1.0f}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingCircle{Vector2{0.f, -42.f}, 1.0f}));

        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingCircle{Vector2{43.f, 0.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingCircle{Vector2{0.f, 43.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingCircle{Vector2{-43.f, 0.f}, 2.0f}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingCircle{Vector2{0.f, -43.f}, 2.0f}));

        REQUIRE(ContainmentType::Disjoint == circle.Contains(BoundingCircle{Vector2{43.f, 0.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == circle.Contains(BoundingCircle{Vector2{0.f, 43.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == circle.Contains(BoundingCircle{Vector2{-43.f, 0.f}, 0.5f}));
        REQUIRE(ContainmentType::Disjoint == circle.Contains(BoundingCircle{Vector2{0.f, -43.f}, 0.5f}));
    }
    SECTION("Contains_BoundingBox2D")
    {
        BoundingCircle circle;
        circle.Center = Vector2::Zero;
        circle.Radius = 42.0f;

        auto min = Vector2::Normalize({-1.f, -1.f}) * 42.0f;
        auto max = Vector2::Normalize({1.f, 1.f}) * 42.0f;
        auto unit = Vector2::Normalize({1.f, 1.f}) * 42.0f;

        REQUIRE(ContainmentType::Contains == circle.Contains(BoundingBox2D{Vector2::Zero, max}));
        REQUIRE(ContainmentType::Contains == circle.Contains(BoundingBox2D{Vector2::Zero - unit * Vector2{1, 0}, max - unit * Vector2{1, 0}}));
        REQUIRE(ContainmentType::Contains == circle.Contains(BoundingBox2D{Vector2::Zero - unit * Vector2{0, 1}, max - unit * Vector2{0, 1}}));
        REQUIRE(ContainmentType::Contains == circle.Contains(BoundingBox2D{min, Vector2::Zero}));
        REQUIRE(ContainmentType::Contains == circle.Contains(BoundingBox2D{min + unit * Vector2{1, 0}, Vector2::Zero + unit * Vector2{1, 0}}));
        REQUIRE(ContainmentType::Contains == circle.Contains(BoundingBox2D{min + unit * Vector2{0, 1}, Vector2::Zero + unit * Vector2{0, 1}}));

        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingBox2D{Vector2::Zero, max * 1.01f}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingBox2D{Vector2::Zero - unit * Vector2{1, 0}, max * 1.01f - unit * Vector2{1, 0}}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingBox2D{Vector2::Zero - unit * Vector2{0, 1}, max * 1.01f - unit * Vector2{0, 1}}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingBox2D{min * 1.01f, Vector2::Zero}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingBox2D{min * 1.01f + unit * Vector2{1, 0}, Vector2::Zero + unit * Vector2{1, 0}}));
        REQUIRE(ContainmentType::Intersects == circle.Contains(BoundingBox2D{min * 1.01f + unit * Vector2{0, 1}, Vector2::Zero + unit * Vector2{0, 1}}));

        REQUIRE(ContainmentType::Disjoint == circle.Contains(BoundingBox2D{max * 1.01f, max * 1.01f + unit}));
    }
    SECTION("Intersects_BoundingBox2D")
    {
        BoundingCircle circle;
        circle.Center = Vector2::Zero;
        circle.Radius = 42.0f;

        auto min = Vector2::Normalize({-1.f, -1.f}) * 42.0f;
        auto max = Vector2::Normalize({1.f, 1.f}) * 42.0f;
        auto unit = Vector2::Normalize({1.f, 1.f}) * 42.0f;

        REQUIRE(circle.Intersects(BoundingBox2D{Vector2::Zero, max}));
        REQUIRE(circle.Intersects(BoundingBox2D{Vector2::Zero - unit * Vector2{1, 0}, max - unit * Vector2{1, 0}}));
        REQUIRE(circle.Intersects(BoundingBox2D{Vector2::Zero - unit * Vector2{0, 1}, max - unit * Vector2{0, 1}}));
        REQUIRE(circle.Intersects(BoundingBox2D{min, Vector2::Zero}));
        REQUIRE(circle.Intersects(BoundingBox2D{min + unit * Vector2{1, 0}, Vector2::Zero + unit * Vector2{1, 0}}));
        REQUIRE(circle.Intersects(BoundingBox2D{min + unit * Vector2{0, 1}, Vector2::Zero + unit * Vector2{0, 1}}));

        REQUIRE(circle.Intersects(BoundingBox2D{Vector2::Zero, max * 1.01f}));
        REQUIRE(circle.Intersects(BoundingBox2D{Vector2::Zero - unit * Vector2{1, 0}, max * 1.01f - unit * Vector2{1, 0}}));
        REQUIRE(circle.Intersects(BoundingBox2D{Vector2::Zero - unit * Vector2{0, 1}, max * 1.01f - unit * Vector2{0, 1}}));
        REQUIRE(circle.Intersects(BoundingBox2D{min * 1.01f, Vector2::Zero}));
        REQUIRE(circle.Intersects(BoundingBox2D{min * 1.01f + unit * Vector2{1, 0}, Vector2::Zero + unit * Vector2{1, 0}}));
        REQUIRE(circle.Intersects(BoundingBox2D{min * 1.01f + unit * Vector2{0, 1}, Vector2::Zero + unit * Vector2{0, 1}}));

        REQUIRE_FALSE(circle.Intersects(BoundingBox2D{max * 1.01f, max * 1.01f + unit}));
    }
    SECTION("Intersects_BoundingCircle")
    {
        BoundingCircle circle;
        circle.Center = Vector2::Zero;
        circle.Radius = 42.0f;

        REQUIRE(circle.Intersects(circle));
        REQUIRE(circle.Intersects(BoundingCircle{Vector2::Zero, 43.0f}));
        REQUIRE(circle.Intersects(BoundingCircle{Vector2::Zero, 41.0f}));

        REQUIRE(circle.Intersects(BoundingCircle{Vector2{40.f, 0.f}, 1.9f}));
        REQUIRE(circle.Intersects(BoundingCircle{Vector2{0.f, 40.f}, 1.9f}));
        REQUIRE(circle.Intersects(BoundingCircle{Vector2{-40.f, 0.f}, 1.9f}));
        REQUIRE(circle.Intersects(BoundingCircle{Vector2{0.f, -40.f}, 1.9f}));

        REQUIRE(circle.Intersects(BoundingCircle{Vector2{42.f, 0.f}, 1.0f}));
        REQUIRE(circle.Intersects(BoundingCircle{Vector2{0.f, 42.f}, 1.0f}));
        REQUIRE(circle.Intersects(BoundingCircle{Vector2{-42.f, 0.f}, 1.0f}));
        REQUIRE(circle.Intersects(BoundingCircle{Vector2{0.f, -42.f}, 1.0f}));

        REQUIRE(circle.Intersects(BoundingCircle{Vector2{43.f, 0.f}, 2.0f}));
        REQUIRE(circle.Intersects(BoundingCircle{Vector2{0.f, 43.f}, 2.0f}));
        REQUIRE(circle.Intersects(BoundingCircle{Vector2{-43.f, 0.f}, 2.0f}));
        REQUIRE(circle.Intersects(BoundingCircle{Vector2{0.f, -43.f}, 2.0f}));

        REQUIRE_FALSE(circle.Intersects(BoundingCircle{Vector2{43.f, 0.f}, 0.5f}));
        REQUIRE_FALSE(circle.Intersects(BoundingCircle{Vector2{0.f, 43.f}, 0.5f}));
        REQUIRE_FALSE(circle.Intersects(BoundingCircle{Vector2{-43.f, 0.f}, 0.5f}));
        REQUIRE_FALSE(circle.Intersects(BoundingCircle{Vector2{0.f, -43.f}, 0.5f}));
    }
}
