// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/MathHelper.hpp>
#include <gtest/iutest_switch.hpp>

namespace MathHelper = Pomdog::MathHelper;
using Pomdog::Degree;
using Pomdog::Radian;

TEST(MathHelper, Clamp)
{
    EXPECT_EQ(0.0, MathHelper::Clamp(std::numeric_limits<double>::lowest(), 0.0, 1.0));
    EXPECT_EQ(0.0, MathHelper::Clamp(0.0, 0.0, 1.0));
    EXPECT_EQ(0.5, MathHelper::Clamp(0.5, 0.0, 1.0));
    EXPECT_EQ(1.0, MathHelper::Clamp(1.0, 0.0, 1.0));
    EXPECT_EQ(1.0, MathHelper::Clamp(std::numeric_limits<double>::max(), 0.0, 1.0));

    EXPECT_EQ(0.0f, MathHelper::Clamp(std::numeric_limits<float>::lowest(), 0.0f, 1.0f));
    EXPECT_EQ(0.0f, MathHelper::Clamp(0.0f, 0.0f, 1.0f));
    EXPECT_EQ(0.5f, MathHelper::Clamp(0.5f, 0.0f, 1.0f));
    EXPECT_EQ(1.0f, MathHelper::Clamp(1.0f, 0.0f, 1.0f));
    EXPECT_EQ(1.0f, MathHelper::Clamp(std::numeric_limits<float>::max(), 0.0f, 1.0f));

    EXPECT_EQ(-4.0f, MathHelper::Clamp(-4.3f, -4.0f, 5.0f));
    EXPECT_EQ(-2.5f, MathHelper::Clamp(-2.5f, -4.0f, 5.0f));
    EXPECT_EQ(0.0f, MathHelper::Clamp(0.0f, -4.0f, 5.0f));
    EXPECT_EQ(3.5f, MathHelper::Clamp(3.5f, -4.0f, 5.0f));
    EXPECT_EQ(5.0f, MathHelper::Clamp(5.7f, -4.0f, 5.0f));
}

TEST(MathHelper, Saturate)
{
    EXPECT_EQ(0.0, MathHelper::Saturate(std::numeric_limits<double>::lowest()));
    EXPECT_EQ(0.0, MathHelper::Saturate(-0.1));
    EXPECT_EQ(0.0, MathHelper::Saturate(0.0));
    EXPECT_EQ(0.1, MathHelper::Saturate(0.1));
    EXPECT_EQ(0.5, MathHelper::Saturate(0.5));
    EXPECT_EQ(0.9, MathHelper::Saturate(0.9));
    EXPECT_EQ(1.0, MathHelper::Saturate(1.0));
    EXPECT_EQ(1.0, MathHelper::Saturate(1.1));
    EXPECT_EQ(1.0, MathHelper::Saturate(std::numeric_limits<double>::max()));

    EXPECT_EQ(0.0f, MathHelper::Saturate(std::numeric_limits<float>::lowest()));
    EXPECT_EQ(0.0f, MathHelper::Saturate(-0.1f));
    EXPECT_EQ(0.0f, MathHelper::Saturate(0.0f));
    EXPECT_EQ(0.1f, MathHelper::Saturate(0.1f));
    EXPECT_EQ(0.5f, MathHelper::Saturate(0.5f));
    EXPECT_EQ(0.9f, MathHelper::Saturate(0.9f));
    EXPECT_EQ(1.0f, MathHelper::Saturate(1.0f));
    EXPECT_EQ(1.0f, MathHelper::Saturate(1.1f));
    EXPECT_EQ(1.0f, MathHelper::Saturate(std::numeric_limits<float>::max()));
}

TEST(MathHelper, Lerp)
{
    constexpr auto epsilon = std::numeric_limits<float>::epsilon();

    EXPECT_NEAR(0.0f, MathHelper::Lerp(0.0f, 1.0f, 0.0f), epsilon);
    EXPECT_NEAR(0.2f, MathHelper::Lerp(0.0f, 1.0f, 0.2f), epsilon);
    EXPECT_NEAR(0.5f, MathHelper::Lerp(0.0f, 1.0f, 0.5f), epsilon);
    EXPECT_NEAR(0.8f, MathHelper::Lerp(0.0f, 1.0f, 0.8f), epsilon);
    EXPECT_NEAR(1.0f, MathHelper::Lerp(0.0f, 1.0f, 1.0f), epsilon);

    EXPECT_NEAR(-1.0f, MathHelper::Lerp(-1.0f, 1.0f, 0.0f), epsilon);
    EXPECT_NEAR(-0.6f, MathHelper::Lerp(-1.0f, 1.0f, 0.2f), epsilon);
    EXPECT_NEAR(0.0f, MathHelper::Lerp(-1.0f, 1.0f, 0.5f), epsilon);
    EXPECT_NEAR(0.6f, MathHelper::Lerp(-1.0f, 1.0f, 0.8f), epsilon);
    EXPECT_NEAR(1.0f, MathHelper::Lerp(-1.0f, 1.0f, 1.0f), epsilon);

    EXPECT_NEAR(-0.5f, MathHelper::Lerp(-0.5f, 0.5f, 0.0f), epsilon);
    EXPECT_NEAR(-0.3f, MathHelper::Lerp(-0.5f, 0.5f, 0.2f), epsilon);
    EXPECT_NEAR(0.0f, MathHelper::Lerp(-0.5f, 0.5f, 0.5f), epsilon);
    EXPECT_NEAR(0.3f, MathHelper::Lerp(-0.5f, 0.5f, 0.8f), epsilon);
    EXPECT_NEAR(0.5f, MathHelper::Lerp(-0.5f, 0.5f, 1.0f), epsilon);

    EXPECT_LT(MathHelper::Lerp(0.0f, 1.0f, 0.0f), MathHelper::Lerp(0.0f, 1.0f, 0.1f));
    EXPECT_LT(MathHelper::Lerp(0.0f, 1.0f, 0.1f), MathHelper::Lerp(0.0f, 1.0f, 0.2f));
    EXPECT_LT(MathHelper::Lerp(0.0f, 1.0f, 0.2f), MathHelper::Lerp(0.0f, 1.0f, 0.3f));
    EXPECT_LT(MathHelper::Lerp(0.0f, 1.0f, 0.3f), MathHelper::Lerp(0.0f, 1.0f, 0.4f));
    EXPECT_LT(MathHelper::Lerp(0.0f, 1.0f, 0.4f), MathHelper::Lerp(0.0f, 1.0f, 0.5f));
    EXPECT_LT(MathHelper::Lerp(0.0f, 1.0f, 0.5f), MathHelper::Lerp(0.0f, 1.0f, 0.6f));
    EXPECT_LT(MathHelper::Lerp(0.0f, 1.0f, 0.6f), MathHelper::Lerp(0.0f, 1.0f, 0.7f));
    EXPECT_LT(MathHelper::Lerp(0.0f, 1.0f, 0.7f), MathHelper::Lerp(0.0f, 1.0f, 0.8f));
    EXPECT_LT(MathHelper::Lerp(0.0f, 1.0f, 0.8f), MathHelper::Lerp(0.0f, 1.0f, 0.9f));
    EXPECT_LT(MathHelper::Lerp(0.0f, 1.0f, 0.9f), MathHelper::Lerp(0.0f, 1.0f, 1.0f));
}

TEST(MathHelper, SmoothStep)
{
    constexpr auto epsilon = std::numeric_limits<float>::epsilon();

    EXPECT_NEAR(0.0f, MathHelper::SmoothStep(0.0f, 1.0f, 0.0f), epsilon);
    EXPECT_NEAR(0.5f, MathHelper::SmoothStep(0.0f, 1.0f, 0.5f), epsilon);
    EXPECT_NEAR(1.0f, MathHelper::SmoothStep(0.0f, 1.0f, 1.0f), epsilon);

    EXPECT_NEAR(-1.0f, MathHelper::SmoothStep(-1.0f, 1.0f, 0.0f), epsilon);
    EXPECT_NEAR(0.0f, MathHelper::SmoothStep(-1.0f, 1.0f, 0.5f), epsilon);
    EXPECT_NEAR(1.0f, MathHelper::SmoothStep(-1.0f, 1.0f, 1.0f), epsilon);

    EXPECT_NEAR(-0.5f, MathHelper::SmoothStep(-0.5f, 0.5f, 0.0f), epsilon);
    EXPECT_NEAR(0.0f, MathHelper::SmoothStep(-0.5f, 0.5f, 0.5f), epsilon);
    EXPECT_NEAR(0.5f, MathHelper::SmoothStep(-0.5f, 0.5f, 1.0f), epsilon);

    EXPECT_LT(MathHelper::SmoothStep(0.0f, 1.0f, 0.0f), MathHelper::SmoothStep(0.0f, 1.0f, 0.1f));
    EXPECT_LT(MathHelper::SmoothStep(0.0f, 1.0f, 0.1f), MathHelper::SmoothStep(0.0f, 1.0f, 0.2f));
    EXPECT_LT(MathHelper::SmoothStep(0.0f, 1.0f, 0.2f), MathHelper::SmoothStep(0.0f, 1.0f, 0.3f));
    EXPECT_LT(MathHelper::SmoothStep(0.0f, 1.0f, 0.3f), MathHelper::SmoothStep(0.0f, 1.0f, 0.4f));
    EXPECT_LT(MathHelper::SmoothStep(0.0f, 1.0f, 0.4f), MathHelper::SmoothStep(0.0f, 1.0f, 0.5f));
    EXPECT_LT(MathHelper::SmoothStep(0.0f, 1.0f, 0.5f), MathHelper::SmoothStep(0.0f, 1.0f, 0.6f));
    EXPECT_LT(MathHelper::SmoothStep(0.0f, 1.0f, 0.6f), MathHelper::SmoothStep(0.0f, 1.0f, 0.7f));
    EXPECT_LT(MathHelper::SmoothStep(0.0f, 1.0f, 0.7f), MathHelper::SmoothStep(0.0f, 1.0f, 0.8f));
    EXPECT_LT(MathHelper::SmoothStep(0.0f, 1.0f, 0.8f), MathHelper::SmoothStep(0.0f, 1.0f, 0.9f));
    EXPECT_LT(MathHelper::SmoothStep(0.0f, 1.0f, 0.9f), MathHelper::SmoothStep(0.0f, 1.0f, 1.0f));
}

TEST(MathHelper, ToDegrees)
{
    constexpr auto epsilon = 0.0000001f;
    EXPECT_NEAR(0.0f, MathHelper::ToDegrees(0.0f).value, epsilon);
    EXPECT_NEAR(45.0f, MathHelper::ToDegrees(0.7853981633f).value, epsilon);
    EXPECT_NEAR(90.0f, MathHelper::ToDegrees(1.5707963267f).value, epsilon);
    EXPECT_NEAR(180.0f, MathHelper::ToDegrees(3.1415926535f).value, epsilon);
    EXPECT_NEAR(360.0f, MathHelper::ToDegrees(6.2831853071f).value, epsilon);

    EXPECT_NEAR(0.0f, MathHelper::ToDegrees(Radian<float>(0.0f)).value, epsilon);
    EXPECT_NEAR(45.0f, MathHelper::ToDegrees(Radian<float>(0.7853981633f)).value, epsilon);
    EXPECT_NEAR(90.0f, MathHelper::ToDegrees(Radian<float>(1.5707963267f)).value, epsilon);
    EXPECT_NEAR(180.0f, MathHelper::ToDegrees(Radian<float>(3.1415926535f)).value, epsilon);
    EXPECT_NEAR(360.0f, MathHelper::ToDegrees(Radian<float>(6.2831853071f)).value, epsilon);
}

TEST(MathHelper, ToRadians)
{
    constexpr auto epsilon = 0.0000005f;
    EXPECT_NEAR(0.0f, MathHelper::ToRadians(0.0f).value, epsilon);
    EXPECT_NEAR(0.7853981633f, MathHelper::ToRadians(45.0f).value, epsilon);
    EXPECT_NEAR(1.5707963267f, MathHelper::ToRadians(90.0f).value, epsilon);
    EXPECT_NEAR(3.1415926535f, MathHelper::ToRadians(180.0f).value, epsilon);
    EXPECT_NEAR(6.2831853071f, MathHelper::ToRadians(360.0f).value, epsilon);

    EXPECT_NEAR(0.0f, MathHelper::ToRadians(Degree<float>(0.0f)).value, epsilon);
    EXPECT_NEAR(0.7853981633f, MathHelper::ToRadians(Degree<float>(45.0f)).value, epsilon);
    EXPECT_NEAR(1.5707963267f, MathHelper::ToRadians(Degree<float>(90.0f)).value, epsilon);
    EXPECT_NEAR(3.1415926535f, MathHelper::ToRadians(Degree<float>(180.0f)).value, epsilon);
    EXPECT_NEAR(6.2831853071f, MathHelper::ToRadians(Degree<float>(360.0f)).value, epsilon);
}
