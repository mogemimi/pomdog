// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Math.hpp"
#include "catch_amalgamated.hpp"

using Pomdog::Vector2;

TEST_CASE("Vector2", "[Vector2]")
{
    SECTION("Vector2")
    {
        Vector2 vec{0, 0};
        REQUIRE(vec.X == 0.0f);
        REQUIRE(vec.Y == 0.0f);

        vec = {1, 2};
        REQUIRE(vec.X == 1.0f);
        REQUIRE(vec.Y == 2.0f);
    }
    SECTION("Constants")
    {
        REQUIRE(Vector2{0.0f, 0.0f} == Vector2::Zero);
    }
    SECTION("Addition")
    {
        REQUIRE(Vector2{5, 7} == Vector2{2, 3} + Vector2{3, 4});
    }
    SECTION("Subtraction")
    {
        REQUIRE(Vector2{-4, -5} == Vector2{2, 3} - Vector2{6, 8});
        REQUIRE(Vector2{+4, +5} == Vector2{6, 8} - Vector2{2, 3});
    }
    SECTION("Multiply")
    {
        Vector2 const result(4.0f * 3.0f, 7.0f * 3.0f);

        REQUIRE(result == Vector2{4, 7} * 3);
        REQUIRE(result == Vector2{4, 7} * Vector2{3, 3});
        REQUIRE(result == Vector2{3, 3} * Vector2{4, 7});
        REQUIRE(result == 3.0f * Vector2{4, 7});
    }
    SECTION("Division")
    {
        REQUIRE(Vector2{10.0f / 2, 8.0f / 2} == Vector2{10, 8} / 2);
        REQUIRE(Vector2{10.0f / 2, 8.0f / 2} == Vector2{10, 8} / Vector2{2, 2});
    }
    SECTION("Lerp")
    {
        REQUIRE(Vector2{0.0f, 0.0f} == Vector2::Lerp(Vector2::Zero, Vector2{40.0f, 50.0f}, 0.0f));
        REQUIRE(Vector2{4.0f, 5.0f} == Vector2::Lerp(Vector2::Zero, Vector2{40.0f, 50.0f}, 0.1f));
        REQUIRE(Vector2{36.f, 45.f} == Vector2::Lerp(Vector2::Zero, Vector2{40.0f, 50.0f}, 0.9f));
        REQUIRE(Vector2{40.f, 50.f} == Vector2::Lerp(Vector2::Zero, Vector2{40.0f, 50.0f}, 1.0f));

        REQUIRE(Vector2{1.0f, 2.0f} == Vector2::Lerp(Vector2{1.0f, 2.0f}, Vector2{4.0f, 5.0f}, 0.0f));
        REQUIRE(Vector2{2.5f, 3.5f} == Vector2::Lerp(Vector2{1.0f, 2.0f}, Vector2{4.0f, 5.0f}, 0.5f));
        REQUIRE(Vector2{4.0f, 5.0f} == Vector2::Lerp(Vector2{1.0f, 2.0f}, Vector2{4.0f, 5.0f}, 1.0f));
    }
    SECTION("Normalize")
    {
        REQUIRE(Vector2::Normalize(Vector2{0.0f, 0.0f}) == Vector2{0.0f, 0.0f});
        REQUIRE(Vector2::Normalize(Vector2{1.0f, 0.0f}) == Vector2{1.0f, 0.0f});
        REQUIRE(Vector2::Normalize(Vector2{0.0f, 1.0f}) == Vector2{0.0f, 1.0f});
    }
    SECTION("Rotate")
    {
        REQUIRE(Vector2::Rotate(Vector2{0.0f, 0.0f}, 0.0f) == Vector2{0.0f, 0.0f});

        const auto approx = [](float x) -> auto { return Catch::Approx(x).margin(0.000001f); };
        {
            auto vec = Vector2::Rotate(Vector2{1.0f, 0.0f}, 0.0f);
            REQUIRE(vec.X == approx(1.0f));
            REQUIRE(vec.Y == approx(0.0f));
        }
        {
            auto vec = Vector2::Rotate(Vector2{1.0f, 0.0f}, Pomdog::Math::PiOver2<float>);
            REQUIRE(vec.X == approx(0.0f));
            REQUIRE(vec.Y == approx(1.0f));
        }
        {
            auto vec = Vector2::Rotate(Vector2{1.0f, 0.0f}, Pomdog::Math::Pi<float>);
            REQUIRE(vec.X == approx(-1.0f));
            REQUIRE(vec.Y == approx(0.0f));
        }
        {
            auto vec = Vector2::Rotate(Vector2{1.0f, 0.0f}, -Pomdog::Math::PiOver2<float>);
            REQUIRE(vec.X == approx(0.0f));
            REQUIRE(vec.Y == approx(-1.0f));
        }
        {
            auto vec = Vector2::Rotate(Vector2{1.0f, 0.0f}, Pomdog::Math::TwoPi<float>);
            REQUIRE(vec.X == approx(1.0f));
            REQUIRE(vec.Y == approx(0.0f));
        }
    }
}
