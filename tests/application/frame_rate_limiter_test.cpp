// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/application/backends/frame_rate_limiter.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/chrono/time_point.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Duration;
using pomdog::TimePoint;
using pomdog::detail::FrameRateLimiter;

namespace {

[[nodiscard]] TimePoint
at(double seconds) noexcept
{
    return TimePoint{Duration{seconds}};
}

} // namespace

TEST_CASE("FrameRateLimiter")
{
    constexpr double frame60 = 1.0 / 60.0;

    SUBCASE("is disabled by default and returns a zero sleep duration")
    {
        FrameRateLimiter limiter = {};
        CHECK_FALSE(limiter.isEnabled());
        CHECK(limiter.computeSleepDuration(at(0.0)).count() == doctest::Approx(0.0));
        CHECK(limiter.computeSleepDuration(at(100.0)).count() == doctest::Approx(0.0));
    }

    SUBCASE("a non-positive target frame rate disables limiting")
    {
        FrameRateLimiter limiter = {};

        limiter.setTargetFrameRate(60);
        CHECK(limiter.isEnabled());

        limiter.setTargetFrameRate(0);
        CHECK_FALSE(limiter.isEnabled());
        CHECK(limiter.computeSleepDuration(at(0.0)).count() == doctest::Approx(0.0));

        limiter.setTargetFrameRate(-5);
        CHECK_FALSE(limiter.isEnabled());
    }

    SUBCASE("holds a steady cadence when frames finish on time")
    {
        FrameRateLimiter limiter = {};
        limiter.setTargetFrameRate(60);

        // NOTE: The first frame anchors the deadline and sleeps one full frame.
        CHECK(limiter.computeSleepDuration(at(0.0)).count() == doctest::Approx(frame60));

        // NOTE: Each subsequent frame arriving exactly at the previous deadline
        // sleeps another full frame.
        CHECK(limiter.computeSleepDuration(at(frame60)).count() == doctest::Approx(frame60));
        CHECK(limiter.computeSleepDuration(at(2.0 * frame60)).count() == doctest::Approx(frame60));
    }

    SUBCASE("shortens the sleep by the work already spent in the frame")
    {
        FrameRateLimiter limiter = {};
        limiter.setTargetFrameRate(60);
        CHECK(limiter.computeSleepDuration(at(0.0)).count() == doctest::Approx(frame60));

        // NOTE: Frame 1 spent 0.005 s of work past the previous deadline.
        const auto sleepDuration = limiter.computeSleepDuration(at(frame60 + 0.005)).count();
        CHECK(sleepDuration == doctest::Approx(frame60 - 0.005));
    }

    SUBCASE("discards the backlog without a zero-sleep catch-up burst")
    {
        FrameRateLimiter limiter = {};
        limiter.setTargetFrameRate(60);
        CHECK(limiter.computeSleepDuration(at(0.0)).count() == doctest::Approx(frame60));

        // NOTE: Frame 1 finished 1.5 frames past the deadline. The debt-discarding
        // policy skips the missed slot and sleeps to the next grid point (~0.5
        // frame) instead of returning zero (which would start a catch-up burst).
        const auto sleepDuration = limiter.computeSleepDuration(at(2.5 * frame60)).count();
        CHECK(sleepDuration == doctest::Approx(0.5 * frame60));
        CHECK(sleepDuration > 0.0);

        // NOTE: The cadence resumes immediately at one full frame (no burst).
        CHECK(limiter.computeSleepDuration(at(3.0 * frame60)).count() == doctest::Approx(frame60));
    }

    SUBCASE("skips missed slots after a large stall instead of bursting")
    {
        FrameRateLimiter limiter = {};
        limiter.setTargetFrameRate(60);
        CHECK(limiter.computeSleepDuration(at(0.0)).count() == doctest::Approx(frame60));

        // NOTE: A ~10-second stall (debugger pause / OS sleep / GPU stall / window
        // drag). The missed slots are skipped and the deadline lands on the next
        // grid point after `now`, so the sleep is a single bounded frame in
        // (0, frame60] rather than a long backlog or a burst of catch-up frames.
        const auto sleepDuration = limiter.computeSleepDuration(at(10.0 + 0.005)).count();
        CHECK(sleepDuration > 0.0);
        CHECK(sleepDuration <= frame60 + 1e-9);
    }

    SUBCASE("re-anchors cleanly when the target frame rate changes")
    {
        FrameRateLimiter limiter = {};
        limiter.setTargetFrameRate(60);
        CHECK(limiter.computeSleepDuration(at(0.0)).count() == doctest::Approx(frame60));

        constexpr double frame30 = 1.0 / 30.0;
        limiter.setTargetFrameRate(30);

        // NOTE: The next tick re-anchors to `now` with the new frame duration;
        // no stale 60 fps deadline carries over.
        CHECK(limiter.computeSleepDuration(at(0.01)).count() == doctest::Approx(frame30));
    }
}
