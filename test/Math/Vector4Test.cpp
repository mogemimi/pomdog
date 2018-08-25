// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/Vector4.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::Matrix4x4;
using Pomdog::Vector4;

TEST(Vector4, TrivialCase)
{
    Vector4 vec {0, 0, 0, 0};
    EXPECT_EQ(vec.X, 0.0f);
    EXPECT_EQ(vec.Y, 0.0f);
    EXPECT_EQ(vec.Z, 0.0f);
    EXPECT_EQ(vec.W, 0.0f);

    vec = {1, 2, 3, 4};
    EXPECT_EQ(vec.X, 1.0f);
    EXPECT_EQ(vec.Y, 2.0f);
    EXPECT_EQ(vec.Z, 3.0f);
    EXPECT_EQ(vec.W, 4.0f);
}

TEST(Vector4, Addition)
{
    EXPECT_EQ(Vector4(1.f + 5.f, 2.f + 6.f, 3.f + 7.f, 4.f + 8.f), Vector4(1, 2, 3, 4) + Vector4(5, 6, 7, 8));
    EXPECT_EQ(Vector4(1.f - 5.f, 2.f - 6.f, 3.f - 7.f, 4.f - 8.f), Vector4(1, 2, 3, 4) + Vector4(-5, -6, -7, -8));
}

TEST(Vector4, Subtraction)
{
    EXPECT_EQ(Vector4(1.f - 5.f, 2.f - 6.f, 3.f - 7.f, 4.f - 8.f), Vector4(1, 2, 3, 4) - Vector4(5, 6, 7, 8));
    EXPECT_EQ(Vector4(1.f + 5.f, 2.f + 6.f, 3.f + 7.f, 4.f + 8.f), Vector4(1, 2, 3, 4) - Vector4(-5, -6, -7, -8));
}

TEST(Vector4, Multiply)
{
    EXPECT_EQ(Vector4(4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f), Vector4(4, 5, 6, 7) * Vector4(3, 3, 3, 3));
    EXPECT_EQ(Vector4(4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f), Vector4(3, 3, 3, 3) * Vector4(4, 5, 6, 7));
    EXPECT_EQ(Vector4(4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f), Vector4(4, 5, 6, 7) * 3);
    EXPECT_EQ(Vector4(4.f * 3.f, 5.f * 3.f, 6.f * 3.f, 7.f * 3.f), 3.0f * Vector4(4, 5, 6, 7));
}

TEST(Vector4, Division)
{
    EXPECT_EQ(Vector4(4.f / 3.f, 5.f / 3.f, 6.f / 3.f, 7.f / 3.f), Vector4(4, 5, 6, 7) / Vector4(3, 3, 3, 3));
    EXPECT_EQ(Vector4(3.f / 4.f, 3.f / 5.f, 3.f / 6.f, 3.f / 7.f), Vector4(3, 3, 3, 3) / Vector4(4, 5, 6, 7));
    EXPECT_EQ(Vector4(4.f / 3.f, 5.f / 3.f, 6.f / 3.f, 7.f / 3.f), Vector4(4, 5, 6, 7) / 3.0f);
}

TEST(Vector4, Transform_IdentityMatrix)
{
    EXPECT_EQ(Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4::Transform(Vector4(0.0f, 0.0f, 0.0f, 0.0f), Matrix4x4::Identity));
    EXPECT_EQ(Vector4(1.0f, 2.0f, 3.0f, 4.0f), Vector4::Transform(Vector4(1.0f, 2.0f, 3.0f, 4.0f), Matrix4x4::Identity));
}

TEST(Vector4, Transform)
{
    EXPECT_EQ(Vector4(26.0f, 37.0f, 48.0f, 59.0f), Vector4::Transform(Vector4(26.0f, 37.0f, 48.0f, 59.0f), Matrix4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f)));

    EXPECT_EQ(Vector4(52.0f, 74.0f, 96.0f, 59.0f), Vector4::Transform(Vector4(26.0f, 37.0f, 48.0f, 59.0f), Matrix4x4(
        2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f)));

    EXPECT_EQ(Vector4(52.0f, 74.0f, 96.0f, 4588.0f), Vector4::Transform(Vector4(26.0f, 37.0f, 48.0f, 59.0f), Matrix4x4(
        2.0f, 0.0f, 0.0f, 42.0f,
        0.0f, 2.0f, 0.0f, 41.0f,
        0.0f, 0.0f, 2.0f, 40.0f,
        0.0f, 0.0f, 0.0f, 1.0f)));

    EXPECT_EQ(Vector4(388.0f, 74.0f, 304.0f, 4588.0f), Vector4::Transform(Vector4(26.0f, 37.0f, 48.0f, 59.0f), Matrix4x4(
        2.0f, 0.0f, 8.0f, 42.0f,
        0.0f, 2.0f, 0.0f, 41.0f,
        7.0f, 0.0f, 2.0f, 40.0f,
        0.0f, 0.0f, 0.0f, 1.0f)));
}
