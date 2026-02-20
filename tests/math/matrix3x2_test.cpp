// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/matrix3x2.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Matrix3x2;
using pomdog::Radian;
using pomdog::Vector2;
using pomdog::math::multiply;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("Matrix3x2")
{
    SUBCASE("first test case")
    {
        Matrix3x2 matrix{
            1.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 1.0f};
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(1.0f == matrix(0, 1));
        REQUIRE(1.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(1.0f == matrix(2, 0));
        REQUIRE(1.0f == matrix(2, 1));

        matrix = Matrix3x2{
            0.0f, 1.0f,
            3.0f, 4.0f,
            6.0f, 7.0f};
        REQUIRE(0.0f == matrix(0, 0));
        REQUIRE(1.0f == matrix(0, 1));
        REQUIRE(3.0f == matrix(1, 0));
        REQUIRE(4.0f == matrix(1, 1));
        REQUIRE(6.0f == matrix(2, 0));
        REQUIRE(7.0f == matrix(2, 1));
    }
    SUBCASE("createIdentity")
    {
        Matrix3x2 matrix = Matrix3x2::createIdentity();
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
    }
    SUBCASE("multiply scalar")
    {
        REQUIRE(
            Matrix3x2{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f} ==
            Matrix3x2::createIdentity() * 0.0f);
        REQUIRE(
            Matrix3x2::createIdentity() ==
            Matrix3x2::createIdentity() * 1.0f);
        REQUIRE(
            Matrix3x2{4.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f} ==
            Matrix3x2::createIdentity() * 4.0f);
    }
    SUBCASE("multiply matrix")
    {
        REQUIRE(
            Matrix3x2{1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f} ==
            multiply(Matrix3x2::createIdentity(), Matrix3x2::createIdentity()));

        REQUIRE(
            Matrix3x2{
                0.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 0.0f} ==
            multiply(
                Matrix3x2::createIdentity(),
                Matrix3x2{
                    0.0f, 0.0f,
                    0.0f, 0.0f,
                    0.0f, 0.0f}));

        REQUIRE(
            Matrix3x2{
                0.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 0.0f} ==
            multiply(
                Matrix3x2{
                    0.0f, 0.0f,
                    0.0f, 0.0f,
                    0.0f, 0.0f},
                Matrix3x2::createIdentity()));

        REQUIRE(
            Matrix3x2{
                46.0f, 41.0f,
                18.0f, 15.0f,
                74.0f, 67.0f} ==
            multiply(
                Matrix3x2{
                    3.0f, 4.0f,
                    1.0f, 2.0f,
                    5.0f, 6.0f},
                Matrix3x2{
                    10.0f, 11.0f,
                    4.0f, 2.0f,
                    0.0f, 0.0f}));

        REQUIRE(
            Matrix3x2{
                46.0f, 41.0f,
                18.0f, 15.0f,
                81.0f, 78.0f} ==
            multiply(
                Matrix3x2{
                    3.0f, 4.0f,
                    1.0f, 2.0f,
                    5.0f, 6.0f},
                Matrix3x2{
                    10.0f, 11.0f,
                    4.0f, 2.0f,
                    7.0f, 11.0f}));
    }
    SUBCASE("createTranslation")
    {
        auto matrix = Matrix3x2::createTranslation(Vector2{3.0f, 4.0f});
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(2, 0) == 3.0f);
        REQUIRE(matrix(2, 1) == 4.0f);
    }
    SUBCASE("createScale")
    {
        auto matrix = Matrix3x2::createScale(Vector2{3.0f, 4.0f});
        REQUIRE(matrix(0, 0) == 3.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 4.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);

        matrix = Matrix3x2::createScale(7.0f);
        REQUIRE(matrix(0, 0) == 7.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 7.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
    }
    SUBCASE("createRotation")
    {
        auto matrix = Matrix3x2::createRotation(Radian<float>{3.0f});
        REQUIRE(matrix(0, 0) == approx(std::cos(3.0f)));
        REQUIRE(matrix(0, 1) == approx(std::sin(3.0f)));
        REQUIRE(matrix(1, 0) == approx(-std::sin(3.0f)));
        REQUIRE(matrix(1, 1) == approx(std::cos(3.0f)));
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
    }
    SUBCASE("createSkew")
    {
        auto matrix = Matrix3x2::createSkew(Vector2{3.0f, 4.0f});
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == approx(std::tan(4.0f)));
        REQUIRE(matrix(1, 0) == approx(std::tan(3.0f)));
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
    }
    SUBCASE("data")
    {
        Matrix3x2 matrix{0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        REQUIRE(0.0f == *(matrix.data() + 0));
        REQUIRE(1.0f == *(matrix.data() + 1));
        REQUIRE(2.0f == *(matrix.data() + 2));
        REQUIRE(3.0f == *(matrix.data() + 3));
        REQUIRE(4.0f == *(matrix.data() + 4));
        REQUIRE(5.0f == *(matrix.data() + 5));
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }
}
