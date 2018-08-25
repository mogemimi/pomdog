// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Application/Timer.hpp>
#include <Pomdog/Application/GameClock.hpp>
#include <gtest/iutest_switch.hpp>
#include <chrono>
#include <thread>

using namespace Pomdog;

TEST(Timer, TrivialCase)
{
    GameClock clock;
    Timer timer(clock);

    EXPECT_TRUE(timer.IsEnabled());
    EXPECT_FALSE(timer.IsSingleShot());
    EXPECT_FALSE(timer.GetInterval());
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

    timer.SetInterval();
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

TEST(Timer, Scaling)
{
    constexpr double scale = 0.4;
    constexpr double epsilon = 0.001;

    GameClock clock;
    Timer timer(clock);
    timer.SetScale(scale);
    timer.Start();

    for (int i = 0; i < 100; i++) {
        clock.Tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ASSERT_EQ(scale, timer.GetScale());
    EXPECT_NEAR(clock.GetTotalGameTime().count() * scale, timer.GetTotalTime().count(), epsilon);
}
