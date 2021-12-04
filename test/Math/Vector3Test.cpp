// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "catch_amalgamated.hpp"

using Pomdog::Matrix4x4;
using Pomdog::Vector3;

TEST_CASE("Vector3", "[Vector3]")
{
    SECTION("Vector3")
    {
        Vector3 vec{0, 0, 0};
        REQUIRE(vec.X == 0.0f);
        REQUIRE(vec.Y == 0.0f);
        REQUIRE(vec.Z == 0.0f);

        vec = {1, 2, 3};
        REQUIRE(vec.X == 1.0f);
        REQUIRE(vec.Y == 2.0f);
        REQUIRE(vec.Z == 3.0f);
    }
    SECTION("Constants")
    {
        REQUIRE(Vector3{1.0f, 0.0f, 0.0f} == Vector3::UnitX);
        REQUIRE(Vector3{0.0f, 1.0f, 0.0f} == Vector3::UnitY);
        REQUIRE(Vector3{0.0f, 0.0f, 1.0f} == Vector3::UnitZ);
        REQUIRE(Vector3{0.0f, 0.0f, 0.0f} == Vector3::Zero);
    }
    SECTION("Addition")
    {
        REQUIRE(Vector3{1.0f + 4.0f, 2.0f + 5.0f, 3.0f + 6.0f} == Vector3{1.0f, 2.0f, 3.0f} + Vector3{4.0f, 5.0f, 6.0f});
        REQUIRE(Vector3{1.0f - 4.0f, 2.0f - 5.0f, 3.0f - 6.0f} == Vector3{1.0f, 2.0f, 3.0f} + Vector3{-4, -5, -6});
    }
    SECTION("Subtraction")
    {
        REQUIRE(Vector3{1.0f - 4.0f, 2.0f - 5.0f, 3.0f - 6.0f} == Vector3{1.0f, 2.0f, 3.0f} - Vector3{4.0f, 5.0f, 6.0f});
        REQUIRE(Vector3{4.0f - 1.0f, 5.0f - 2.0f, 6.0f - 3.0f} == Vector3{4.0f, 5.0f, 6.0f} - Vector3{1.0f, 2.0f, 3.0f});
    }
    SECTION("Multiply")
    {
        REQUIRE(Vector3{4.0f * 3.0f, 5.0f * 3.0f, 6.0f * 3.0f} == Vector3{4.0f, 5.0f, 6.0f} * Vector3{3.0f, 3.0f, 3.0f});
        REQUIRE(Vector3{4.0f * 3.0f, 5.0f * 3.0f, 6.0f * 3.0f} == Vector3{3.0f, 3.0f, 3.0f} * Vector3{4.0f, 5.0f, 6.0f});
        REQUIRE(Vector3{4.0f * 3.0f, 5.0f * 3.0f, 6.0f * 3.0f} == Vector3{4.0f, 5.0f, 6.0f} * 3.0f);
        REQUIRE(Vector3{4.0f * 3.0f, 5.0f * 3.0f, 6.0f * 3.0f} == 3.0f * Vector3{4.0f, 5.0f, 6.0f});
    }
    SECTION("Division")
    {
        REQUIRE(Vector3{4.0f / 3.0f, 5.0f / 3.0f, 6.0f / 3.0f} == Vector3{4.0f, 5.0f, 6.0f} / Vector3{3.0f, 3.0f, 3.0f});
        REQUIRE(Vector3{4.0f / 3.0f, 5.0f / 3.0f, 6.0f / 3.0f} == Vector3{4.0f, 5.0f, 6.0f} / 3.0f);
        REQUIRE(Vector3{3.0f / 4.0f, 3.0f / 5.0f, 3.0f / 6.0f} == Vector3{3.0f, 3.0f, 3.0f} / Vector3{4.0f, 5.0f, 6.0f});
    }
    SECTION("Lerp")
    {
        REQUIRE(Vector3{0.0f, 0.0f, 0.0f} == Vector3::Lerp(Vector3::Zero, Vector3{40.0f, 50.0f, 60.0f}, 0.0f));
        REQUIRE(Vector3{4.0f, 5.0f, 6.0f} == Vector3::Lerp(Vector3::Zero, Vector3{40.0f, 50.0f, 60.0f}, 0.1f));
        REQUIRE(Vector3{36.f, 45.f, 54.f} == Vector3::Lerp(Vector3::Zero, Vector3{40.0f, 50.0f, 60.0f}, 0.9f));
        REQUIRE(Vector3{40.f, 50.f, 60.f} == Vector3::Lerp(Vector3::Zero, Vector3{40.0f, 50.0f, 60.0f}, 1.0f));

        REQUIRE(Vector3{1.0f, 2.0f, 3.0f} == Vector3::Lerp(Vector3{1.0f, 2.0f, 3.0f}, Vector3{4.0f, 5.0f, 6.0f}, 0.0f));
        REQUIRE(Vector3{2.5f, 3.5f, 4.5f} == Vector3::Lerp(Vector3{1.0f, 2.0f, 3.0f}, Vector3{4.0f, 5.0f, 6.0f}, 0.5f));
        REQUIRE(Vector3{4.0f, 5.0f, 6.0f} == Vector3::Lerp(Vector3{1.0f, 2.0f, 3.0f}, Vector3{4.0f, 5.0f, 6.0f}, 1.0f));
    }
    SECTION("Normalize")
    {
        REQUIRE(Vector3::Normalize(Vector3{0.0f, 0.0f, 0.0f}) == Vector3{0.0f, 0.0f, 0.0f});
        REQUIRE(Vector3::Normalize(Vector3{1.0f, 0.0f, 0.0f}) == Vector3{1.0f, 0.0f, 0.0f});
        REQUIRE(Vector3::Normalize(Vector3{0.0f, 1.0f, 0.0f}) == Vector3{0.0f, 1.0f, 0.0f});
        REQUIRE(Vector3::Normalize(Vector3{0.0f, 0.0f, 1.0f}) == Vector3{0.0f, 0.0f, 1.0f});
    }
    SECTION("Cross_AssociativeLow")
    {
        // NOTE: Associative low of vectors
        const Vector3 p = Vector3::UnitX;
        const Vector3 q = Vector3::UnitY;
        const Vector3 r = Vector3::UnitZ;

        REQUIRE(3.0f * Vector3::Cross(Vector3::Zero, p) == Vector3::Cross(Vector3::Zero, 3.0f * p));
        REQUIRE(-7.0f * Vector3::Cross(Vector3::Zero, q) == Vector3::Cross(Vector3::Zero, -7.0f * q));
        REQUIRE(42.0f * Vector3::Cross(Vector3::Zero, r) == Vector3::Cross(Vector3::Zero, 42.0f * r));

        const Vector3 a = {42.0f, 7.0f, 13.0f};
        const Vector3 b = {5.0f, -8.0f, 9.0f};
        const Vector3 c = {-81.0f, -63.0f, -2.0f};

        REQUIRE(5.0f * Vector3::Cross(a, b) == Vector3::Cross(5.0f * a, b));
        REQUIRE(5.0f * Vector3::Cross(a, b) == Vector3::Cross(a, 5.0f * b));
        REQUIRE(-14.0f * Vector3::Cross(b, c) == Vector3::Cross(-14.0f * b, c));
        REQUIRE(-14.0f * Vector3::Cross(b, c) == Vector3::Cross(b, -14.0f * c));
        REQUIRE(-72.0f * Vector3::Cross(c, a) == Vector3::Cross(-72.0f * c, a));
        REQUIRE(-72.0f * Vector3::Cross(c, a) == Vector3::Cross(c, -72.0f * a));
    }
    SECTION("Cross_CommutativeProperty")
    {
        // NOTE: Commutative property of vectors
        REQUIRE(Vector3::Zero == Vector3::Cross(Vector3::Zero, Vector3::Zero));

        REQUIRE(-Vector3::Cross(Vector3::UnitX, Vector3::Zero) == Vector3::Cross(Vector3::Zero, Vector3::UnitX));
        REQUIRE(-Vector3::Cross(Vector3::UnitY, Vector3::Zero) == Vector3::Cross(Vector3::Zero, Vector3::UnitY));
        REQUIRE(-Vector3::Cross(Vector3::UnitZ, Vector3::Zero) == Vector3::Cross(Vector3::Zero, Vector3::UnitZ));

        const Vector3 a = {42.0f, 7.0f, 13.0f};
        const Vector3 b = {5.0f, -8.0f, 9.0f};
        const Vector3 c = {-81.0f, -63.0f, -2.0f};

        REQUIRE(-Vector3::Cross(a, b) == Vector3::Cross(b, a));
        REQUIRE(-Vector3::Cross(a, c) == Vector3::Cross(c, a));
        REQUIRE(-Vector3::Cross(b, c) == Vector3::Cross(c, b));
    }
    SECTION("Transform_IdentityMatrix")
    {
        REQUIRE(Vector3{0.0f, 0.0f, 0.0f} == Vector3::Transform(Vector3{0.0f, 0.0f, 0.0f}, Matrix4x4::Identity));
        REQUIRE(Vector3{41.0f, 42.0f, 43.0f} == Vector3::Transform(Vector3{41.0f, 42.0f, 43.0f}, Matrix4x4::Identity));
    }
}
