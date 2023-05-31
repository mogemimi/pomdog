// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/chrono/detail/make_time_source.h"
#include "pomdog/chrono/timer.h"
#include "pomdog/utility/errors.h"
#include <catch_amalgamated.hpp>
#include <chrono>
#include <cmath>
#include <thread>

using namespace pomdog;

TEST_CASE("Timer", "[Timer]")
{
    auto timeSource = detail::makeTimeSource();
    detail::GameClockImpl clock;
    REQUIRE(clock.initialize(60, timeSource) == nullptr);

    Timer timer(clock);

    SECTION("Timer")
    {
        REQUIRE(timer.isEnabled());
        REQUIRE_FALSE(timer.isSingleShot());
        REQUIRE_FALSE(timer.getInterval());
        REQUIRE(timer.getTotalTime() == Duration::zero());
        REQUIRE(timer.getScale() == 1.0);
    }
    SECTION("Interval")
    {
        timer.setInterval(std::chrono::milliseconds(10));
        REQUIRE(timer.getInterval());
        REQUIRE(*timer.getInterval() == std::chrono::milliseconds(10));

        timer.setInterval();
        REQUIRE_FALSE(timer.getInterval());
    }
    SECTION("Scale")
    {
        timer.setScale(0.5);
        REQUIRE(timer.getScale() == 0.5);

        timer.setScale(0);
        REQUIRE(timer.getScale() == 0);

        timer.setScale(-0.5);
        REQUIRE(timer.getScale() == -0.5);
    }
    SECTION("Scaling")
    {
        constexpr double scale = 0.4;
        constexpr double epsilon = 0.001;

        timer.setScale(scale);
        timer.start();

        for (int i = 0; i < 100; i++) {
            clock.tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        REQUIRE(scale == timer.getScale());
        REQUIRE(std::abs(clock.getTotalGameTime().count() * scale - timer.getTotalTime().count()) < epsilon);
    }
}
