// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::Matrix4x4;
using Pomdog::Vector3;

TEST(Vector3, TrivialCase)
{
    Vector3 vec {0, 0, 0};
    EXPECT_EQ(vec.X, 0.0f);
    EXPECT_EQ(vec.Y, 0.0f);
    EXPECT_EQ(vec.Z, 0.0f);

    vec = {1, 2, 3};
    EXPECT_EQ(vec.X, 1.0f);
    EXPECT_EQ(vec.Y, 2.0f);
    EXPECT_EQ(vec.Z, 3.0f);
}

TEST(Vector3, Constants)
{
    EXPECT_EQ(Vector3(1.0f, 0.0f, 0.0f), Vector3::UnitX);
    EXPECT_EQ(Vector3(0.0f, 1.0f, 0.0f), Vector3::UnitY);
    EXPECT_EQ(Vector3(0.0f, 0.0f, 1.0f), Vector3::UnitZ);
    EXPECT_EQ(Vector3(0.0f, 0.0f, 0.0f), Vector3::Zero);
}

TEST(Vector3, Addition)
{
    EXPECT_EQ(Vector3(1.0f + 4.0f, 2.0f + 5.0f, 3.0f + 6.0f), Vector3(1, 2, 3) + Vector3(4, 5, 6));
    EXPECT_EQ(Vector3(1.0f - 4.0f, 2.0f - 5.0f, 3.0f - 6.0f), Vector3(1, 2, 3) + Vector3(-4, -5, -6));
}

TEST(Vector3, Subtraction)
{
    EXPECT_EQ(Vector3(1.0f - 4.0f, 2.0f - 5.0f, 3.0f - 6.0f), Vector3(1, 2, 3) - Vector3(4, 5, 6));
    EXPECT_EQ(Vector3(4.0f - 1.0f, 5.0f - 2.0f, 6.0f - 3.0f), Vector3(4, 5, 6) - Vector3(1, 2, 3));
}

TEST(Vector3, Multiply)
{
    EXPECT_EQ(Vector3(4.0f * 3.0f, 5.0f * 3.0f, 6.0f * 3.0f), Vector3(4, 5, 6) * Vector3(3, 3, 3));
    EXPECT_EQ(Vector3(4.0f * 3.0f, 5.0f * 3.0f, 6.0f * 3.0f), Vector3(3, 3, 3) * Vector3(4, 5, 6));
    EXPECT_EQ(Vector3(4.0f * 3.0f, 5.0f * 3.0f, 6.0f * 3.0f), Vector3(4, 5, 6) * 3.0f);
    EXPECT_EQ(Vector3(4.0f * 3.0f, 5.0f * 3.0f, 6.0f * 3.0f), 3.0f * Vector3(4, 5, 6));
}

TEST(Vector3, Division)
{
    EXPECT_EQ(Vector3(4.0f/3.0f, 5.0f/3.0f, 6.0f/3.0f), Vector3(4, 5, 6) / Vector3(3, 3, 3));
    EXPECT_EQ(Vector3(4.0f/3.0f, 5.0f/3.0f, 6.0f/3.0f), Vector3(4, 5, 6) / 3.0f);
    EXPECT_EQ(Vector3(3.0f/4.0f, 3.0f/5.0f, 3.0f/6.0f), Vector3(3, 3, 3) / Vector3(4, 5, 6));
}

TEST(Vector3, Lerp)
{
    EXPECT_EQ(Vector3(0.0f, 0.0f, 0.0f), Vector3::Lerp(Vector3::Zero, Vector3(40.0f, 50.0f, 60.0f), 0.0f));
    EXPECT_EQ(Vector3(4.0f, 5.0f, 6.0f), Vector3::Lerp(Vector3::Zero, Vector3(40.0f, 50.0f, 60.0f), 0.1f));
    EXPECT_EQ(Vector3(36.f, 45.f, 54.f), Vector3::Lerp(Vector3::Zero, Vector3(40.0f, 50.0f, 60.0f), 0.9f));
    EXPECT_EQ(Vector3(40.f, 50.f, 60.f), Vector3::Lerp(Vector3::Zero, Vector3(40.0f, 50.0f, 60.0f), 1.0f));

    EXPECT_EQ(Vector3(1.0f, 2.0f, 3.0f), Vector3::Lerp(Vector3(1.0f, 2.0f, 3.0f), Vector3(4.0f, 5.0f, 6.0f), 0.0f));
    EXPECT_EQ(Vector3(2.5f, 3.5f, 4.5f), Vector3::Lerp(Vector3(1.0f, 2.0f, 3.0f), Vector3(4.0f, 5.0f, 6.0f), 0.5f));
    EXPECT_EQ(Vector3(4.0f, 5.0f, 6.0f), Vector3::Lerp(Vector3(1.0f, 2.0f, 3.0f), Vector3(4.0f, 5.0f, 6.0f), 1.0f));
}

TEST(Vector3, Cross_AssociativeLow)
{
    // NOTE: Associative low of vectors
    const Vector3 p = Vector3::UnitX;
    const Vector3 q = Vector3::UnitY;
    const Vector3 r = Vector3::UnitZ;

    EXPECT_EQ(3.0f * Vector3::Cross(Vector3::Zero, p), Vector3::Cross(Vector3::Zero, 3.0f * p));
    EXPECT_EQ(-7.0f * Vector3::Cross(Vector3::Zero, q), Vector3::Cross(Vector3::Zero, -7.0f * q));
    EXPECT_EQ(42.0f * Vector3::Cross(Vector3::Zero, r), Vector3::Cross(Vector3::Zero, 42.0f * r));

    const Vector3 a = {42.0f, 7.0f, 13.0f};
    const Vector3 b = {5.0f, -8.0f, 9.0f};
    const Vector3 c = {-81.0f, -63.0f, -2.0f};

    EXPECT_EQ(5.0f * Vector3::Cross(a, b), Vector3::Cross(5.0f * a, b));
    EXPECT_EQ(5.0f * Vector3::Cross(a, b), Vector3::Cross(a, 5.0f * b));
    EXPECT_EQ(-14.0f * Vector3::Cross(b, c), Vector3::Cross(-14.0f * b, c));
    EXPECT_EQ(-14.0f * Vector3::Cross(b, c), Vector3::Cross(b, -14.0f * c));
    EXPECT_EQ(-72.0f * Vector3::Cross(c, a), Vector3::Cross(-72.0f * c, a));
    EXPECT_EQ(-72.0f * Vector3::Cross(c, a), Vector3::Cross(c, -72.0f * a));
}

TEST(Vector3, Cross_CommutativeProperty)
{
    // NOTE: Commutative property of vectors
    EXPECT_EQ(Vector3::Zero, Vector3::Cross(Vector3::Zero, Vector3::Zero));

    EXPECT_EQ(-Vector3::Cross(Vector3::UnitX, Vector3::Zero), Vector3::Cross(Vector3::Zero, Vector3::UnitX));
    EXPECT_EQ(-Vector3::Cross(Vector3::UnitY, Vector3::Zero), Vector3::Cross(Vector3::Zero, Vector3::UnitY));
    EXPECT_EQ(-Vector3::Cross(Vector3::UnitZ, Vector3::Zero), Vector3::Cross(Vector3::Zero, Vector3::UnitZ));

    const Vector3 a = {42.0f, 7.0f, 13.0f};
    const Vector3 b = {5.0f, -8.0f, 9.0f};
    const Vector3 c = {-81.0f, -63.0f, -2.0f};

    EXPECT_EQ(-Vector3::Cross(a, b), Vector3::Cross(b, a));
    EXPECT_EQ(-Vector3::Cross(a, c), Vector3::Cross(c, a));
    EXPECT_EQ(-Vector3::Cross(b, c), Vector3::Cross(c, b));
}

TEST(Vector3, Transform_IdentityMatrix)
{
    EXPECT_EQ(Vector3(0.0f, 0.0f, 0.0f), Vector3::Transform(Vector3(0.0f, 0.0f, 0.0f), Matrix4x4::Identity));
    EXPECT_EQ(Vector3(41.0f, 42.0f, 43.0f), Vector3::Transform(Vector3(41.0f, 42.0f, 43.0f), Matrix4x4::Identity));
}
