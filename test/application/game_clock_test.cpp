// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/game_clock.hpp"
#include "catch_amalgamated.hpp"
#include <chrono>
#include <cmath>
#include <thread>

using namespace Pomdog;

TEST_CASE("GameClock", "[GameClock]")
{
    SECTION("Tick")
    {
        constexpr auto Epsilon = std::numeric_limits<float>::epsilon();

        const int framesPerSecond = 30;
        GameClock clock{framesPerSecond};
        const auto sleepTime = std::chrono::milliseconds(1);
        auto prevTotalTime = clock.GetTotalGameTime();

        REQUIRE(clock.GetFrameNumber() == 0);
        REQUIRE(clock.GetTotalGameTime() == Duration::zero());
        REQUIRE(std::abs(clock.GetFrameRate() - 30.0f) < Epsilon);
        REQUIRE(clock.GetFrameDuration() == Duration{1.0 / 30.0});
        REQUIRE(clock.GetElapsedTime() >= Duration::zero());

        clock.Tick();
        REQUIRE(clock.GetFrameNumber() == 1);
        REQUIRE(clock.GetTotalGameTime() > prevTotalTime);
        REQUIRE(clock.GetFrameRate() > 0.0f);
        REQUIRE(clock.GetFrameDuration() > Duration::zero());
        REQUIRE(clock.GetElapsedTime() >= Duration::zero());

        prevTotalTime = clock.GetTotalGameTime();
        std::this_thread::sleep_for(sleepTime);
        clock.Tick();

        REQUIRE(clock.GetFrameNumber() == 2);
        REQUIRE(clock.GetTotalGameTime() > prevTotalTime);
        REQUIRE(clock.GetFrameRate() > 0.0f);
        REQUIRE(clock.GetFrameDuration() > Duration::zero());
        REQUIRE(clock.GetElapsedTime() >= Duration::zero());

        prevTotalTime = clock.GetTotalGameTime();
        std::this_thread::sleep_for(sleepTime);
        clock.Tick();

        REQUIRE(clock.GetFrameNumber() == 3);
        REQUIRE(clock.GetTotalGameTime() > prevTotalTime);
        REQUIRE(clock.GetFrameRate() > 0.0f);
        REQUIRE(clock.GetFrameDuration() > Duration::zero());
        REQUIRE(clock.GetElapsedTime() >= Duration::zero());

        prevTotalTime = clock.GetTotalGameTime();
        std::this_thread::sleep_for(sleepTime);
        clock.Tick();

        REQUIRE(clock.GetFrameNumber() == 4);
        REQUIRE(clock.GetTotalGameTime() > prevTotalTime);
        REQUIRE(clock.GetFrameRate() > 0.0f);
        REQUIRE(clock.GetFrameDuration() > Duration::zero());
        REQUIRE(clock.GetElapsedTime() >= Duration::zero());
    }
    SECTION("ElapsedTime")
    {
        GameClock clock;
        const auto sleepTime = std::chrono::milliseconds(16);

        clock.Tick();
        auto prevElapsedTime = clock.GetElapsedTime();
        auto nextElapsedTime = clock.GetElapsedTime();
        REQUIRE(Duration::zero() <= prevElapsedTime);
        REQUIRE(prevElapsedTime <= nextElapsedTime);

        std::this_thread::sleep_for(sleepTime);
        nextElapsedTime = clock.GetElapsedTime();
        REQUIRE(prevElapsedTime < nextElapsedTime);
        std::swap(prevElapsedTime, nextElapsedTime);

        std::this_thread::sleep_for(sleepTime);
        nextElapsedTime = clock.GetElapsedTime();
        REQUIRE(prevElapsedTime < nextElapsedTime);
    }
}
