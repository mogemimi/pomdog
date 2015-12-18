// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include <Pomdog/Application/GameClock.hpp>
#include <gtest/iutest_switch.hpp>
#include <chrono>
#include <thread>

using namespace Pomdog;

namespace {

static constexpr auto Epsilon = std::numeric_limits<float>::epsilon();

}// unused namespace

TEST(GameClock, Tick)
{
    const int framesPerSecond = 30;
    GameClock clock{framesPerSecond};
    const auto sleepTime = std::chrono::milliseconds(1);
    auto prevTotalTime = clock.TotalGameTime();

    EXPECT_EQ(0, clock.FrameNumber());
    EXPECT_EQ(Duration::zero(), clock.TotalGameTime());
    EXPECT_NEAR(30.0f, clock.FrameRate(), Epsilon);
    EXPECT_EQ(Duration(1.0 / 30.0), clock.FrameDuration());
    EXPECT_LE(Duration::zero(), clock.ElapsedTime());

    clock.Tick();
    EXPECT_EQ(1, clock.FrameNumber());
    EXPECT_LT(prevTotalTime, clock.TotalGameTime());
    EXPECT_LT(0.0f, clock.FrameRate());
    EXPECT_LT(Duration::zero(), clock.FrameDuration());
    EXPECT_LE(Duration::zero(), clock.ElapsedTime());

    prevTotalTime = clock.TotalGameTime();
    std::this_thread::sleep_for(sleepTime);
    clock.Tick();

    EXPECT_EQ(2, clock.FrameNumber());
    EXPECT_LT(prevTotalTime, clock.TotalGameTime());
    EXPECT_LT(0.0f, clock.FrameRate());
    EXPECT_LT(Duration::zero(), clock.FrameDuration());
    EXPECT_LE(Duration::zero(), clock.ElapsedTime());

    prevTotalTime = clock.TotalGameTime();
    std::this_thread::sleep_for(sleepTime);
    clock.Tick();

    EXPECT_EQ(3, clock.FrameNumber());
    EXPECT_LT(prevTotalTime, clock.TotalGameTime());
    EXPECT_LT(0.0f, clock.FrameRate());
    EXPECT_LT(Duration::zero(), clock.FrameDuration());
    EXPECT_LE(Duration::zero(), clock.ElapsedTime());

    prevTotalTime = clock.TotalGameTime();
    std::this_thread::sleep_for(sleepTime);
    clock.Tick();

    EXPECT_EQ(4, clock.FrameNumber());
    EXPECT_LT(prevTotalTime, clock.TotalGameTime());
    EXPECT_LT(0.0f, clock.FrameRate());
    EXPECT_LT(Duration::zero(), clock.FrameDuration());
    EXPECT_LE(Duration::zero(), clock.ElapsedTime());
}

TEST(GameClock, ElapsedTime)
{
    GameClock clock;
    const auto sleepTime = std::chrono::milliseconds(16);

    clock.Tick();
    auto prevElapsedTime = clock.ElapsedTime();
    auto nextElapsedTime = clock.ElapsedTime();
    EXPECT_LE(Duration::zero(), prevElapsedTime);
    EXPECT_LE(prevElapsedTime, nextElapsedTime);

    std::this_thread::sleep_for(sleepTime);
    nextElapsedTime = clock.ElapsedTime();
    EXPECT_LT(prevElapsedTime, nextElapsedTime);
    std::swap(prevElapsedTime, nextElapsedTime);

    std::this_thread::sleep_for(sleepTime);
    nextElapsedTime = clock.ElapsedTime();
    EXPECT_LT(prevElapsedTime, nextElapsedTime);
}
