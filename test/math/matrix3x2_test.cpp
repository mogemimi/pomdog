// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix3x2.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include <catch_amalgamated.hpp>

using namespace pomdog;
using pomdog::Matrix3x2;
using pomdog::Vector2;

namespace {

static constexpr auto Epsilon = std::numeric_limits<float>::epsilon();

} // namespace

TEST_CASE("Matrix3x2", "[Matrix3x2]")
{
    SECTION("FirstTestCase")
    {
        Matrix3x2 matrix{
            1.0f, 1.0f, //
            1.0f, 1.0f, //
            1.0f, 1.0f, //
        };
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(1.0f == matrix(0, 1));
        REQUIRE(1.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(1.0f == matrix(2, 0));
        REQUIRE(1.0f == matrix(2, 1));

        matrix = Matrix3x2{
            0.0f, 1.0f, //
            3.0f, 4.0f, //
            6.0f, 7.0f, //
        };
        REQUIRE(0.0f == matrix(0, 0));
        REQUIRE(1.0f == matrix(0, 1));
        REQUIRE(3.0f == matrix(1, 0));
        REQUIRE(4.0f == matrix(1, 1));
        REQUIRE(6.0f == matrix(2, 0));
        REQUIRE(7.0f == matrix(2, 1));
    }
    SECTION("Identity")
    {
        REQUIRE(1.0f == Matrix3x2::createIdentity()(0, 0));
        REQUIRE(0.0f == Matrix3x2::createIdentity()(0, 1));
        REQUIRE(0.0f == Matrix3x2::createIdentity()(1, 0));
        REQUIRE(1.0f == Matrix3x2::createIdentity()(1, 1));
        REQUIRE(0.0f == Matrix3x2::createIdentity()(2, 0));
        REQUIRE(0.0f == Matrix3x2::createIdentity()(2, 1));

        Matrix3x2 matrix = Matrix3x2::createIdentity();
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
    }
    SECTION("Add")
    {
        auto matrix1 = Matrix3x2::createTranslation({3.0f, 4.0f});
        auto matrix2 = Matrix3x2::createTranslation({5.0f, 2.0f});

        auto matrix = matrix1 + matrix2;
        REQUIRE(2.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(2.0f == matrix(1, 1));
        REQUIRE(8.0f == matrix(2, 0));
        REQUIRE(6.0f == matrix(2, 1));
    }
    SECTION("Subtract")
    {
        auto matrix1 = Matrix3x2::createTranslation({5.0f, 4.0f});
        auto matrix2 = Matrix3x2::createTranslation({2.0f, 3.0f});

        auto matrix = matrix1 - matrix2;
        REQUIRE(0.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(0.0f == matrix(1, 1));
        REQUIRE(3.0f == matrix(2, 0));
        REQUIRE(1.0f == matrix(2, 1));
    }
    SECTION("Multiply_Scalar")
    {
        REQUIRE(
            Matrix3x2(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) ==
            math::multiply(Matrix3x2::createIdentity(), 0.0f));
        REQUIRE(
            Matrix3x2::createIdentity() ==
            math::multiply(Matrix3x2::createIdentity(), 1.0f));
        REQUIRE(
            Matrix3x2(4.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f) ==
            math::multiply(Matrix3x2::createIdentity(), 4.0f));
    }
    SECTION("Multiply_Matrix")
    {
        REQUIRE(
            Matrix3x2::createIdentity() ==
            math::multiply(
                Matrix3x2::createIdentity(),
                Matrix3x2::createIdentity()));

        REQUIRE(
            Matrix3x2(
                0.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 0.0f) ==
            math::multiply(
                Matrix3x2::createIdentity(),
                Matrix3x2(
                    0.0f, 0.0f,
                    0.0f, 0.0f,
                    0.0f, 0.0f)));

        REQUIRE(
            Matrix3x2(
                0.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 0.0f) ==
            math::multiply(
                Matrix3x2(
                    0.0f, 0.0f,
                    0.0f, 0.0f,
                    0.0f, 0.0f),
                Matrix3x2::createIdentity()));

        REQUIRE(
            Matrix3x2(
                46.0f, 41.0f,
                106.0f, 101.0f,
                48.0f, 130.0f) ==
            math::multiply(
                Matrix3x2(
                    3.0f, 4.0f,
                    8.0f, 9.0f,
                    17.0f, 1.0f),
                Matrix3x2(
                    2.0f, 7.0f,
                    10.0f, 5.0f,
                    4.0f, 6.0f)));
    }
    SECTION("CreateTranslation")
    {
        auto matrix = Matrix3x2::createTranslation({3.0f, 4.0f});
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(3.0f == matrix(2, 0));
        REQUIRE(4.0f == matrix(2, 1));
    }
    SECTION("CreateScale")
    {
        auto matrix = Matrix3x2::createScale({3.0f, 4.0f});
        REQUIRE(3.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(4.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));

        matrix = Matrix3x2::createScale(7.0f);
        REQUIRE(7.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(7.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
    }
    SECTION("CreateRotation")
    {
        auto matrix = Matrix3x2::createRotation(3.0f);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(0, 0)) < Epsilon);
        REQUIRE(std::abs(std::sin(3.0f) - matrix(0, 1)) < Epsilon);
        REQUIRE(std::abs(-std::sin(3.0f) - matrix(1, 0)) < Epsilon);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(1, 1)) < Epsilon);
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
    }
    SECTION("CreateSkew")
    {
        auto matrix = Matrix3x2::createSkew({3.0f, 4.0f});
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(std::abs(std::tan(4.0f) - matrix(0, 1)) < Epsilon);
        REQUIRE(std::abs(std::tan(3.0f) - matrix(1, 0)) < Epsilon);
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
    }
    SECTION("Data")
    {
        Matrix3x2 matrix{0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
        REQUIRE(0.0f == *(matrix.data() + 0));
        REQUIRE(1.0f == *(matrix.data() + 1));
        REQUIRE(2.0f == *(matrix.data() + 2));
        REQUIRE(3.0f == *(matrix.data() + 3));
        REQUIRE(4.0f == *(matrix.data() + 4));
        REQUIRE(5.0f == *(matrix.data() + 5));
    }
}
