// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include <Pomdog/Application/Timer.hpp>
#include <Pomdog/Application/GameClock.hpp>
#include <gtest/iutest_switch.hpp>
#include <chrono>

using namespace Pomdog;

TEST(Timer, TrivialCase)
{
    GameClock clock;
    Timer timer(clock);

    EXPECT_TRUE(timer.IsEnabled());
    EXPECT_FALSE(timer.IsSingleShot());
    EXPECT_FALSE(timer.GetInterval());
    EXPECT_EQ(Duration::zero(), timer.GetFrameDuration());
    EXPECT_EQ(Duration::zero(), timer.GetTotalTime());
    EXPECT_EQ(1.0, timer.GetScale());
}

TEST(Timer, Interval)
{
    GameClock clock;
    Timer timer(clock);

    timer.SetInterval(std::chrono::milliseconds(10));
    ASSERT_TRUE(timer.GetInterval());
    EXPECT_EQ(std::chrono::milliseconds(10), *timer.GetInterval());

    timer.SetInterval(Pomdog::NullOpt);
    EXPECT_FALSE(timer.GetInterval());
}

TEST(Timer, Scale)
{
    GameClock clock;
    Timer timer(clock);

    timer.SetScale(0.5);
    EXPECT_EQ(0.5, timer.GetScale());

    timer.SetScale(0);
    EXPECT_EQ(0, timer.GetScale());

    timer.SetScale(-0.5);
    EXPECT_EQ(-0.5, timer.GetScale());
}
