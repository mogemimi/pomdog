// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix4x4.hpp"
#include "pomdog/math/radian.hpp"
#include "pomdog/math/vector3.hpp"
#include <catch_amalgamated.hpp>

using Pomdog::Matrix4x4;
using Pomdog::Vector3;

namespace {

static constexpr auto Epsilon = std::numeric_limits<float>::epsilon();

} // namespace

TEST_CASE("Matrix4x4", "[Matrix4x4]")
{
    SECTION("FirstTestCase")
    {
        Matrix4x4 matrix{
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        };
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(1.0f == matrix(0, 1));
        REQUIRE(1.0f == matrix(0, 2));
        REQUIRE(1.0f == matrix(0, 3));
        REQUIRE(1.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(1.0f == matrix(1, 2));
        REQUIRE(1.0f == matrix(1, 3));
        REQUIRE(1.0f == matrix(2, 0));
        REQUIRE(1.0f == matrix(2, 1));
        REQUIRE(1.0f == matrix(2, 2));
        REQUIRE(1.0f == matrix(2, 3));
        REQUIRE(1.0f == matrix(3, 0));
        REQUIRE(1.0f == matrix(3, 1));
        REQUIRE(1.0f == matrix(3, 2));
        REQUIRE(1.0f == matrix(3, 3));

        matrix = Matrix4x4{
            0.0f, 1.0f, 2.0f, 3.0f,
            4.0f, 5.0f, 6.0f, 7.0f,
            8.0f, 9.0f, 10.0f, 11.0f,
            12.0f, 13.0f, 14.0f, 15.0f
        };
        REQUIRE(0.0f == matrix(0, 0));
        REQUIRE(1.0f == matrix(0, 1));
        REQUIRE(2.0f == matrix(0, 2));
        REQUIRE(3.0f == matrix(0, 3));
        REQUIRE(4.0f == matrix(1, 0));
        REQUIRE(5.0f == matrix(1, 1));
        REQUIRE(6.0f == matrix(1, 2));
        REQUIRE(7.0f == matrix(1, 3));
        REQUIRE(8.0f == matrix(2, 0));
        REQUIRE(9.0f == matrix(2, 1));
        REQUIRE(10.0f == matrix(2, 2));
        REQUIRE(11.0f == matrix(2, 3));
        REQUIRE(12.0f == matrix(3, 0));
        REQUIRE(13.0f == matrix(3, 1));
        REQUIRE(14.0f == matrix(3, 2));
        REQUIRE(15.0f == matrix(3, 3));
    }
    SECTION("Identity")
    {
        REQUIRE(1.0f == Matrix4x4::Identity(0, 0));
        REQUIRE(0.0f == Matrix4x4::Identity(0, 1));
        REQUIRE(0.0f == Matrix4x4::Identity(0, 2));
        REQUIRE(0.0f == Matrix4x4::Identity(0, 3));
        REQUIRE(0.0f == Matrix4x4::Identity(1, 0));
        REQUIRE(1.0f == Matrix4x4::Identity(1, 1));
        REQUIRE(0.0f == Matrix4x4::Identity(1, 2));
        REQUIRE(0.0f == Matrix4x4::Identity(1, 3));
        REQUIRE(0.0f == Matrix4x4::Identity(2, 0));
        REQUIRE(0.0f == Matrix4x4::Identity(2, 1));
        REQUIRE(1.0f == Matrix4x4::Identity(2, 2));
        REQUIRE(0.0f == Matrix4x4::Identity(2, 3));
        REQUIRE(0.0f == Matrix4x4::Identity(3, 0));
        REQUIRE(0.0f == Matrix4x4::Identity(3, 1));
        REQUIRE(0.0f == Matrix4x4::Identity(3, 2));
        REQUIRE(1.0f == Matrix4x4::Identity(3, 3));

        Matrix4x4 matrix = Matrix4x4::Identity;
        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(0, 2));
        REQUIRE(0.0f == matrix(0, 3));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(1, 2));
        REQUIRE(0.0f == matrix(1, 3));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
        REQUIRE(1.0f == matrix(2, 2));
        REQUIRE(0.0f == matrix(2, 3));
        REQUIRE(0.0f == matrix(3, 0));
        REQUIRE(0.0f == matrix(3, 1));
        REQUIRE(0.0f == matrix(3, 2));
        REQUIRE(1.0f == matrix(3, 3));
    }
    SECTION("Multiply_Scalar")
    {
        REQUIRE(
            Matrix4x4(
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f) ==
            Matrix4x4::Multiply(Matrix4x4::Identity, 0.0f));
        REQUIRE(
            Matrix4x4::Identity ==
            Matrix4x4::Multiply(Matrix4x4::Identity, 1.0f));
        REQUIRE(
            Matrix4x4(
                4.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 4.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 4.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 4.0f) ==
            Matrix4x4::Multiply(Matrix4x4::Identity, 4.0f));
    }
    SECTION("Multiply_Matrix")
    {
        REQUIRE(
            Matrix4x4(
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f) ==
            Matrix4x4::Multiply(
                Matrix4x4::Identity,
                Matrix4x4(
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f)));

        REQUIRE(
            Matrix4x4::Identity ==
            Matrix4x4::Multiply(Matrix4x4::Identity, Matrix4x4::Identity));

        REQUIRE(
            Matrix4x4(
                84.0f, 134.0f, 324.0f, 139.0f,
                268.0f, 317.0f, 593.0f, 188.0f,
                222.0f, 301.0f, 649.0f, 276.0f,
                97.0f, 95.0f, 933.0f, 65.0f) ==
            Matrix4x4::Multiply(
                Matrix4x4(
                    3.0f, 9.0f, 4.0f, 2.0f,
                    2.0f, 7.0f, 19.0f, 6.0f,
                    14.0f, 5.0f, 11.0f, 8.0f,
                    2.0f, 1.0f, 3.0f, 17.0f),
                Matrix4x4(
                    4.0f, 8.0f, 4.0f, 12.0f,
                    2.0f, 6.0f, 20.0f, 9.0f,
                    12.0f, 13.0f, 7.0f, 5.0f,
                    3.0f, 2.0f, 52.0f, 1.0f)));
    }
    SECTION("CreateTranslation")
    {
        auto matrix = Matrix4x4::CreateTranslation({3.0f, 4.0, 5.0f});
        REQUIRE(3.0f == matrix(3, 0));
        REQUIRE(4.0f == matrix(3, 1));
        REQUIRE(5.0f == matrix(3, 2));
        REQUIRE(1.0f == matrix(3, 3));

        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(0, 2));
        REQUIRE(0.0f == matrix(0, 3));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(1, 2));
        REQUIRE(0.0f == matrix(1, 3));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
        REQUIRE(1.0f == matrix(2, 2));
        REQUIRE(0.0f == matrix(2, 3));
    }
    SECTION("CreateScale")
    {
        auto matrix = Matrix4x4::CreateScale({3.0f, 4.0, 5.0f});
        REQUIRE(3.0f == matrix(0, 0));
        REQUIRE(4.0f == matrix(1, 1));
        REQUIRE(5.0f == matrix(2, 2));
        REQUIRE(1.0f == matrix(3, 3));

        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(0, 2));
        REQUIRE(0.0f == matrix(0, 3));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(0.0f == matrix(1, 2));
        REQUIRE(0.0f == matrix(1, 3));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
        REQUIRE(0.0f == matrix(2, 3));
        REQUIRE(0.0f == matrix(3, 0));
        REQUIRE(0.0f == matrix(3, 1));
        REQUIRE(0.0f == matrix(3, 2));
    }
    SECTION("CreateRotationX")
    {
        auto matrix = Matrix4x4::CreateRotationX(3.0f);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(1, 1)) < Epsilon);
        REQUIRE(std::abs(std::sin(3.0f) - matrix(1, 2)) < Epsilon);
        REQUIRE(std::abs(-std::sin(3.0f) - matrix(2, 1)) < Epsilon);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(2, 2)) < Epsilon);

        REQUIRE(1.0f == matrix(0, 0));
        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(0, 2));
        REQUIRE(0.0f == matrix(0, 3));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(0.0f == matrix(1, 3));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 3));
        REQUIRE(0.0f == matrix(3, 0));
        REQUIRE(0.0f == matrix(3, 1));
        REQUIRE(0.0f == matrix(3, 2));
        REQUIRE(1.0f == matrix(3, 3));
    }
    SECTION("CreateRotationY")
    {
        auto matrix = Matrix4x4::CreateRotationY(3.0f);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(0, 0)) < Epsilon);
        REQUIRE(std::abs(-std::sin(3.0f) - matrix(0, 2)) < Epsilon);
        REQUIRE(std::abs(std::sin(3.0f) - matrix(2, 0)) < Epsilon);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(2, 2)) < Epsilon);

        REQUIRE(0.0f == matrix(0, 1));
        REQUIRE(0.0f == matrix(0, 3));
        REQUIRE(0.0f == matrix(1, 0));
        REQUIRE(1.0f == matrix(1, 1));
        REQUIRE(0.0f == matrix(1, 2));
        REQUIRE(0.0f == matrix(1, 3));
        REQUIRE(0.0f == matrix(2, 1));
        REQUIRE(0.0f == matrix(2, 3));
        REQUIRE(0.0f == matrix(3, 0));
        REQUIRE(0.0f == matrix(3, 1));
        REQUIRE(0.0f == matrix(3, 2));
        REQUIRE(1.0f == matrix(3, 3));
    }
    SECTION("CreateRotationZ")
    {
        auto matrix = Matrix4x4::CreateRotationZ(3.0f);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(0, 0)) < Epsilon);
        REQUIRE(std::abs(std::sin(3.0f) - matrix(0, 1)) < Epsilon);
        REQUIRE(std::abs(-std::sin(3.0f) - matrix(1, 0)) < Epsilon);
        REQUIRE(std::abs(std::cos(3.0f) - matrix(1, 1)) < Epsilon);

        REQUIRE(0.0f == matrix(0, 2));
        REQUIRE(0.0f == matrix(0, 3));
        REQUIRE(0.0f == matrix(1, 2));
        REQUIRE(0.0f == matrix(1, 3));
        REQUIRE(0.0f == matrix(2, 0));
        REQUIRE(0.0f == matrix(2, 1));
        REQUIRE(1.0f == matrix(2, 2));
        REQUIRE(0.0f == matrix(2, 3));
        REQUIRE(0.0f == matrix(3, 0));
        REQUIRE(0.0f == matrix(3, 1));
        REQUIRE(0.0f == matrix(3, 2));
        REQUIRE(1.0f == matrix(3, 3));
    }
    SECTION("Data")
    {
        Matrix4x4 matrix{
            0.0f, 1.0f, 2.0f, 3.0f,
            4.0f, 5.0f, 6.0f, 7.0f,
            8.0f, 9.0f, 10.0f, 11.0f,
            12.0f, 13.0f, 14.0f, 15.0f
        };
        REQUIRE(0.0f == *(matrix.Data() + 0));
        REQUIRE(1.0f == *(matrix.Data() + 1));
        REQUIRE(2.0f == *(matrix.Data() + 2));
        REQUIRE(3.0f == *(matrix.Data() + 3));
        REQUIRE(4.0f == *(matrix.Data() + 4));
        REQUIRE(5.0f == *(matrix.Data() + 5));
        REQUIRE(6.0f == *(matrix.Data() + 6));
        REQUIRE(7.0f == *(matrix.Data() + 7));
        REQUIRE(8.0f == *(matrix.Data() + 8));
        REQUIRE(9.0f == *(matrix.Data() + 9));
        REQUIRE(10.0f == *(matrix.Data() + 10));
        REQUIRE(11.0f == *(matrix.Data() + 11));
        REQUIRE(12.0f == *(matrix.Data() + 12));
        REQUIRE(13.0f == *(matrix.Data() + 13));
        REQUIRE(14.0f == *(matrix.Data() + 14));
        REQUIRE(15.0f == *(matrix.Data() + 15));
    }
}
