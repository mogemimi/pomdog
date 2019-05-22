// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "catch.hpp"

using Pomdog::Matrix4x4;
using Pomdog::Vector4;

TEST_CASE("Vector4", "[Vector4]")
{
    SECTION("Vector4")
    {
        Vector4 vec {0, 0, 0, 0};
        REQUIRE(vec.X == 0.0f);
        REQUIRE(vec.Y == 0.0f);
        REQUIRE(vec.Z == 0.0f);
        REQUIRE(vec.W == 0.0f);

        vec = {1, 2, 3, 4};
        REQUIRE(vec.X == 1.0f);
        REQUIRE(vec.Y == 2.0f);
        REQUIRE(vec.Z == 3.0f);
        REQUIRE(vec.W == 4.0f);
    }
    SECTION("Addition")
    {
        REQUIRE(Vector4{1.f + 5.f, 2.f + 6.f, 3.f + 7.f, 4.f + 8.f} == Vector4{1, 2, 3, 4} + Vector4{5, 6, 7, 8});
        REQUIRE(Vector4{1.f - 5.f, 2.f - 6.f, 3.f - 7.f, 4.f - 8.f} == Vector4{1, 2, 3, 4} + Vector4{-5, -6, -7, -8});
    }
    SECTION("Subtraction")
    {
        REQUIRE(Vector4{1.f - 5.f, 2.f - 6.f, 3.f - 7.f, 4.f - 8.f} == Vector4{1, 2, 3, 4} - Vector4{5, 6, 7, 8});
        REQUIRE(Vector4{1.f + 5.f, 2.f + 6.f, 3.f + 7.f, 4.f + 8.f} == Vector4{1, 2, 3, 4} - Vector4{-5, -6, -7, -8});
    }
    SECTION("Multiply")
    {
        REQUIRE(Vector4{4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f} == Vector4{4, 5, 6, 7} * Vector4{3, 3, 3, 3});
        REQUIRE(Vector4{4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f} == Vector4{3, 3, 3, 3} * Vector4{4, 5, 6, 7});
        REQUIRE(Vector4{4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f} == Vector4{4, 5, 6, 7} * 3);
        REQUIRE(Vector4{4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f} == 3.0f * Vector4{4, 5, 6, 7});
    }
    SECTION("Division")
    {
        REQUIRE(Vector4{4.f / 3.f, 5.f / 3.f, 6.f / 3.f, 7.f / 3.f} == Vector4{4, 5, 6, 7} / Vector4{3, 3, 3, 3});
        REQUIRE(Vector4{3.f / 4.f, 3.f / 5.f, 3.f / 6.f, 3.f / 7.f} == Vector4{3, 3, 3, 3} / Vector4{4, 5, 6, 7});
        REQUIRE(Vector4{4.f / 3.f, 5.f / 3.f, 6.f / 3.f, 7.f / 3.f} == Vector4{4, 5, 6, 7} / 3.0f);
    }
    SECTION("Transform_IdentityMatrix")
    {
        REQUIRE(Vector4{0.0f, 0.0f, 0.0f, 0.0f} == Vector4::Transform(Vector4{0.0f, 0.0f, 0.0f, 0.0f}, Matrix4x4::Identity));
        REQUIRE(Vector4{1.0f, 2.0f, 3.0f, 4.0f} == Vector4::Transform(Vector4{1.0f, 2.0f, 3.0f, 4.0f}, Matrix4x4::Identity));
    }
    SECTION("Transform")
    {
        REQUIRE(Vector4{26.0f, 37.0f, 48.0f, 59.0f} == Vector4::Transform(Vector4{26.0f, 37.0f, 48.0f, 59.0f}, Matrix4x4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f)));

        REQUIRE(Vector4{52.0f, 74.0f, 96.0f, 59.0f} == Vector4::Transform(Vector4{26.0f, 37.0f, 48.0f, 59.0f}, Matrix4x4(
            2.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 2.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f)));

        REQUIRE(Vector4{52.0f, 74.0f, 96.0f, 4588.0f} == Vector4::Transform(Vector4{26.0f, 37.0f, 48.0f, 59.0f}, Matrix4x4(
            2.0f, 0.0f, 0.0f, 42.0f,
            0.0f, 2.0f, 0.0f, 41.0f,
            0.0f, 0.0f, 2.0f, 40.0f,
            0.0f, 0.0f, 0.0f, 1.0f)));

        REQUIRE(Vector4{388.0f, 74.0f, 304.0f, 4588.0f} == Vector4::Transform(Vector4{26.0f, 37.0f, 48.0f, 59.0f}, Matrix4x4(
            2.0f, 0.0f, 8.0f, 42.0f,
            0.0f, 2.0f, 0.0f, 41.0f,
            7.0f, 0.0f, 2.0f, 40.0f,
            0.0f, 0.0f, 0.0f, 1.0f)));
    }
}
