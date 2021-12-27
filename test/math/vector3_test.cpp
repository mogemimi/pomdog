// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/vector3.h"
#include <catch_amalgamated.hpp>

using pomdog::Matrix4x4;
using pomdog::Vector3;

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
        REQUIRE(Vector3{1.0f, 0.0f, 0.0f} == Vector3::UnitX());
        REQUIRE(Vector3{0.0f, 1.0f, 0.0f} == Vector3::UnitY());
        REQUIRE(Vector3{0.0f, 0.0f, 1.0f} == Vector3::UnitZ());
        REQUIRE(Vector3{0.0f, 0.0f, 0.0f} == Vector3::Zero());
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
        using pomdog::math::Lerp;
        REQUIRE(Vector3{0.0f, 0.0f, 0.0f} == Lerp(Vector3::Zero(), Vector3{40.0f, 50.0f, 60.0f}, 0.0f));
        REQUIRE(Vector3{4.0f, 5.0f, 6.0f} == Lerp(Vector3::Zero(), Vector3{40.0f, 50.0f, 60.0f}, 0.1f));
        REQUIRE(Vector3{36.f, 45.f, 54.f} == Lerp(Vector3::Zero(), Vector3{40.0f, 50.0f, 60.0f}, 0.9f));
        REQUIRE(Vector3{40.f, 50.f, 60.f} == Lerp(Vector3::Zero(), Vector3{40.0f, 50.0f, 60.0f}, 1.0f));

        REQUIRE(Vector3{1.0f, 2.0f, 3.0f} == Lerp(Vector3{1.0f, 2.0f, 3.0f}, Vector3{4.0f, 5.0f, 6.0f}, 0.0f));
        REQUIRE(Vector3{2.5f, 3.5f, 4.5f} == Lerp(Vector3{1.0f, 2.0f, 3.0f}, Vector3{4.0f, 5.0f, 6.0f}, 0.5f));
        REQUIRE(Vector3{4.0f, 5.0f, 6.0f} == Lerp(Vector3{1.0f, 2.0f, 3.0f}, Vector3{4.0f, 5.0f, 6.0f}, 1.0f));
    }
    SECTION("Normalize")
    {
        using pomdog::math::Normalize;
        REQUIRE(Normalize(Vector3{0.0f, 0.0f, 0.0f}) == Vector3{0.0f, 0.0f, 0.0f});
        REQUIRE(Normalize(Vector3{1.0f, 0.0f, 0.0f}) == Vector3{1.0f, 0.0f, 0.0f});
        REQUIRE(Normalize(Vector3{0.0f, 1.0f, 0.0f}) == Vector3{0.0f, 1.0f, 0.0f});
        REQUIRE(Normalize(Vector3{0.0f, 0.0f, 1.0f}) == Vector3{0.0f, 0.0f, 1.0f});
    }
    SECTION("Cross_AssociativeLow")
    {
        using pomdog::math::Cross;

        // NOTE: Associative low of vectors
        const Vector3 p = Vector3::UnitX();
        const Vector3 q = Vector3::UnitY();
        const Vector3 r = Vector3::UnitZ();

        REQUIRE(3.0f * Cross(Vector3::Zero(), p) == Cross(Vector3::Zero(), 3.0f * p));
        REQUIRE(-7.0f * Cross(Vector3::Zero(), q) == Cross(Vector3::Zero(), -7.0f * q));
        REQUIRE(42.0f * Cross(Vector3::Zero(), r) == Cross(Vector3::Zero(), 42.0f * r));

        const Vector3 a = {42.0f, 7.0f, 13.0f};
        const Vector3 b = {5.0f, -8.0f, 9.0f};
        const Vector3 c = {-81.0f, -63.0f, -2.0f};

        REQUIRE(5.0f * Cross(a, b) == Cross(5.0f * a, b));
        REQUIRE(5.0f * Cross(a, b) == Cross(a, 5.0f * b));
        REQUIRE(-14.0f * Cross(b, c) == Cross(-14.0f * b, c));
        REQUIRE(-14.0f * Cross(b, c) == Cross(b, -14.0f * c));
        REQUIRE(-72.0f * Cross(c, a) == Cross(-72.0f * c, a));
        REQUIRE(-72.0f * Cross(c, a) == Cross(c, -72.0f * a));
    }
    SECTION("Cross_CommutativeProperty")
    {
        using pomdog::math::Cross;

        // NOTE: Commutative property of vectors
        REQUIRE(Vector3::Zero() == Cross(Vector3::Zero(), Vector3::Zero()));

        REQUIRE(-Cross(Vector3::UnitX(), Vector3::Zero()) == Cross(Vector3::Zero(), Vector3::UnitX()));
        REQUIRE(-Cross(Vector3::UnitY(), Vector3::Zero()) == Cross(Vector3::Zero(), Vector3::UnitY()));
        REQUIRE(-Cross(Vector3::UnitZ(), Vector3::Zero()) == Cross(Vector3::Zero(), Vector3::UnitZ()));

        const Vector3 a = {42.0f, 7.0f, 13.0f};
        const Vector3 b = {5.0f, -8.0f, 9.0f};
        const Vector3 c = {-81.0f, -63.0f, -2.0f};

        REQUIRE(-Cross(a, b) == Cross(b, a));
        REQUIRE(-Cross(a, c) == Cross(c, a));
        REQUIRE(-Cross(b, c) == Cross(c, b));
    }
    SECTION("Transform_IdentityMatrix")
    {
        using pomdog::math::Transform;
        REQUIRE(Vector3{0.0f, 0.0f, 0.0f} == Transform(Vector3{0.0f, 0.0f, 0.0f}, Matrix4x4::Identity()));
        REQUIRE(Vector3{41.0f, 42.0f, 43.0f} == Transform(Vector3{41.0f, 42.0f, 43.0f}, Matrix4x4::Identity()));
    }
}
