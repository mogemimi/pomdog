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
        REQUIRE(vec.x == 0.0f);
        REQUIRE(vec.y == 0.0f);
        REQUIRE(vec.z == 0.0f);

        vec = {1, 2, 3};
        REQUIRE(vec.x == 1.0f);
        REQUIRE(vec.y == 2.0f);
        REQUIRE(vec.z == 3.0f);
    }
    SECTION("Constants")
    {
        REQUIRE(Vector3{1.0f, 0.0f, 0.0f} == Vector3::createUnitX());
        REQUIRE(Vector3{0.0f, 1.0f, 0.0f} == Vector3::createUnitY());
        REQUIRE(Vector3{0.0f, 0.0f, 1.0f} == Vector3::createUnitZ());
        REQUIRE(Vector3{0.0f, 0.0f, 0.0f} == Vector3::createZero());
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
        using pomdog::math::lerp;
        REQUIRE(Vector3{0.0f, 0.0f, 0.0f} == lerp(Vector3::createZero(), Vector3{40.0f, 50.0f, 60.0f}, 0.0f));
        REQUIRE(Vector3{4.0f, 5.0f, 6.0f} == lerp(Vector3::createZero(), Vector3{40.0f, 50.0f, 60.0f}, 0.1f));
        REQUIRE(Vector3{36.f, 45.f, 54.f} == lerp(Vector3::createZero(), Vector3{40.0f, 50.0f, 60.0f}, 0.9f));
        REQUIRE(Vector3{40.f, 50.f, 60.f} == lerp(Vector3::createZero(), Vector3{40.0f, 50.0f, 60.0f}, 1.0f));

        REQUIRE(Vector3{1.0f, 2.0f, 3.0f} == lerp(Vector3{1.0f, 2.0f, 3.0f}, Vector3{4.0f, 5.0f, 6.0f}, 0.0f));
        REQUIRE(Vector3{2.5f, 3.5f, 4.5f} == lerp(Vector3{1.0f, 2.0f, 3.0f}, Vector3{4.0f, 5.0f, 6.0f}, 0.5f));
        REQUIRE(Vector3{4.0f, 5.0f, 6.0f} == lerp(Vector3{1.0f, 2.0f, 3.0f}, Vector3{4.0f, 5.0f, 6.0f}, 1.0f));
    }
    SECTION("Normalize")
    {
        using pomdog::math::normalize;
        REQUIRE(normalize(Vector3{0.0f, 0.0f, 0.0f}) == Vector3{0.0f, 0.0f, 0.0f});
        REQUIRE(normalize(Vector3{1.0f, 0.0f, 0.0f}) == Vector3{1.0f, 0.0f, 0.0f});
        REQUIRE(normalize(Vector3{0.0f, 1.0f, 0.0f}) == Vector3{0.0f, 1.0f, 0.0f});
        REQUIRE(normalize(Vector3{0.0f, 0.0f, 1.0f}) == Vector3{0.0f, 0.0f, 1.0f});
    }
    SECTION("Cross_AssociativeLow")
    {
        using pomdog::math::cross;

        // NOTE: Associative low of vectors
        const Vector3 p = Vector3::createUnitX();
        const Vector3 q = Vector3::createUnitY();
        const Vector3 r = Vector3::createUnitZ();

        REQUIRE(3.0f * cross(Vector3::createZero(), p) == cross(Vector3::createZero(), 3.0f * p));
        REQUIRE(-7.0f * cross(Vector3::createZero(), q) == cross(Vector3::createZero(), -7.0f * q));
        REQUIRE(42.0f * cross(Vector3::createZero(), r) == cross(Vector3::createZero(), 42.0f * r));

        const Vector3 a = {42.0f, 7.0f, 13.0f};
        const Vector3 b = {5.0f, -8.0f, 9.0f};
        const Vector3 c = {-81.0f, -63.0f, -2.0f};

        REQUIRE(5.0f * cross(a, b) == cross(5.0f * a, b));
        REQUIRE(5.0f * cross(a, b) == cross(a, 5.0f * b));
        REQUIRE(-14.0f * cross(b, c) == cross(-14.0f * b, c));
        REQUIRE(-14.0f * cross(b, c) == cross(b, -14.0f * c));
        REQUIRE(-72.0f * cross(c, a) == cross(-72.0f * c, a));
        REQUIRE(-72.0f * cross(c, a) == cross(c, -72.0f * a));
    }
    SECTION("Cross_CommutativeProperty")
    {
        using pomdog::math::cross;

        // NOTE: Commutative property of vectors
        REQUIRE(Vector3::createZero() == cross(Vector3::createZero(), Vector3::createZero()));

        REQUIRE(-cross(Vector3::createUnitX(), Vector3::createZero()) == cross(Vector3::createZero(), Vector3::createUnitX()));
        REQUIRE(-cross(Vector3::createUnitY(), Vector3::createZero()) == cross(Vector3::createZero(), Vector3::createUnitY()));
        REQUIRE(-cross(Vector3::createUnitZ(), Vector3::createZero()) == cross(Vector3::createZero(), Vector3::createUnitZ()));

        const Vector3 a = {42.0f, 7.0f, 13.0f};
        const Vector3 b = {5.0f, -8.0f, 9.0f};
        const Vector3 c = {-81.0f, -63.0f, -2.0f};

        REQUIRE(-cross(a, b) == cross(b, a));
        REQUIRE(-cross(a, c) == cross(c, a));
        REQUIRE(-cross(b, c) == cross(c, b));
    }
    SECTION("Transform_IdentityMatrix")
    {
        using pomdog::math::transform;
        REQUIRE(Vector3{0.0f, 0.0f, 0.0f} == transform(Vector3{0.0f, 0.0f, 0.0f}, Matrix4x4::createIdentity()));
        REQUIRE(Vector3{41.0f, 42.0f, 43.0f} == transform(Vector3{41.0f, 42.0f, 43.0f}, Matrix4x4::createIdentity()));
    }
}
