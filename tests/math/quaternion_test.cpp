// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/quaternion.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Quaternion;
using pomdog::Vector3;
using pomdog::math::toRadian;

// FIXME: https://github.com/doctest/doctest/issues/735
//        doctest::Approx(double) with -Wdouble-promotion causes warnings on Clang compiler.
POMDOG_CLANG_SUPPRESS_WARNING("-Wdouble-promotion")

namespace {

constexpr float Epsilon = 0.0000001f;
constexpr float Epsilon2 = 0.0000005f;

} // namespace

TEST_CASE("Quaternion")
{
    SUBCASE("constructor")
    {
        Quaternion quaternion{0, 0, 0, 1};
        REQUIRE(quaternion.x == 0.0f);
        REQUIRE(quaternion.y == 0.0f);
        REQUIRE(quaternion.z == 0.0f);
        REQUIRE(quaternion.w == 1.0f);

        quaternion = {4, 3, 2, 1};
        REQUIRE(quaternion.x == 4.0f);
        REQUIRE(quaternion.y == 3.0f);
        REQUIRE(quaternion.z == 2.0f);
        REQUIRE(quaternion.w == 1.0f);
    }
    SUBCASE("constants")
    {
        REQUIRE(Quaternion::createIdentity().x == 0.0f);
        REQUIRE(Quaternion::createIdentity().y == 0.0f);
        REQUIRE(Quaternion::createIdentity().z == 0.0f);
        REQUIRE(Quaternion::createIdentity().w == 1.0f);
        REQUIRE(Quaternion::createIdentity() == Quaternion{0.0f, 0.0f, 0.0f, 1.0f});
    }
    SUBCASE("multiply quaternion identity")
    {
        REQUIRE(
            Quaternion::createIdentity() ==
            Quaternion::createIdentity() * Quaternion::createIdentity());
    }
    SUBCASE("multiply quaternion")
    {
        // For positive quaternions:
        // q1 = {x=1, y=2, z=3, w=4}, q2 = {x=5, y=6, z=7, w=8}
        // result.x = w1*x2 + x1*w2 + y1*z2 - z1*y2 = 4*5 + 1*8 + 2*7 - 3*6 = 24
        // result.y = w1*y2 + y1*w2 + z1*x2 - x1*z2 = 4*6 + 2*8 + 3*5 - 1*7 = 48
        // result.z = w1*z2 + z1*w2 + x1*y2 - y1*x2 = 4*7 + 3*8 + 1*6 - 2*5 = 48
        // result.w = w1*w2 - x1*x2 - y1*y2 - z1*z2 = 4*8 - 1*5 - 2*6 - 3*7 = -6
        REQUIRE(
            Quaternion{24.0f, 48.0f, 48.0f, -6.0f} ==
            Quaternion{1, 2, 3, 4} * Quaternion{5, 6, 7, 8});

        // For negative quaternions:
        // q1 = {x=-1, y=-2, z=-3, w=4}, q2 = {x=-5, y=-6, z=-7, w=8}
        // result.x = w1*x2 + x1*w2 + y1*z2 - z1*y2 = 4*(-5) + (-1)*8 + (-2)*(-7) - (-3)*(-6) = -20 - 8 + 14 - 18 = -32
        // result.y = w1*y2 + y1*w2 + z1*x2 - x1*z2 = 4*(-6) + (-2)*8 + (-3)*(-5) - (-1)*(-7) = -24 - 16 + 15 - 7 = -32
        // result.z = w1*z2 + z1*w2 + x1*y2 - y1*x2 = 4*(-7) + (-3)*8 + (-1)*(-6) - (-2)*(-5) = -28 - 24 + 6 - 10 = -56
        // result.w = w1*w2 - x1*x2 - y1*y2 - z1*z2 = 4*8 - (-1)*(-5) - (-2)*(-6) - (-3)*(-7) = 32 - 5 - 12 - 21 = -6
        REQUIRE(
            Quaternion{-32.0f, -32.0f, -56.0f, -6.0f} ==
            Quaternion{-1, -2, -3, 4} * Quaternion{-5, -6, -7, 8});
    }
    SUBCASE("add")
    {
        auto a = Quaternion{1, 2, 3, 4};
        auto b = Quaternion{5, 6, 7, 8};
        auto c = a + b;
        REQUIRE(6.0f == c.x);
        REQUIRE(8.0f == c.y);
        REQUIRE(10.0f == c.z);
        REQUIRE(12.0f == c.w);
    }
    SUBCASE("subtract")
    {
        auto a = Quaternion{1, 2, 3, 4};
        auto b = Quaternion{5, 7, 9, 11};
        auto c = a - b;
        REQUIRE(-4.0f == c.x);
        REQUIRE(-5.0f == c.y);
        REQUIRE(-6.0f == c.z);
        REQUIRE(-7.0f == c.w);
    }
    SUBCASE("multiply scalar")
    {
        auto a = Quaternion{1, 2, 3, 4};
        auto scalar = 4.0f;
        auto c = a * scalar;
        REQUIRE(4.0f == c.x);
        REQUIRE(8.0f == c.y);
        REQUIRE(12.0f == c.z);
        REQUIRE(16.0f == c.w);
    }
    SUBCASE("divide scalar")
    {
        auto a = Quaternion{4, 8, 12, 16};
        auto scalar = 4.0f;
        auto c = a / scalar;
        REQUIRE(1.0f == c.x);
        REQUIRE(2.0f == c.y);
        REQUIRE(3.0f == c.z);
        REQUIRE(4.0f == c.w);
    }
    SUBCASE("length")
    {
        REQUIRE(1.0f == pomdog::math::length(Quaternion{0, 0, 0, 1}));
        REQUIRE(1.0f == pomdog::math::length(Quaternion{0, 0, 1, 0}));
        REQUIRE(1.0f == pomdog::math::length(Quaternion{0, 1, 0, 0}));
        REQUIRE(1.0f == pomdog::math::length(Quaternion{1, 0, 0, 0}));
        REQUIRE(doctest::Approx(std::sqrt(4.0f * 4.0f + 3.0f * 3.0f + 2.0f * 2.0f + 1.0f * 1.0f)).epsilon(Epsilon) == pomdog::math::length(Quaternion{4, 3, 2, 1}));
        REQUIRE(doctest::Approx(std::sqrt(1.0f * 1.0f + 2.0f * 2.0f + 3.0f * 3.0f + 4.0f * 4.0f)).epsilon(Epsilon) == pomdog::math::length(Quaternion{1, 2, 3, 4}));
    }
    SUBCASE("normalize")
    {
        const auto actual1 = pomdog::math::normalize(Quaternion{0, 0, 0, 1});
        REQUIRE(doctest::Approx(actual1.x).epsilon(Epsilon) == 0.0f);
        REQUIRE(doctest::Approx(actual1.y).epsilon(Epsilon) == 0.0f);
        REQUIRE(doctest::Approx(actual1.z).epsilon(Epsilon) == 0.0f);
        REQUIRE(doctest::Approx(actual1.w).epsilon(Epsilon) == 1.0f);

        // norm = sqrt(329^2 + 94^2 + 37^2 + 115^2) = sqrt(131671) ≈ 362.865
        const auto actual2 = pomdog::math::normalize(Quaternion{329, 94, 37, 115});
        const auto norm = std::sqrt(329.0f * 329.0f + 94.0f * 94.0f + 37.0f * 37.0f + 115.0f * 115.0f);
        REQUIRE(doctest::Approx(actual2.x).epsilon(Epsilon) == 329.0f / norm);
        REQUIRE(doctest::Approx(actual2.y).epsilon(Epsilon) == 94.0f / norm);
        REQUIRE(doctest::Approx(actual2.z).epsilon(Epsilon) == 37.0f / norm);
        REQUIRE(doctest::Approx(actual2.w).epsilon(Epsilon) == 115.0f / norm);
    }
    SUBCASE("dot")
    {
        REQUIRE(doctest::Approx(1.0).epsilon(Epsilon) == pomdog::math::dot(Quaternion{0, 0, 0, 1}, Quaternion{0, 0, 0, 1}));
        REQUIRE(doctest::Approx(1.0).epsilon(Epsilon) == pomdog::math::dot(Quaternion{0, 0, 1, 0}, Quaternion{0, 0, 1, 0}));
        REQUIRE(doctest::Approx(1.0).epsilon(Epsilon) == pomdog::math::dot(Quaternion{0, 1, 0, 0}, Quaternion{0, 1, 0, 0}));
        REQUIRE(doctest::Approx(1.0).epsilon(Epsilon) == pomdog::math::dot(Quaternion{1, 0, 0, 0}, Quaternion{1, 0, 0, 0}));
        REQUIRE(doctest::Approx(70.0).epsilon(Epsilon) == pomdog::math::dot(Quaternion{1, 2, 3, 4}, Quaternion{5, 6, 7, 8}));
    }
    SUBCASE("invert")
    {
        auto result = pomdog::math::invert(Quaternion{0, 0, 0, 1});
        REQUIRE(result.x == 0.0f);
        REQUIRE(result.y == 0.0f);
        REQUIRE(result.z == 0.0f);
        REQUIRE(result.w == 1.0f);

        result = pomdog::math::invert(Quaternion{1, 0, 0, 0});
        REQUIRE(result.x == -1.0f);
        REQUIRE(result.y == 0.0f);
        REQUIRE(result.z == 0.0f);
        REQUIRE(result.w == 0.0f);

        result = pomdog::math::invert(Quaternion{0, 1, 0, 0});
        REQUIRE(result.x == 0.0f);
        REQUIRE(result.y == -1.0f);
        REQUIRE(result.z == 0.0f);
        REQUIRE(result.w == 0.0f);

        result = pomdog::math::invert(Quaternion{0, 0, 1, 0});
        REQUIRE(result.x == 0.0f);
        REQUIRE(result.y == 0.0f);
        REQUIRE(result.z == -1.0f);
        REQUIRE(result.w == 0.0f);
    }
    SUBCASE("invert rotate x")
    {
        const auto rotation = Quaternion::createFromAxisAngle(Vector3{1.0f, 0.0f, 0.0f}, toRadian(45.0f));
        const auto invRotation = pomdog::math::invert(rotation);

        auto result = rotation * invRotation;
        REQUIRE(doctest::Approx(result.x).epsilon(Epsilon2) == 0.0f);
        REQUIRE(doctest::Approx(result.y).epsilon(Epsilon2) == 0.0f);
        REQUIRE(doctest::Approx(result.z).epsilon(Epsilon2) == 0.0f);
        REQUIRE(doctest::Approx(result.w).epsilon(Epsilon2) == 1.0f);
    }
    SUBCASE("invert rotate y")
    {
        const auto rotation = Quaternion::createFromAxisAngle(Vector3{0.0f, 1.0f, 0.0f}, toRadian(45.0f));
        const auto invRotation = pomdog::math::invert(rotation);

        auto result = rotation * invRotation;
        REQUIRE(doctest::Approx(result.x).epsilon(Epsilon2) == 0.0f);
        REQUIRE(doctest::Approx(result.y).epsilon(Epsilon2) == 0.0f);
        REQUIRE(doctest::Approx(result.z).epsilon(Epsilon2) == 0.0f);
        REQUIRE(doctest::Approx(result.w).epsilon(Epsilon2) == 1.0f);
    }
    SUBCASE("invert rotate z")
    {
        const auto rotation = Quaternion::createFromAxisAngle(Vector3{0.0f, 0.0f, 1.0f}, toRadian(45.0f));
        const auto invRotation = pomdog::math::invert(rotation);

        auto result = rotation * invRotation;
        REQUIRE(doctest::Approx(result.x).epsilon(Epsilon2) == 0.0f);
        REQUIRE(doctest::Approx(result.y).epsilon(Epsilon2) == 0.0f);
        REQUIRE(doctest::Approx(result.z).epsilon(Epsilon2) == 0.0f);
        REQUIRE(doctest::Approx(result.w).epsilon(Epsilon2) == 1.0f);
    }
    SUBCASE("rotate")
    {
        const auto rotation = Quaternion::createFromAxisAngle(Vector3{0.0f, 1.0f, 0.0f}, toRadian(90.0f));
        const auto vector = pomdog::math::rotate(rotation, Vector3{1.0f, 0.0f, 0.0f});
        REQUIRE(doctest::Approx(vector.x).epsilon(Epsilon2) == 0.0f);
        REQUIRE(doctest::Approx(vector.y).epsilon(Epsilon2) == 0.0f);
        REQUIRE(doctest::Approx(vector.z).epsilon(Epsilon2) == -1.0f);
    }
    SUBCASE("operator/")
    {
        REQUIRE(
            (Quaternion{1.0f, 2.0f, 3.0f, 4.0f} / 5.0f) ==
            Quaternion{1.0f / 5.0f, 2.0f / 5.0f, 3.0f / 5.0f, 4.0f / 5.0f});
    }
}
