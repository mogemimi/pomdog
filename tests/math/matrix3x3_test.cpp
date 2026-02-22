// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/matrix3x3.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::Matrix3x3;
using pomdog::Radian;
using pomdog::Vector2;
using pomdog::Vector3;
using pomdog::math::multiply;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("Matrix3x3")
{
    SUBCASE("constructor")
    {
        Matrix3x3 matrix{
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f};
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 1.0f);
        REQUIRE(matrix(0, 2) == 1.0f);
        REQUIRE(matrix(1, 0) == 1.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(1, 2) == 1.0f);
        REQUIRE(matrix(2, 0) == 1.0f);
        REQUIRE(matrix(2, 1) == 1.0f);
        REQUIRE(matrix(2, 2) == 1.0f);

        matrix = Matrix3x3{
            0.0f, 1.0f, 2.0f,
            3.0f, 4.0f, 5.0f,
            6.0f, 7.0f, 8.0f};
        REQUIRE(matrix(0, 0) == 0.0f);
        REQUIRE(matrix(0, 1) == 1.0f);
        REQUIRE(matrix(0, 2) == 2.0f);
        REQUIRE(matrix(1, 0) == 3.0f);
        REQUIRE(matrix(1, 1) == 4.0f);
        REQUIRE(matrix(1, 2) == 5.0f);
        REQUIRE(matrix(2, 0) == 6.0f);
        REQUIRE(matrix(2, 1) == 7.0f);
        REQUIRE(matrix(2, 2) == 8.0f);
    }
    SUBCASE("createIdentity")
    {
        Matrix3x3 matrix = Matrix3x3::createIdentity();
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 1.0f);
    }
    SUBCASE("multiply scalar")
    {
        REQUIRE(
            Matrix3x3{
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f} ==
            Matrix3x3::createIdentity() * 0.0f);

        REQUIRE(
            Matrix3x3::createIdentity() ==
            Matrix3x3::createIdentity() * 1.0f);

        REQUIRE(
            Matrix3x3{
                4.0f, 0.0f, 0.0f,
                0.0f, 4.0f, 0.0f,
                0.0f, 0.0f, 4.0f} ==
            Matrix3x3::createIdentity() * 4.0f);
    }
    SUBCASE("multiply matrix")
    {
        REQUIRE(
            Matrix3x3{
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f} ==
            multiply(
                Matrix3x3::createIdentity(),
                Matrix3x3::createIdentity()));

        REQUIRE(
            Matrix3x3{
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f} ==
            multiply(
                Matrix3x3::createIdentity(),
                Matrix3x3{
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f}));

        REQUIRE(
            Matrix3x3{
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f} ==
            multiply(
                Matrix3x3{
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f},
                Matrix3x3::createIdentity()));
    }
    SUBCASE("data")
    {
        Matrix3x3 matrix{
            0.0f, 1.0f, 2.0f,
            3.0f, 4.0f, 5.0f,
            6.0f, 7.0f, 8.0f};
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        REQUIRE(0.0f == *(matrix.data() + 0));
        REQUIRE(1.0f == *(matrix.data() + 1));
        REQUIRE(2.0f == *(matrix.data() + 2));
        REQUIRE(3.0f == *(matrix.data() + 3));
        REQUIRE(4.0f == *(matrix.data() + 4));
        REQUIRE(5.0f == *(matrix.data() + 5));
        REQUIRE(6.0f == *(matrix.data() + 6));
        REQUIRE(7.0f == *(matrix.data() + 7));
        REQUIRE(8.0f == *(matrix.data() + 8));
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }
    SUBCASE("determinant")
    {
        using pomdog::math::determinant;
        REQUIRE(
            approx(-78.0f) ==
            determinant(Matrix3x3{
                3.0f, 4.0f, 5.0f,
                2.0f, 7.0f, -1.0f,
                4.0f, 1.0f, 5.0f}));

        REQUIRE(
            approx(234.0f) ==
            determinant(Matrix3x3{
                3.0f, 4.0f, 5.0f,
                2.0f, 7.0f, -1.0f,
                -4.0f, 1.0f, 5.0f}));

        REQUIRE(
            approx(1.0f) ==
            determinant(Matrix3x3::createIdentity()));
    }
    SUBCASE("adjoint")
    {
        using pomdog::math::adjoint;
        REQUIRE(
            Matrix3x3{
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f} ==
            adjoint(Matrix3x3::createIdentity()));

        REQUIRE(
            Matrix3x3{
                36.0f, -15.0f, -39.0f,
                -14.0f, -5.0f, 13.0f,
                -26.0f, 13.0f, 13.0f} ==
            adjoint(Matrix3x3{
                3.0f, 4.0f, 5.0f,
                2.0f, 7.0f, -1.0f,
                4.0f, 1.0f, 5.0f}));
    }
    SUBCASE("invert")
    {
        auto matrix = Matrix3x3{
            3.0f, 4.0f, 5.0f,
            2.0f, 7.0f, -1.0f,
            4.0f, 1.0f, 5.0f};

        auto inverted = pomdog::math::invert(matrix);
        auto result = inverted * matrix;

        REQUIRE(result(0, 0) == approx(1.0f));
        REQUIRE(result(0, 1) == approx(0.0f));
        REQUIRE(result(0, 2) == approx(0.0f));
        REQUIRE(result(1, 0) == approx(0.0f));
        REQUIRE(result(1, 1) == approx(1.0f));
        REQUIRE(result(1, 2) == approx(0.0f));
        REQUIRE(result(2, 0) == approx(0.0f));
        REQUIRE(result(2, 1) == approx(0.0f));
        REQUIRE(result(2, 2) == approx(1.0f));
    }
    SUBCASE("createTranslation")
    {
        auto matrix = Matrix3x3::createTranslation(Vector2{3.0f, 4.0f});
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(2, 0) == 3.0f);
        REQUIRE(matrix(2, 1) == 4.0f);
        REQUIRE(matrix(2, 2) == 1.0f);
    }
    SUBCASE("createScale")
    {
        auto matrix = Matrix3x3::createScale(Vector3{3.0f, 4.0f, 5.0f});
        REQUIRE(matrix(0, 0) == 3.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 4.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 5.0f);

        matrix = Matrix3x3::createScale(7.0f);
        REQUIRE(matrix(0, 0) == 7.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 7.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 7.0f);
    }
    SUBCASE("createRotationX")
    {
        auto matrix = Matrix3x3::createRotationX(Radian<f32>{3.0f});
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == approx(std::cos(3.0f)));
        REQUIRE(matrix(1, 2) == approx(std::sin(3.0f)));
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == approx(-std::sin(3.0f)));
        REQUIRE(matrix(2, 2) == approx(std::cos(3.0f)));
    }
    SUBCASE("createRotationY")
    {
        auto matrix = Matrix3x3::createRotationY(Radian<f32>{3.0f});
        REQUIRE(matrix(0, 0) == approx(std::cos(3.0f)));
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == approx(-std::sin(3.0f)));
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(2, 0) == approx(std::sin(3.0f)));
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == approx(std::cos(3.0f)));
    }
    SUBCASE("createRotationZ")
    {
        auto matrix = Matrix3x3::createRotationZ(Radian<f32>{3.0f});
        REQUIRE(matrix(0, 0) == approx(std::cos(3.0f)));
        REQUIRE(matrix(0, 1) == approx(std::sin(3.0f)));
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(1, 0) == approx(-std::sin(3.0f)));
        REQUIRE(matrix(1, 1) == approx(std::cos(3.0f)));
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 1.0f);
    }
}
