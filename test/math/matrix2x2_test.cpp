// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix2x2.h"
#include <catch_amalgamated.hpp>

using namespace pomdog;
using pomdog::Matrix2x2;

TEST_CASE("Matrix2x2", "[Matrix2x2]")
{
    SECTION("FirstTestCase")
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
    SECTION("Identity")
    {
        REQUIRE(Matrix2x2::createIdentity()(0, 0) == 1.0f);
        REQUIRE(Matrix2x2::createIdentity()(0, 1) == 0.0f);
        REQUIRE(Matrix2x2::createIdentity()(1, 0) == 0.0f);
        REQUIRE(Matrix2x2::createIdentity()(1, 1) == 1.0f);

        Matrix2x2 matrix = Matrix2x2::createIdentity();
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
    }
    SECTION("Multiply_Scalar")
    {
        REQUIRE(
            Matrix2x2(0.0f, 0.0f, 0.0f, 0.0f) ==
            math::multiply(Matrix2x2::createIdentity(), 0.0f));
        REQUIRE(
            Matrix2x2::createIdentity() ==
            math::multiply(Matrix2x2::createIdentity(), 1.0f));
        REQUIRE(
            Matrix2x2(4.0f, 0.0f, 0.0f, 4.0f) ==
            math::multiply(Matrix2x2::createIdentity(), 4.0f));
    }
    SECTION("Multiply_Matrix")
    {
        REQUIRE(
            Matrix2x2(1.0f, 0.0f, 0.0f, 1.0f) ==
            math::multiply(
                Matrix2x2::createIdentity(),
                Matrix2x2::createIdentity()));

        REQUIRE(
            Matrix2x2(0.0f, 0.0f, 0.0f, 0.0f) ==
            math::multiply(
                Matrix2x2::createIdentity(),
                Matrix2x2(0.0f, 0.0f, 0.0f, 0.0f)));

        REQUIRE(
            Matrix2x2(157.0f, 752.0f, 156.0f, 176.0f) ==
            math::multiply(
                Matrix2x2(5.0f, 61.0f, 20.0f, 8.0f),
                Matrix2x2(7.0f, 4.0f, 2.0f, 12.0f)));

        REQUIRE(
            Matrix2x2(46.0f, 41.0f, 106.0f, 101.0f) ==
            math::multiply(
                Matrix2x2(3.0f, 4.0f, 8.0f, 9.0f),
                Matrix2x2(2.0f, 7.0f, 10.0f, 5.0f)));
    }
    SECTION("Data")
    {
        Matrix2x2 matrix{0.0f, 1.0f, 2.0f, 3.0f};
        REQUIRE(0.0f == *(matrix.data() + 0));
        REQUIRE(1.0f == *(matrix.data() + 1));
        REQUIRE(2.0f == *(matrix.data() + 2));
        REQUIRE(3.0f == *(matrix.data() + 3));
    }
}
