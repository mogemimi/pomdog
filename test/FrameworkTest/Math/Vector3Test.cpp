// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include <Pomdog/Math/Vector3.hpp>
#include <gtest/iutest_switch.hpp>

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


