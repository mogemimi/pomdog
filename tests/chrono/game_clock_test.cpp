// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/chrono/time_source.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <chrono>
#include <cmath>
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Duration;
using pomdog::TimePoint;
using pomdog::detail::GameClockImpl;
using pomdog::detail::TimeSource;

namespace {

class MockTimeSource final : public TimeSource {
public:
    TimePoint currentTime = TimePoint{Duration{0.0}};

    [[nodiscard]] TimePoint now() const noexcept override
    {
        return currentTime;
    }

    void advance(Duration delta)
    {
        currentTime = TimePoint{currentTime.time_since_epoch() + delta};
    }
};

} // namespace

TEST_CASE("GameClock")
{
    SUBCASE("tick")
    {
        auto timeSource = std::make_shared<MockTimeSource>();
        GameClockImpl clock;
        auto err = clock.initialize(60, timeSource);
        REQUIRE(err == nullptr);

        const auto before = clock.getTotalGameTime();
        timeSource->advance(Duration{1.0 / 60.0});
        clock.tick();
        const auto after = clock.getTotalGameTime();
        REQUIRE(before <= after);
    }
    SUBCASE("getFrameNumber")
    {
        auto timeSource = std::make_shared<MockTimeSource>();
        GameClockImpl clock;
        auto err = clock.initialize(60, timeSource);
        REQUIRE(err == nullptr);

        REQUIRE(clock.getFrameNumber() == 0);
        timeSource->advance(Duration{1.0 / 60.0});
        clock.tick();
        REQUIRE(clock.getFrameNumber() == 1);
        timeSource->advance(Duration{1.0 / 60.0});
        clock.tick();
        REQUIRE(clock.getFrameNumber() == 2);
        timeSource->advance(Duration{1.0 / 60.0});
        clock.tick();
        REQUIRE(clock.getFrameNumber() == 3);
    }
    SUBCASE("getFrameRate")
    {
        auto timeSource = std::make_shared<MockTimeSource>();
        GameClockImpl clock;
        auto err = clock.initialize(60, timeSource);
        REQUIRE(err == nullptr);

        timeSource->advance(Duration{1.0 / 60.0});
        clock.tick();
        REQUIRE(clock.getFrameRate() >= 0);
    }
    SUBCASE("getFrameDuration")
    {
        auto timeSource = std::make_shared<MockTimeSource>();
        GameClockImpl clock;
        auto err = clock.initialize(60, timeSource);
        REQUIRE(err == nullptr);

        timeSource->advance(Duration{1.0 / 60.0});
        clock.tick();
        REQUIRE(clock.getFrameDuration().count() >= 0.0);
    }
    SUBCASE("getTotalGameTime")
    {
        auto timeSource = std::make_shared<MockTimeSource>();
        GameClockImpl clock;
        auto err = clock.initialize(60, timeSource);
        REQUIRE(err == nullptr);

        REQUIRE(clock.getTotalGameTime().count() >= 0.0);
        timeSource->advance(Duration{1.0 / 60.0});
        clock.tick();
        REQUIRE(clock.getTotalGameTime().count() >= 0.0);
    }
    SUBCASE("getElapsedTime")
    {
        auto timeSource = std::make_shared<MockTimeSource>();
        GameClockImpl clock;
        auto err = clock.initialize(60, timeSource);
        REQUIRE(err == nullptr);

        const auto before = clock.getElapsedTime();
        REQUIRE(before.count() >= 0.0);
        timeSource->advance(Duration{1.0 / 60.0});
        const auto after = clock.getElapsedTime();
        REQUIRE(after.count() >= 0.0);
        REQUIRE(before <= after);
    }
    SUBCASE("restart")
    {
        auto timeSource = std::make_shared<MockTimeSource>();
        GameClockImpl clock;
        auto err = clock.initialize(60, timeSource);
        REQUIRE(err == nullptr);

        // NOTE: Advance time for 1 seconds
        for (int i = 0; i < 60; ++i) {
            timeSource->advance(Duration{1.0 / 60.0});
            clock.tick();
        }
        REQUIRE(clock.getTotalGameTime().count() >= doctest::Approx(1.0 / 60.0).epsilon(0.000001));

        clock.restart();
        REQUIRE(clock.getElapsedTime().count() == doctest::Approx(0.0).epsilon(0.000001));
        REQUIRE(clock.getTotalGameTime().count() >= doctest::Approx(1.0).epsilon(0.000001));
    }
}
