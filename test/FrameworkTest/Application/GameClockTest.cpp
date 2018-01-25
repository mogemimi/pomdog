// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

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
    auto prevTotalTime = clock.GetTotalGameTime();

    EXPECT_EQ(0, clock.GetFrameNumber());
    EXPECT_EQ(Duration::zero(), clock.GetTotalGameTime());
    EXPECT_NEAR(30.0f, clock.GetFrameRate(), Epsilon);
    EXPECT_EQ(Duration(1.0 / 30.0), clock.GetFrameDuration());
    EXPECT_LE(Duration::zero(), clock.GetElapsedTime());

    clock.Tick();
    EXPECT_EQ(1, clock.GetFrameNumber());
    EXPECT_LT(prevTotalTime, clock.GetTotalGameTime());
    EXPECT_LT(0.0f, clock.GetFrameRate());
    EXPECT_LT(Duration::zero(), clock.GetFrameDuration());
    EXPECT_LE(Duration::zero(), clock.GetElapsedTime());

    prevTotalTime = clock.GetTotalGameTime();
    std::this_thread::sleep_for(sleepTime);
    clock.Tick();

    EXPECT_EQ(2, clock.GetFrameNumber());
    EXPECT_LT(prevTotalTime, clock.GetTotalGameTime());
    EXPECT_LT(0.0f, clock.GetFrameRate());
    EXPECT_LT(Duration::zero(), clock.GetFrameDuration());
    EXPECT_LE(Duration::zero(), clock.GetElapsedTime());

    prevTotalTime = clock.GetTotalGameTime();
    std::this_thread::sleep_for(sleepTime);
    clock.Tick();

    EXPECT_EQ(3, clock.GetFrameNumber());
    EXPECT_LT(prevTotalTime, clock.GetTotalGameTime());
    EXPECT_LT(0.0f, clock.GetFrameRate());
    EXPECT_LT(Duration::zero(), clock.GetFrameDuration());
    EXPECT_LE(Duration::zero(), clock.GetElapsedTime());

    prevTotalTime = clock.GetTotalGameTime();
    std::this_thread::sleep_for(sleepTime);
    clock.Tick();

    EXPECT_EQ(4, clock.GetFrameNumber());
    EXPECT_LT(prevTotalTime, clock.GetTotalGameTime());
    EXPECT_LT(0.0f, clock.GetFrameRate());
    EXPECT_LT(Duration::zero(), clock.GetFrameDuration());
    EXPECT_LE(Duration::zero(), clock.GetElapsedTime());
}

TEST(GameClock, ElapsedTime)
{
    GameClock clock;
    const auto sleepTime = std::chrono::milliseconds(16);

    clock.Tick();
    auto prevElapsedTime = clock.GetElapsedTime();
    auto nextElapsedTime = clock.GetElapsedTime();
    EXPECT_LE(Duration::zero(), prevElapsedTime);
    EXPECT_LE(prevElapsedTime, nextElapsedTime);

    std::this_thread::sleep_for(sleepTime);
    nextElapsedTime = clock.GetElapsedTime();
    EXPECT_LT(prevElapsedTime, nextElapsedTime);
    std::swap(prevElapsedTime, nextElapsedTime);

    std::this_thread::sleep_for(sleepTime);
    nextElapsedTime = clock.GetElapsedTime();
    EXPECT_LT(prevElapsedTime, nextElapsedTime);
}
