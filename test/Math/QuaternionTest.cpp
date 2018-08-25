// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/Quaternion.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::Quaternion;

TEST(Quaternion, TrivialCase)
{
    Quaternion quaternion{0, 0, 0, 1};
    EXPECT_EQ(quaternion.X, 0.0f);
    EXPECT_EQ(quaternion.Y, 0.0f);
    EXPECT_EQ(quaternion.Z, 0.0f);
    EXPECT_EQ(quaternion.W, 1.0f);

    quaternion = {4, 3, 2, 1};
    EXPECT_EQ(quaternion.X, 4.0f);
    EXPECT_EQ(quaternion.Y, 3.0f);
    EXPECT_EQ(quaternion.Z, 2.0f);
    EXPECT_EQ(quaternion.W, 1.0f);
}

TEST(Quaternion, Constants)
{
    EXPECT_EQ(Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Quaternion::Identity);
}
