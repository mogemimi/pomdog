// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Radian.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::Matrix4x4;
using Pomdog::Vector3;

namespace {

static constexpr auto Epsilon = std::numeric_limits<float>::epsilon();

}// unused namespace

TEST(Matrix4x4, FirstTestCase)
{
    Matrix4x4 matrix {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };
    EXPECT_EQ(1.0f, matrix(0, 0));
    EXPECT_EQ(1.0f, matrix(0, 1));
    EXPECT_EQ(1.0f, matrix(0, 2));
    EXPECT_EQ(1.0f, matrix(0, 3));
    EXPECT_EQ(1.0f, matrix(1, 0));
    EXPECT_EQ(1.0f, matrix(1, 1));
    EXPECT_EQ(1.0f, matrix(1, 2));
    EXPECT_EQ(1.0f, matrix(1, 3));
    EXPECT_EQ(1.0f, matrix(2, 0));
    EXPECT_EQ(1.0f, matrix(2, 1));
    EXPECT_EQ(1.0f, matrix(2, 2));
    EXPECT_EQ(1.0f, matrix(2, 3));
    EXPECT_EQ(1.0f, matrix(3, 0));
    EXPECT_EQ(1.0f, matrix(3, 1));
    EXPECT_EQ(1.0f, matrix(3, 2));
    EXPECT_EQ(1.0f, matrix(3, 3));

    matrix = Matrix4x4 {
        0.0f, 1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f, 7.0f,
        8.0f, 9.0f, 10.0f, 11.0f,
        12.0f, 13.0f, 14.0f, 15.0f
    };
    EXPECT_EQ(0.0f, matrix(0, 0));
    EXPECT_EQ(1.0f, matrix(0, 1));
    EXPECT_EQ(2.0f, matrix(0, 2));
    EXPECT_EQ(3.0f, matrix(0, 3));
    EXPECT_EQ(4.0f, matrix(1, 0));
    EXPECT_EQ(5.0f, matrix(1, 1));
    EXPECT_EQ(6.0f, matrix(1, 2));
    EXPECT_EQ(7.0f, matrix(1, 3));
    EXPECT_EQ(8.0f, matrix(2, 0));
    EXPECT_EQ(9.0f, matrix(2, 1));
    EXPECT_EQ(10.0f, matrix(2, 2));
    EXPECT_EQ(11.0f, matrix(2, 3));
    EXPECT_EQ(12.0f, matrix(3, 0));
    EXPECT_EQ(13.0f, matrix(3, 1));
    EXPECT_EQ(14.0f, matrix(3, 2));
    EXPECT_EQ(15.0f, matrix(3, 3));
}

TEST(Matrix4x4, Identity)
{
    EXPECT_EQ(1.0f, Matrix4x4::Identity(0, 0));
    EXPECT_EQ(0.0f, Matrix4x4::Identity(0, 1));
    EXPECT_EQ(0.0f, Matrix4x4::Identity(0, 2));
    EXPECT_EQ(0.0f, Matrix4x4::Identity(0, 3));
    EXPECT_EQ(0.0f, Matrix4x4::Identity(1, 0));
    EXPECT_EQ(1.0f, Matrix4x4::Identity(1, 1));
    EXPECT_EQ(0.0f, Matrix4x4::Identity(1, 2));
    EXPECT_EQ(0.0f, Matrix4x4::Identity(1, 3));
    EXPECT_EQ(0.0f, Matrix4x4::Identity(2, 0));
    EXPECT_EQ(0.0f, Matrix4x4::Identity(2, 1));
    EXPECT_EQ(1.0f, Matrix4x4::Identity(2, 2));
    EXPECT_EQ(0.0f, Matrix4x4::Identity(2, 3));
    EXPECT_EQ(0.0f, Matrix4x4::Identity(3, 0));
    EXPECT_EQ(0.0f, Matrix4x4::Identity(3, 1));
    EXPECT_EQ(0.0f, Matrix4x4::Identity(3, 2));
    EXPECT_EQ(1.0f, Matrix4x4::Identity(3, 3));

    Matrix4x4 matrix = Matrix4x4::Identity;
    EXPECT_EQ(1.0f, matrix(0, 0));
    EXPECT_EQ(0.0f, matrix(0, 1));
    EXPECT_EQ(0.0f, matrix(0, 2));
    EXPECT_EQ(0.0f, matrix(0, 3));
    EXPECT_EQ(0.0f, matrix(1, 0));
    EXPECT_EQ(1.0f, matrix(1, 1));
    EXPECT_EQ(0.0f, matrix(1, 2));
    EXPECT_EQ(0.0f, matrix(1, 3));
    EXPECT_EQ(0.0f, matrix(2, 0));
    EXPECT_EQ(0.0f, matrix(2, 1));
    EXPECT_EQ(1.0f, matrix(2, 2));
    EXPECT_EQ(0.0f, matrix(2, 3));
    EXPECT_EQ(0.0f, matrix(3, 0));
    EXPECT_EQ(0.0f, matrix(3, 1));
    EXPECT_EQ(0.0f, matrix(3, 2));
    EXPECT_EQ(1.0f, matrix(3, 3));
}

TEST(Matrix4x4, Multiply_Scalar)
{
    EXPECT_EQ(
        Matrix4x4(
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f),
        Matrix4x4::Multiply(Matrix4x4::Identity, 0.0f));
    EXPECT_EQ(
        Matrix4x4::Identity,
        Matrix4x4::Multiply(Matrix4x4::Identity, 1.0f));
    EXPECT_EQ(
        Matrix4x4(
            4.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 4.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 4.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 4.0f),
        Matrix4x4::Multiply(Matrix4x4::Identity, 4.0f));
}

TEST(Matrix4x4, Multiply_Matrix)
{
    EXPECT_EQ(
        Matrix4x4(
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f),
        Matrix4x4::Multiply(
            Matrix4x4::Identity,
            Matrix4x4(
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f)));

    EXPECT_EQ(
        Matrix4x4::Identity,
        Matrix4x4::Multiply(Matrix4x4::Identity, Matrix4x4::Identity));

    EXPECT_EQ(
        Matrix4x4(
            84.0f, 134.0f, 324.0f, 139.0f,
            268.0f, 317.0f, 593.0f, 188.0f,
            222.0f, 301.0f, 649.0f, 276.0f,
            97.0f, 95.0f, 933.0f, 65.0f),
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

TEST(Matrix4x4, CreateTranslation)
{
    auto matrix = Matrix4x4::CreateTranslation({3.0f, 4.0, 5.0f});
    EXPECT_EQ(3.0f, matrix(3, 0));
    EXPECT_EQ(4.0f, matrix(3, 1));
    EXPECT_EQ(5.0f, matrix(3, 2));
    EXPECT_EQ(1.0f, matrix(3, 3));

    EXPECT_EQ(1.0f, matrix(0, 0));
    EXPECT_EQ(0.0f, matrix(0, 1));
    EXPECT_EQ(0.0f, matrix(0, 2));
    EXPECT_EQ(0.0f, matrix(0, 3));
    EXPECT_EQ(0.0f, matrix(1, 0));
    EXPECT_EQ(1.0f, matrix(1, 1));
    EXPECT_EQ(0.0f, matrix(1, 2));
    EXPECT_EQ(0.0f, matrix(1, 3));
    EXPECT_EQ(0.0f, matrix(2, 0));
    EXPECT_EQ(0.0f, matrix(2, 1));
    EXPECT_EQ(1.0f, matrix(2, 2));
    EXPECT_EQ(0.0f, matrix(2, 3));
}

TEST(Matrix4x4, CreateScale)
{
    auto matrix = Matrix4x4::CreateScale({3.0f, 4.0, 5.0f});
    EXPECT_EQ(3.0f, matrix(0, 0));
    EXPECT_EQ(4.0f, matrix(1, 1));
    EXPECT_EQ(5.0f, matrix(2, 2));
    EXPECT_EQ(1.0f, matrix(3, 3));

    EXPECT_EQ(0.0f, matrix(0, 1));
    EXPECT_EQ(0.0f, matrix(0, 2));
    EXPECT_EQ(0.0f, matrix(0, 3));
    EXPECT_EQ(0.0f, matrix(1, 0));
    EXPECT_EQ(0.0f, matrix(1, 2));
    EXPECT_EQ(0.0f, matrix(1, 3));
    EXPECT_EQ(0.0f, matrix(2, 0));
    EXPECT_EQ(0.0f, matrix(2, 1));
    EXPECT_EQ(0.0f, matrix(2, 3));
    EXPECT_EQ(0.0f, matrix(3, 0));
    EXPECT_EQ(0.0f, matrix(3, 1));
    EXPECT_EQ(0.0f, matrix(3, 2));
}

TEST(Matrix4x4, CreateRotationX)
{
    auto matrix = Matrix4x4::CreateRotationX(3.0f);
    EXPECT_NEAR(std::cos(3.0f), matrix(1, 1), Epsilon);
    EXPECT_NEAR(std::sin(3.0f), matrix(1, 2), Epsilon);
    EXPECT_NEAR(-std::sin(3.0f), matrix(2, 1), Epsilon);
    EXPECT_NEAR(std::cos(3.0f), matrix(2, 2), Epsilon);

    EXPECT_EQ(1.0f, matrix(0, 0));
    EXPECT_EQ(0.0f, matrix(0, 1));
    EXPECT_EQ(0.0f, matrix(0, 2));
    EXPECT_EQ(0.0f, matrix(0, 3));
    EXPECT_EQ(0.0f, matrix(1, 0));
    EXPECT_EQ(0.0f, matrix(1, 3));
    EXPECT_EQ(0.0f, matrix(2, 0));
    EXPECT_EQ(0.0f, matrix(2, 3));
    EXPECT_EQ(0.0f, matrix(3, 0));
    EXPECT_EQ(0.0f, matrix(3, 1));
    EXPECT_EQ(0.0f, matrix(3, 2));
    EXPECT_EQ(1.0f, matrix(3, 3));
}

TEST(Matrix4x4, CreateRotationY)
{
    auto matrix = Matrix4x4::CreateRotationY(3.0f);
    EXPECT_NEAR(std::cos(3.0f), matrix(0, 0), Epsilon);
    EXPECT_NEAR(-std::sin(3.0f), matrix(0, 2), Epsilon);
    EXPECT_NEAR(std::sin(3.0f), matrix(2, 0), Epsilon);
    EXPECT_NEAR(std::cos(3.0f), matrix(2, 2), Epsilon);

    EXPECT_EQ(0.0f, matrix(0, 1));
    EXPECT_EQ(0.0f, matrix(0, 3));
    EXPECT_EQ(0.0f, matrix(1, 0));
    EXPECT_EQ(1.0f, matrix(1, 1));
    EXPECT_EQ(0.0f, matrix(1, 2));
    EXPECT_EQ(0.0f, matrix(1, 3));
    EXPECT_EQ(0.0f, matrix(2, 1));
    EXPECT_EQ(0.0f, matrix(2, 3));
    EXPECT_EQ(0.0f, matrix(3, 0));
    EXPECT_EQ(0.0f, matrix(3, 1));
    EXPECT_EQ(0.0f, matrix(3, 2));
    EXPECT_EQ(1.0f, matrix(3, 3));
}

TEST(Matrix4x4, CreateRotationZ)
{
    auto matrix = Matrix4x4::CreateRotationZ(3.0f);
    EXPECT_NEAR(std::cos(3.0f), matrix(0, 0), Epsilon);
    EXPECT_NEAR(std::sin(3.0f), matrix(0, 1), Epsilon);
    EXPECT_NEAR(-std::sin(3.0f), matrix(1, 0), Epsilon);
    EXPECT_NEAR(std::cos(3.0f), matrix(1, 1), Epsilon);

    EXPECT_EQ(0.0f, matrix(0, 2));
    EXPECT_EQ(0.0f, matrix(0, 3));
    EXPECT_EQ(0.0f, matrix(1, 2));
    EXPECT_EQ(0.0f, matrix(1, 3));
    EXPECT_EQ(0.0f, matrix(2, 0));
    EXPECT_EQ(0.0f, matrix(2, 1));
    EXPECT_EQ(1.0f, matrix(2, 2));
    EXPECT_EQ(0.0f, matrix(2, 3));
    EXPECT_EQ(0.0f, matrix(3, 0));
    EXPECT_EQ(0.0f, matrix(3, 1));
    EXPECT_EQ(0.0f, matrix(3, 2));
    EXPECT_EQ(1.0f, matrix(3, 3));
}

TEST(Matrix4x4, Data)
{
    Matrix4x4 matrix {
        0.0f, 1.0f, 2.0f, 3.0f,
        4.0f, 5.0f, 6.0f, 7.0f,
        8.0f, 9.0f, 10.0f, 11.0f,
        12.0f, 13.0f, 14.0f, 15.0f
    };
    EXPECT_EQ(0.0f, *(matrix.Data() + 0));
    EXPECT_EQ(1.0f, *(matrix.Data() + 1));
    EXPECT_EQ(2.0f, *(matrix.Data() + 2));
    EXPECT_EQ(3.0f, *(matrix.Data() + 3));
    EXPECT_EQ(4.0f, *(matrix.Data() + 4));
    EXPECT_EQ(5.0f, *(matrix.Data() + 5));
    EXPECT_EQ(6.0f, *(matrix.Data() + 6));
    EXPECT_EQ(7.0f, *(matrix.Data() + 7));
    EXPECT_EQ(8.0f, *(matrix.Data() + 8));
    EXPECT_EQ(9.0f, *(matrix.Data() + 9));
    EXPECT_EQ(10.0f, *(matrix.Data() + 10));
    EXPECT_EQ(11.0f, *(matrix.Data() + 11));
    EXPECT_EQ(12.0f, *(matrix.Data() + 12));
    EXPECT_EQ(13.0f, *(matrix.Data() + 13));
    EXPECT_EQ(14.0f, *(matrix.Data() + 14));
    EXPECT_EQ(15.0f, *(matrix.Data() + 15));
}
