// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/vector4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Matrix4x4;
using pomdog::Vector4;

TEST_CASE("Vector4")
{
    SUBCASE("constructor")
    {
        Vector4 vec{0, 0, 0, 0};
        REQUIRE(vec.x == 0.0f);
        REQUIRE(vec.y == 0.0f);
        REQUIRE(vec.z == 0.0f);
        REQUIRE(vec.w == 0.0f);

        vec = {1, 2, 3, 4};
        REQUIRE(vec.x == 1.0f);
        REQUIRE(vec.y == 2.0f);
        REQUIRE(vec.z == 3.0f);
        REQUIRE(vec.w == 4.0f);
    }
    SUBCASE("addition")
    {
        REQUIRE(Vector4{1.f + 5.f, 2.f + 6.f, 3.f + 7.f, 4.f + 8.f} == Vector4{1, 2, 3, 4} + Vector4{5, 6, 7, 8});
        REQUIRE(Vector4{1.f - 5.f, 2.f - 6.f, 3.f - 7.f, 4.f - 8.f} == Vector4{1, 2, 3, 4} + Vector4{-5, -6, -7, -8});
    }
    SUBCASE("subtraction")
    {
        REQUIRE(Vector4{1.f - 5.f, 2.f - 6.f, 3.f - 7.f, 4.f - 8.f} == Vector4{1, 2, 3, 4} - Vector4{5, 6, 7, 8});
        REQUIRE(Vector4{1.f + 5.f, 2.f + 6.f, 3.f + 7.f, 4.f + 8.f} == Vector4{1, 2, 3, 4} - Vector4{-5, -6, -7, -8});
    }
    SUBCASE("multiply")
    {
        REQUIRE(Vector4{4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f} == Vector4{4, 5, 6, 7} * Vector4{3, 3, 3, 3});
        REQUIRE(Vector4{4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f} == Vector4{3, 3, 3, 3} * Vector4{4, 5, 6, 7});
        REQUIRE(Vector4{4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f} == Vector4{4, 5, 6, 7} * 3);
        REQUIRE(Vector4{4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f} == 3.0f * Vector4{4, 5, 6, 7});
    }
    SUBCASE("division")
    {
        REQUIRE(Vector4{4.f / 3.f, 5.f / 3.f, 6.f / 3.f, 7.f / 3.f} == Vector4{4, 5, 6, 7} / Vector4{3, 3, 3, 3});
        REQUIRE(Vector4{3.f / 4.f, 3.f / 5.f, 3.f / 6.f, 3.f / 7.f} == Vector4{3, 3, 3, 3} / Vector4{4, 5, 6, 7});
        REQUIRE(Vector4{4.f / 3.f, 5.f / 3.f, 6.f / 3.f, 7.f / 3.f} == Vector4{4, 5, 6, 7} / 3.0f);
    }
    SUBCASE("normalize")
    {
        using pomdog::math::normalize;
        REQUIRE(normalize(Vector4{0.0f, 0.0f, 0.0f, 0.0f}) == Vector4{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(normalize(Vector4{1.0f, 0.0f, 0.0f, 0.0f}) == Vector4{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(normalize(Vector4{0.0f, 1.0f, 0.0f, 0.0f}) == Vector4{0.0f, 1.0f, 0.0f, 0.0f});
        REQUIRE(normalize(Vector4{0.0f, 0.0f, 1.0f, 0.0f}) == Vector4{0.0f, 0.0f, 1.0f, 0.0f});
        REQUIRE(normalize(Vector4{0.0f, 0.0f, 0.0f, 1.0f}) == Vector4{0.0f, 0.0f, 0.0f, 1.0f});
    }
    SUBCASE("transform identity matrix")
    {
        using pomdog::math::transform;
        REQUIRE(Vector4{0.0f, 0.0f, 0.0f, 0.0f} == transform(Vector4{0.0f, 0.0f, 0.0f, 0.0f}, Matrix4x4::createIdentity()));
        REQUIRE(Vector4{1.0f, 2.0f, 3.0f, 4.0f} == transform(Vector4{1.0f, 2.0f, 3.0f, 4.0f}, Matrix4x4::createIdentity()));
    }
    SUBCASE("transform")
    {
        using pomdog::math::transform;
        REQUIRE(Vector4{26.0f, 37.0f, 48.0f, 59.0f} ==
                transform(
                    Vector4{26.0f, 37.0f, 48.0f, 59.0f},
                    Matrix4x4{
                        1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f}));

        REQUIRE(Vector4{52.0f, 74.0f, 96.0f, 59.0f} ==
                transform(
                    Vector4{26.0f, 37.0f, 48.0f, 59.0f},
                    Matrix4x4{
                        2.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 2.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 2.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f}));

        REQUIRE(Vector4{52.0f, 74.0f, 96.0f, 4588.0f} ==
                transform(
                    Vector4{26.0f, 37.0f, 48.0f, 59.0f},
                    Matrix4x4{
                        2.0f, 0.0f, 0.0f, 42.0f,
                        0.0f, 2.0f, 0.0f, 41.0f,
                        0.0f, 0.0f, 2.0f, 40.0f,
                        0.0f, 0.0f, 0.0f, 1.0f}));

        REQUIRE(Vector4{388.0f, 74.0f, 304.0f, 4588.0f} ==
                transform(
                    Vector4{26.0f, 37.0f, 48.0f, 59.0f},
                    Matrix4x4{
                        2.0f, 0.0f, 8.0f, 42.0f,
                        0.0f, 2.0f, 0.0f, 41.0f,
                        7.0f, 0.0f, 2.0f, 40.0f,
                        0.0f, 0.0f, 0.0f, 1.0f}));
    }
}
