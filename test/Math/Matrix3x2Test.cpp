// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "catch.hpp"

using Pomdog::Matrix3x2;
using Pomdog::Vector2;

namespace {

static constexpr auto Epsilon = std::numeric_limits<float>::epsilon();

} // namespace

TEST_CASE("Matrix3x2", "[Matrix3x2]")
{
    SECTION("FirstTestCase")
    {
        Matrix3x2 matrix {
            1.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 1.0f,
        };
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(1.0f == matrix(0, 1));
        REQUIRE(1.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(1.0f == matrix(2, 0));
        REQUIRE(1.0f == matrix(2, 1));

        matrix = Matrix3x2 {
            0.0f, 1.0f,
            3.0f, 4.0f,
            6.0f, 7.0f,
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
        REQUIRE(1.0f == Matrix3x2::Identity(0, 0));
        REQUIRE(0.0f == Matrix3x2::Identity(0, 1));
        REQUIRE(0.0f == Matrix3x2::Identity(1, 0));
        REQUIRE(1.0f == Matrix3x2::Identity(1, 1));
        REQUIRE(0.0f == Matrix3x2::Identity(2, 0));
        REQUIRE(0.0f == Matrix3x2::Identity(2, 1));

        Matrix3x2 matrix = Matrix3x2::Identity;
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
    }
    SECTION("Add")
    {
        auto matrix1 = Matrix3x2::CreateTranslation({3.0f, 4.0f});
        auto matrix2 = Matrix3x2::CreateTranslation({5.0f, 2.0f});

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
        auto matrix1 = Matrix3x2::CreateTranslation({5.0f, 4.0f});
        auto matrix2 = Matrix3x2::CreateTranslation({2.0f, 3.0f});

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
            Matrix3x2::Multiply(Matrix3x2::Identity, 0.0f));
        REQUIRE(
            Matrix3x2::Identity ==
            Matrix3x2::Multiply(Matrix3x2::Identity, 1.0f));
        REQUIRE(
            Matrix3x2(4.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f) ==
            Matrix3x2::Multiply(Matrix3x2::Identity, 4.0f));
    }
    SECTION("Multiply_Matrix")
    {
        REQUIRE(
            Matrix3x2::Identity ==
            Matrix3x2::Multiply(
                Matrix3x2::Identity,
                Matrix3x2::Identity));

        REQUIRE(
            Matrix3x2(
                0.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 0.0f) ==
            Matrix3x2::Multiply(
                Matrix3x2::Identity,
                Matrix3x2(
                    0.0f, 0.0f,
                    0.0f, 0.0f,
                    0.0f, 0.0f)));

        REQUIRE(
            Matrix3x2(
                0.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 0.0f) ==
            Matrix3x2::Multiply(
                Matrix3x2(
                    0.0f, 0.0f,
                    0.0f, 0.0f,
                    0.0f, 0.0f),
                Matrix3x2::Identity));

        REQUIRE(
            Matrix3x2(
                46.0f, 41.0f,
                106.0f, 101.0f,
                48.0f, 130.0f) ==
            Matrix3x2::Multiply(
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
        auto matrix = Matrix3x2::CreateTranslation({3.0f, 4.0f});
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(3.0f == matrix(2, 0));
        REQUIRE(4.0f == matrix(2, 1));
    }
    SECTION("CreateScale")
    {
        auto matrix = Matrix3x2::CreateScale({3.0f, 4.0f});
        REQUIRE(3.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(4.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));

        matrix = Matrix3x2::CreateScale(7.0f);
        REQUIRE(7.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(7.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
    }
    SECTION("CreateRotation")
    {
        auto matrix = Matrix3x2::CreateRotation(3.0f);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(0, 0)) < Epsilon);
        REQUIRE(std::abs(std::sin(3.0f) - matrix(0, 1)) < Epsilon);
        REQUIRE(std::abs(-std::sin(3.0f) - matrix(1, 0)) < Epsilon);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(1, 1)) < Epsilon);
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
    }
    SECTION("CreateSkew")
    {
        auto matrix = Matrix3x2::CreateSkew({3.0f, 4.0f});
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
        REQUIRE(0.0f == *(matrix.Data() + 0));
        REQUIRE(1.0f == *(matrix.Data() + 1));
        REQUIRE(2.0f == *(matrix.Data() + 2));
        REQUIRE(3.0f == *(matrix.Data() + 3));
        REQUIRE(4.0f == *(matrix.Data() + 4));
        REQUIRE(5.0f == *(matrix.Data() + 5));
    }
}
