// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/Matrix2x2.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::Matrix2x2;

TEST(Matrix2x2, FirstTestCase)
{
    Matrix2x2 matrix{1.0f, 1.0f, 1.0f, 1.0f};
    EXPECT_EQ(1.0f, matrix(0, 0));
    EXPECT_EQ(1.0f, matrix(0, 1));
    EXPECT_EQ(1.0f, matrix(1, 0));
    EXPECT_EQ(1.0f, matrix(1, 1));

    matrix = Matrix2x2{0.0f, 1.0f, 2.0f, 3.0f};
    EXPECT_EQ(0.0f, matrix(0, 0));
    EXPECT_EQ(1.0f, matrix(0, 1));
    EXPECT_EQ(2.0f, matrix(1, 0));
    EXPECT_EQ(3.0f, matrix(1, 1));
}

TEST(Matrix2x2, Identity)
{
    EXPECT_EQ(1.0f, Matrix2x2::Identity(0, 0));
    EXPECT_EQ(0.0f, Matrix2x2::Identity(0, 1));
    EXPECT_EQ(0.0f, Matrix2x2::Identity(1, 0));
    EXPECT_EQ(1.0f, Matrix2x2::Identity(1, 1));

    Matrix2x2 matrix = Matrix2x2::Identity;
    EXPECT_EQ(1.0f, matrix(0, 0));
    EXPECT_EQ(0.0f, matrix(0, 1));
    EXPECT_EQ(0.0f, matrix(1, 0));
    EXPECT_EQ(1.0f, matrix(1, 1));
}

TEST(Matrix2x2, Multiply_Scalar)
{
    EXPECT_EQ(
        Matrix2x2(0.0f, 0.0f, 0.0f, 0.0f),
        Matrix2x2::Multiply(Matrix2x2::Identity, 0.0f));
    EXPECT_EQ(
        Matrix2x2::Identity,
        Matrix2x2::Multiply(Matrix2x2::Identity, 1.0f));
    EXPECT_EQ(
        Matrix2x2(4.0f, 0.0f, 0.0f, 4.0f),
        Matrix2x2::Multiply(Matrix2x2::Identity, 4.0f));
}

TEST(Matrix2x2, Multiply_Matrix)
{
    EXPECT_EQ(
        Matrix2x2(1.0f, 0.0f, 0.0f, 1.0f),
        Matrix2x2::Multiply(
            Matrix2x2::Identity,
            Matrix2x2::Identity));

    EXPECT_EQ(
        Matrix2x2(0.0f, 0.0f, 0.0f, 0.0f),
        Matrix2x2::Multiply(
            Matrix2x2::Identity,
            Matrix2x2(0.0f, 0.0f, 0.0f, 0.0f)));

    EXPECT_EQ(
        Matrix2x2(157.0f, 752.0f, 156.0f, 176.0f),
        Matrix2x2::Multiply(
            Matrix2x2(5.0f, 61.0f, 20.0f, 8.0f),
            Matrix2x2(7.0f, 4.0f, 2.0f, 12.0f)));

    EXPECT_EQ(
        Matrix2x2(46.0f, 41.0f, 106.0f, 101.0f),
        Matrix2x2::Multiply(
            Matrix2x2(3.0f, 4.0f, 8.0f, 9.0f),
            Matrix2x2(2.0f, 7.0f, 10.0f, 5.0f)));
}

TEST(Matrix2x2, Data)
{
    Matrix2x2 matrix{0.0f, 1.0f, 2.0f, 3.0f};
    EXPECT_EQ(0.0f, *(matrix.Data() + 0));
    EXPECT_EQ(1.0f, *(matrix.Data() + 1));
    EXPECT_EQ(2.0f, *(matrix.Data() + 2));
    EXPECT_EQ(3.0f, *(matrix.Data() + 3));
}
