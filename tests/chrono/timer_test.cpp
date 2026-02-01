// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/chrono/detail/game_clock_impl.h"
#include "pomdog/chrono/time_source.h"
#include "pomdog/chrono/timer.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Duration;
using pomdog::TimePoint;
using pomdog::Timer;
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

[[nodiscard]] std::pair<std::shared_ptr<MockTimeSource>, std::unique_ptr<GameClockImpl>>
createClockWithMockTime()
{
    auto timeSource = std::make_shared<MockTimeSource>();
    auto clock = std::make_unique<GameClockImpl>();
    [[maybe_unused]] auto err = clock->initialize(60, timeSource);
    return {timeSource, std::move(clock)};
}

} // namespace

TEST_CASE("Timer")
{
    SUBCASE("initial state")
    {
        auto [timeSource, clock] = createClockWithMockTime();
        Timer timer{*clock};

        REQUIRE(timer.isEnabled());
        REQUIRE_FALSE(timer.isSingleShot());

        // NOTE: getInterval() returns std::nullopt by default in Timer
        REQUIRE_FALSE(timer.getInterval().has_value());

        REQUIRE(timer.getTotalTime().count() == doctest::Approx(0.0).epsilon(0.0001));
    }
    SUBCASE("start")
    {
        auto [timeSource, clock] = createClockWithMockTime();
        Timer timer{*clock};

        REQUIRE(timer.isEnabled());
        timer.stop();
        REQUIRE_FALSE(timer.isEnabled());
        timer.start();
        REQUIRE(timer.isEnabled());
    }
    SUBCASE("stop")
    {
        auto [timeSource, clock] = createClockWithMockTime();
        Timer timer{*clock};

        REQUIRE(timer.isEnabled());
        timer.stop();
        REQUIRE_FALSE(timer.isEnabled());
    }
    SUBCASE("reset")
    {
        auto [timeSource, clock] = createClockWithMockTime();
        Timer timer{*clock};

        timer.setInterval(Duration{5.0});
        timer.setSingleShot(true);

        timeSource->advance(Duration{1.0 / 60.0});
        clock->tick();
        timeSource->advance(Duration{1.0 / 60.0});
        clock->tick();

        timer.reset();

        REQUIRE(timer.getTotalTime().count() == doctest::Approx(0.0).epsilon(0.0001));
        REQUIRE(timer.isEnabled());
        REQUIRE(timer.isSingleShot());
        REQUIRE(timer.getInterval().value_or(Duration{-1}).count() == doctest::Approx(5.0).epsilon(0.0001));
    }
    SUBCASE("Interval")
    {
        auto [timeSource, clock] = createClockWithMockTime();
        Timer timer{*clock};

        timer.setInterval(Duration{5.0});
        REQUIRE(timer.getInterval().value_or(Duration{-1}).count() == doctest::Approx(5.0).epsilon(0.0001));

        timer.setInterval(Duration{10.0});
        REQUIRE(timer.getInterval().value_or(Duration{-1}).count() == doctest::Approx(10.0).epsilon(0.0001));
    }
    SUBCASE("SingleShot")
    {
        auto [timeSource, clock] = createClockWithMockTime();
        Timer timer{*clock};

        REQUIRE_FALSE(timer.isSingleShot());
        timer.setSingleShot(true);
        REQUIRE(timer.isSingleShot());
        timer.setSingleShot(false);
        REQUIRE_FALSE(timer.isSingleShot());
    }
    SUBCASE("Scale")
    {
        auto [timeSource, clock] = createClockWithMockTime();
        Timer timer{*clock};

        REQUIRE(timer.getScale() == doctest::Approx(1.0).epsilon(0.0001));

        timer.setScale(2.0);
        REQUIRE(timer.getScale() == doctest::Approx(2.0).epsilon(0.0001));
        timeSource->advance(Duration{1.0 / 60.0});
        clock->tick();
        REQUIRE(timer.getTotalTime().count() == doctest::Approx((1.0 / 60.0) * 2.0).epsilon(0.00001));

        timer.setScale(0.5);
        REQUIRE(timer.getScale() == doctest::Approx(0.5).epsilon(0.0001));
    }
}
