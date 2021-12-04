// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Application/Timer.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "catch_amalgamated.hpp"
#include <chrono>
#include <cmath>
#include <thread>

using namespace Pomdog;

TEST_CASE("Timer", "[Timer]")
{
    GameClock clock;
    Timer timer(clock);

    SECTION("Timer")
    {
        REQUIRE(timer.IsEnabled());
        REQUIRE_FALSE(timer.IsSingleShot());
        REQUIRE_FALSE(timer.GetInterval());
        REQUIRE(timer.GetTotalTime() == Duration::zero());
        REQUIRE(timer.GetScale() == 1.0);
    }
    SECTION("Interval")
    {
        timer.SetInterval(std::chrono::milliseconds(10));
        REQUIRE(timer.GetInterval());
        REQUIRE(*timer.GetInterval() == std::chrono::milliseconds(10));

        timer.SetInterval();
        REQUIRE_FALSE(timer.GetInterval());
    }
    SECTION("Scale")
    {
        timer.SetScale(0.5);
        REQUIRE(timer.GetScale() == 0.5);

        timer.SetScale(0);
        REQUIRE(timer.GetScale() == 0);

        timer.SetScale(-0.5);
        REQUIRE(timer.GetScale() == -0.5);
    }
    SECTION("Scaling")
    {
        constexpr double scale = 0.4;
        constexpr double epsilon = 0.001;

        timer.SetScale(scale);
        timer.Start();

        for (int i = 0; i < 100; i++) {
            clock.Tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        REQUIRE(scale == timer.GetScale());
        REQUIRE(std::abs(clock.GetTotalGameTime().count() * scale - timer.GetTotalTime().count()) < epsilon);
    }
}
