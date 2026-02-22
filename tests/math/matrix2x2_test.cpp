// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/matrix2x2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Matrix2x2;
using pomdog::math::multiply;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("Matrix2x2")
{
    SUBCASE("constructor")
    {
        Matrix2x2 matrix{1.0f, 1.0f, 1.0f, 1.0f};
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 1.0f);
        REQUIRE(matrix(1, 0) == 1.0f);
        REQUIRE(matrix(1, 1) == 1.0f);

        matrix = Matrix2x2{0.0f, 1.0f, 2.0f, 3.0f};
        REQUIRE(matrix(0, 0) == 0.0f);
        REQUIRE(matrix(0, 1) == 1.0f);
        REQUIRE(matrix(1, 0) == 2.0f);
        REQUIRE(matrix(1, 1) == 3.0f);
    }
    SUBCASE("createIdentity")
    {
        Matrix2x2 matrix = Matrix2x2::createIdentity();
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
    }
    SUBCASE("multiply scalar")
    {
        REQUIRE(
            Matrix2x2{0.0f, 0.0f, 0.0f, 0.0f} ==
            Matrix2x2::createIdentity() * 0.0f);
        REQUIRE(
            Matrix2x2::createIdentity() ==
            Matrix2x2::createIdentity() * 1.0f);
        REQUIRE(
            Matrix2x2{4.0f, 0.0f, 0.0f, 4.0f} ==
            Matrix2x2::createIdentity() * 4.0f);
    }
    SUBCASE("multiply matrix")
    {
        REQUIRE(
            Matrix2x2{1.0f, 0.0f, 0.0f, 1.0f} ==
            multiply(Matrix2x2::createIdentity(), Matrix2x2::createIdentity()));

        REQUIRE(
            Matrix2x2{
                0.0f, 0.0f,
                0.0f, 0.0f} ==
            multiply(
                Matrix2x2::createIdentity(),
                Matrix2x2{
                    0.0f, 0.0f,
                    0.0f, 0.0f}));

        REQUIRE(
            Matrix2x2{
                0.0f, 0.0f,
                0.0f, 0.0f} ==
            multiply(
                Matrix2x2{
                    0.0f, 0.0f,
                    0.0f, 0.0f},
                Matrix2x2::createIdentity()));

        REQUIRE(
            Matrix2x2{
                46.0f, 41.0f,
                18.0f, 15.0f} ==
            multiply(
                Matrix2x2{
                    3.0f, 4.0f,
                    1.0f, 2.0f},
                Matrix2x2{
                    10.0f, 11.0f,
                    4.0f, 2.0f}));

        REQUIRE(
            Matrix2x2{
                41.0f, 62.0f,
                14.0f, 20.0f} ==
            multiply(
                Matrix2x2{
                    10.0f, 11.0f,
                    4.0f, 2.0f},
                Matrix2x2{
                    3.0f, 4.0f,
                    1.0f, 2.0f}));
    }
    SUBCASE("data")
    {
        Matrix2x2 matrix{0.0f, 1.0f, 2.0f, 3.0f};
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        REQUIRE(0.0f == *(matrix.data() + 0));
        REQUIRE(1.0f == *(matrix.data() + 1));
        REQUIRE(2.0f == *(matrix.data() + 2));
        REQUIRE(3.0f == *(matrix.data() + 3));
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }
    SUBCASE("determinant")
    {
        using pomdog::math::determinant;
        REQUIRE(
            approx(-26.0f) ==
            determinant(Matrix2x2{
                3.0f, 4.0f,
                5.0f, -2.0f}));

        REQUIRE(
            approx(-92.0f) ==
            determinant(Matrix2x2{
                -3.0f, 4.0f,
                32.0f, -12.0f}));

        REQUIRE(
            approx(1.0f) ==
            determinant(Matrix2x2::createIdentity()));
    }
}
