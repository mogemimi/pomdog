// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector_xz.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Vector2;
using pomdog::VectorXZ;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("VectorXZ")
{
    SUBCASE("constructor")
    {
        VectorXZ vec{0, 0};
        REQUIRE(vec.x == 0.0f);
        REQUIRE(vec.z == 0.0f);

        vec = {1, 2};
        REQUIRE(vec.x == 1.0f);
        REQUIRE(vec.z == 2.0f);
    }
    SUBCASE("addition")
    {
        REQUIRE(VectorXZ{5, 7} == VectorXZ{2, 3} + VectorXZ{3, 4});
    }
    SUBCASE("subtraction")
    {
        REQUIRE(VectorXZ{-4, -5} == VectorXZ{2, 3} - VectorXZ{6, 8});
        REQUIRE(VectorXZ{+4, +5} == VectorXZ{6, 8} - VectorXZ{2, 3});
    }
    SUBCASE("multiply")
    {
        VectorXZ const result(4.0f * 3.0f, 7.0f * 3.0f);

        REQUIRE(result == VectorXZ{4, 7} * 3);
        REQUIRE(result == VectorXZ{4, 7} * VectorXZ{3, 3});
        REQUIRE(result == VectorXZ{3, 3} * VectorXZ{4, 7});
        REQUIRE(result == 3.0f * VectorXZ{4, 7});
    }
    SUBCASE("division")
    {
        REQUIRE(VectorXZ{10.0f / 2, 8.0f / 2} == VectorXZ{10, 8} / 2);
        REQUIRE(VectorXZ{10.0f / 2, 8.0f / 2} == VectorXZ{10, 8} / VectorXZ{2, 2});
    }
    SUBCASE("lerp")
    {
        using pomdog::math::lerp;
        REQUIRE(VectorXZ{0.0f, 0.0f} == lerp(VectorXZ{0, 0}, VectorXZ{40.0f, 50.0f}, 0.0f));
        REQUIRE(VectorXZ{4.0f, 5.0f} == lerp(VectorXZ{0, 0}, VectorXZ{40.0f, 50.0f}, 0.1f));
        REQUIRE(VectorXZ{36.f, 45.f} == lerp(VectorXZ{0, 0}, VectorXZ{40.0f, 50.0f}, 0.9f));
        REQUIRE(VectorXZ{40.f, 50.f} == lerp(VectorXZ{0, 0}, VectorXZ{40.0f, 50.0f}, 1.0f));

        REQUIRE(VectorXZ{1.0f, 2.0f} == lerp(VectorXZ{1.0f, 2.0f}, VectorXZ{4.0f, 5.0f}, 0.0f));
        REQUIRE(VectorXZ{2.5f, 3.5f} == lerp(VectorXZ{1.0f, 2.0f}, VectorXZ{4.0f, 5.0f}, 0.5f));
        REQUIRE(VectorXZ{4.0f, 5.0f} == lerp(VectorXZ{1.0f, 2.0f}, VectorXZ{4.0f, 5.0f}, 1.0f));
    }
    SUBCASE("normalize")
    {
        using pomdog::math::normalize;
        REQUIRE(normalize(VectorXZ{0.0f, 0.0f}) == VectorXZ{0.0f, 0.0f});
        REQUIRE(normalize(VectorXZ{1.0f, 0.0f}) == VectorXZ{1.0f, 0.0f});
        REQUIRE(normalize(VectorXZ{0.0f, 1.0f}) == VectorXZ{0.0f, 1.0f});
    }
    SUBCASE("rotate")
    {
        using pomdog::math::rotate;
        {
            auto vec = rotate(VectorXZ{0.0f, 0.0f}, 0.0f);
            REQUIRE(vec.x == approx(0.0f));
            REQUIRE(vec.z == approx(0.0f));
        }

        {
            auto vec = rotate(VectorXZ{1.0f, 0.0f}, 0.0f);
            REQUIRE(vec.x == approx(1.0f));
            REQUIRE(vec.z == approx(0.0f));
        }
        {
            auto vec = rotate(VectorXZ{1.0f, 0.0f}, pomdog::math::PiOver2<float>);
            REQUIRE(vec.x == approx(0.0f));
            REQUIRE(vec.z == approx(1.0f));
        }
        {
            auto vec = rotate(VectorXZ{1.0f, 0.0f}, pomdog::math::Pi<float>);
            REQUIRE(vec.x == approx(-1.0f));
            REQUIRE(vec.z == approx(0.0f));
        }
    }
    SUBCASE("toVector2")
    {
        using pomdog::math::toVector2;
        auto v = toVector2(VectorXZ{1.0f, 2.0f});
        REQUIRE(v.x == 1.0f);
        REQUIRE(v.y == 2.0f);
    }
    SUBCASE("toVectorXZ")
    {
        using pomdog::math::toVectorXZ;
        auto v = toVectorXZ(Vector2{3.0f, 4.0f});
        REQUIRE(v.x == 3.0f);
        REQUIRE(v.z == 4.0f);
    }
    SUBCASE("length")
    {
        using pomdog::math::length;
        REQUIRE(length(VectorXZ{0.0f, 0.0f}) == approx(0.0f));
        REQUIRE(length(VectorXZ{3.0f, 4.0f}) == approx(5.0f));
    }
    SUBCASE("dot")
    {
        using pomdog::math::dot;
        REQUIRE(dot(VectorXZ{1, 0}, VectorXZ{0, 1}) == approx(0.0f));
        REQUIRE(dot(VectorXZ{1, 0}, VectorXZ{1, 0}) == approx(1.0f));
    }
    SUBCASE("cross")
    {
        using pomdog::math::cross;
        REQUIRE(cross(VectorXZ{1, 0}, VectorXZ{0, 1}) == approx(1.0f));
        REQUIRE(cross(VectorXZ{0, 1}, VectorXZ{1, 0}) == approx(-1.0f));
    }
}
