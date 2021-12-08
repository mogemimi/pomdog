// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix3x3.hpp"
#include "pomdog/math/radian.hpp"
#include "pomdog/math/vector2.hpp"
#include "pomdog/math/vector3.hpp"
#include <catch_amalgamated.hpp>

using pomdog::Matrix3x3;
using pomdog::Vector2;
using pomdog::Vector3;

namespace {

static constexpr auto Epsilon = std::numeric_limits<float>::epsilon();

} // namespace

TEST_CASE("Matrix3x3", "[Matrix3x3]")
{
    SECTION("FirstTestCase")
    {
        Matrix3x3 matrix{
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f
        };
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(1.0f == matrix(0, 1));
        REQUIRE(1.0f == matrix(0, 2));
        REQUIRE(1.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(1.0f == matrix(1, 2));
        REQUIRE(1.0f == matrix(2, 0));
        REQUIRE(1.0f == matrix(2, 1));
        REQUIRE(1.0f == matrix(2, 2));

        matrix = Matrix3x3{
            0.0f, 1.0f, 2.0f,
            3.0f, 4.0f, 5.0f,
            6.0f, 7.0f, 8.0f
        };
        REQUIRE(0.0f == matrix(0, 0));
        REQUIRE(1.0f == matrix(0, 1));
        REQUIRE(2.0f == matrix(0, 2));
        REQUIRE(3.0f == matrix(1, 0));
        REQUIRE(4.0f == matrix(1, 1));
        REQUIRE(5.0f == matrix(1, 2));
        REQUIRE(6.0f == matrix(2, 0));
        REQUIRE(7.0f == matrix(2, 1));
        REQUIRE(8.0f == matrix(2, 2));
    }
    SECTION("Identity")
    {
        REQUIRE(1.0f == Matrix3x3::Identity(0, 0));
        REQUIRE(0.0f == Matrix3x3::Identity(0, 1));
        REQUIRE(0.0f == Matrix3x3::Identity(0, 2));
        REQUIRE(0.0f == Matrix3x3::Identity(1, 0));
        REQUIRE(1.0f == Matrix3x3::Identity(1, 1));
        REQUIRE(0.0f == Matrix3x3::Identity(1, 2));
        REQUIRE(0.0f == Matrix3x3::Identity(2, 0));
        REQUIRE(0.0f == Matrix3x3::Identity(2, 1));
        REQUIRE(1.0f == Matrix3x3::Identity(2, 2));

        Matrix3x3 matrix = Matrix3x3::Identity;
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(0, 2));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(1, 2));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
        REQUIRE(1.0f == matrix(2, 2));
    }
    SECTION("Multiply_Scalar")
    {
        REQUIRE(
            Matrix3x3(
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f) ==
            Matrix3x3::Multiply(Matrix3x3::Identity, 0.0f));
        REQUIRE(
            Matrix3x3::Identity ==
            Matrix3x3::Multiply(Matrix3x3::Identity, 1.0f));
        REQUIRE(
            Matrix3x3(
                4.0f, 0.0f, 0.0f,
                0.0f, 4.0f, 0.0f,
                0.0f, 0.0f, 4.0f) ==
            Matrix3x3::Multiply(Matrix3x3::Identity, 4.0f));
    }
    SECTION("Multiply_Matrix")
    {
        REQUIRE(
            Matrix3x3::Identity ==
            Matrix3x3::Multiply(
                Matrix3x3::Identity,
                Matrix3x3::Identity));

        REQUIRE(
            Matrix3x3(
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f) ==
            Matrix3x3::Multiply(
                Matrix3x3::Identity,
                Matrix3x3(
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f)));

        REQUIRE(
            Matrix3x3(
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f) ==
            Matrix3x3::Multiply(
                Matrix3x3(
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f),
                Matrix3x3::Identity));

        REQUIRE(
            Matrix3x3(
                90.0f, 107.0f, 203.0f,
                114.0f, 113.0f, 200.0f,
                64.0f, 154.0f, 289.0f) ==
            Matrix3x3::Multiply(
                Matrix3x3(
                    3.0f, 4.0f, 11.0f,
                    8.0f, 9.0f, 2.0f,
                    17.0f, 1.0f, 5.0f),
                Matrix3x3(
                    2.0f, 7.0f, 13.0f,
                    10.0f, 5.0f, 8.0f,
                    4.0f, 6.0f, 12.0f)));
    }
    SECTION("CreateTranslation")
    {
        auto matrix = Matrix3x3::CreateTranslation({3.0f, 4.0f});
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(0, 2));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(1, 2));
        REQUIRE(3.0f == matrix(2, 0));
        REQUIRE(4.0f == matrix(2, 1));
        REQUIRE(1.0f == matrix(2, 2));
    }
    SECTION("CreateScale")
    {
        auto matrix = Matrix3x3::CreateScale({3.0f, 4.0f, 5.0f});
        REQUIRE(3.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(0, 2));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(4.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(1, 2));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
        REQUIRE(5.0f == matrix(2, 2));

        matrix = Matrix3x3::CreateScale(7.0f);
        REQUIRE(7.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(0, 2));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(7.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(1, 2));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
        REQUIRE(7.0f == matrix(2, 2));
    }
    SECTION("CreateRotationX")
    {
        auto matrix = Matrix3x3::CreateRotationX(3.0f);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(1, 1)) < Epsilon);
        REQUIRE(std::abs(std::sin(3.0f) - matrix(1, 2)) < Epsilon);
        REQUIRE(std::abs(-std::sin(3.0f) - matrix(2, 1)) < Epsilon);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(2, 2)) < Epsilon);

        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(0, 2));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(0.0f == matrix(2, 0));
    }
    SECTION("CreateRotationY")
    {
        auto matrix = Matrix3x3::CreateRotationY(3.0f);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(0, 0)) < Epsilon);
        REQUIRE(std::abs(-std::sin(3.0f) - matrix(0, 2)) < Epsilon);
        REQUIRE(std::abs(std::sin(3.0f) - matrix(2, 0)) < Epsilon);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(2, 2)) < Epsilon);

        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(1, 2));
        REQUIRE(0.0f == matrix(2, 1));
    }
    SECTION("CreateRotationZ")
    {
        auto matrix = Matrix3x3::CreateRotationZ(3.0f);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(0, 0)) < Epsilon);
        REQUIRE(std::abs(std::sin(3.0f) - matrix(0, 1)) < Epsilon);
        REQUIRE(std::abs(-std::sin(3.0f) - matrix(1, 0)) < Epsilon);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(1, 1)) < Epsilon);

        REQUIRE(0.0f == matrix(0, 2));
        REQUIRE(0.0f == matrix(1, 2));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
        REQUIRE(1.0f == matrix(2, 2));
    }
    SECTION("Data")
    {
        Matrix3x3 matrix{0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        REQUIRE(0.0f == *(matrix.Data() + 0));
        REQUIRE(1.0f == *(matrix.Data() + 1));
        REQUIRE(2.0f == *(matrix.Data() + 2));
        REQUIRE(3.0f == *(matrix.Data() + 3));
        REQUIRE(4.0f == *(matrix.Data() + 4));
        REQUIRE(5.0f == *(matrix.Data() + 5));
        REQUIRE(6.0f == *(matrix.Data() + 6));
        REQUIRE(7.0f == *(matrix.Data() + 7));
        REQUIRE(8.0f == *(matrix.Data() + 8));
    }
}
