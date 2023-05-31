// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/chrono/detail/make_time_source.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <catch_amalgamated.hpp>
#include <chrono>
#include <cmath>
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using namespace pomdog;

TEST_CASE("GameClock", "[GameClock]")
{
    SECTION("Tick")
    {
        constexpr auto Epsilon = std::numeric_limits<float>::epsilon();

        auto timeSource = detail::makeTimeSource();
        const int framesPerSecond = 30;
        detail::GameClockImpl clock;
        REQUIRE(clock.initialize(framesPerSecond, timeSource) == nullptr);

        const auto sleepTime = std::chrono::milliseconds(1);
        auto prevTotalTime = clock.getTotalGameTime();

        REQUIRE(clock.getFrameNumber() == 0);
        REQUIRE(clock.getTotalGameTime() == Duration::zero());
        REQUIRE(std::abs(clock.getFrameRate() - 30.0f) < Epsilon);
        REQUIRE(clock.getFrameDuration() == Duration{1.0 / 30.0});
        REQUIRE(clock.getElapsedTime() >= Duration::zero());

        clock.tick();
        REQUIRE(clock.getFrameNumber() == 1);
        REQUIRE(clock.getTotalGameTime() > prevTotalTime);
        REQUIRE(clock.getFrameRate() > 0.0f);
        REQUIRE(clock.getFrameDuration() > Duration::zero());
        REQUIRE(clock.getElapsedTime() >= Duration::zero());

        prevTotalTime = clock.getTotalGameTime();
        std::this_thread::sleep_for(sleepTime);
        clock.tick();

        REQUIRE(clock.getFrameNumber() == 2);
        REQUIRE(clock.getTotalGameTime() > prevTotalTime);
        REQUIRE(clock.getFrameRate() > 0.0f);
        REQUIRE(clock.getFrameDuration() > Duration::zero());
        REQUIRE(clock.getElapsedTime() >= Duration::zero());

        prevTotalTime = clock.getTotalGameTime();
        std::this_thread::sleep_for(sleepTime);
        clock.tick();

        REQUIRE(clock.getFrameNumber() == 3);
        REQUIRE(clock.getTotalGameTime() > prevTotalTime);
        REQUIRE(clock.getFrameRate() > 0.0f);
        REQUIRE(clock.getFrameDuration() > Duration::zero());
        REQUIRE(clock.getElapsedTime() >= Duration::zero());

        prevTotalTime = clock.getTotalGameTime();
        std::this_thread::sleep_for(sleepTime);
        clock.tick();

        REQUIRE(clock.getFrameNumber() == 4);
        REQUIRE(clock.getTotalGameTime() > prevTotalTime);
        REQUIRE(clock.getFrameRate() > 0.0f);
        REQUIRE(clock.getFrameDuration() > Duration::zero());
        REQUIRE(clock.getElapsedTime() >= Duration::zero());
    }
    SECTION("ElapsedTime")
    {
        auto timeSource = detail::makeTimeSource();
        const int framesPerSecond = 60;
        detail::GameClockImpl clock;
        REQUIRE(clock.initialize(framesPerSecond, timeSource) == nullptr);

        const auto sleepTime = std::chrono::milliseconds(16);

        clock.tick();
        auto prevElapsedTime = clock.getElapsedTime();
        auto nextElapsedTime = clock.getElapsedTime();
        REQUIRE(Duration::zero() <= prevElapsedTime);
        REQUIRE(prevElapsedTime <= nextElapsedTime);

        std::this_thread::sleep_for(sleepTime);
        nextElapsedTime = clock.getElapsedTime();
        REQUIRE(prevElapsedTime < nextElapsedTime);
        std::swap(prevElapsedTime, nextElapsedTime);

        std::this_thread::sleep_for(sleepTime);
        nextElapsedTime = clock.getElapsedTime();
        REQUIRE(prevElapsedTime < nextElapsedTime);
    }
}
