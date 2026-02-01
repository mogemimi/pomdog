// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/chrono/detail/make_time_source.h"
#include "pomdog/chrono/time_source.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::detail::makeTimeSource;

TEST_CASE("TimeSource")
{
    SUBCASE("now")
    {
        auto timeSource = makeTimeSource();
        REQUIRE(timeSource != nullptr);

        const auto time1 = timeSource->now();
        REQUIRE(time1.time_since_epoch().count() >= 0.0);

        // NOTE: Advance some time
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        const auto time2 = timeSource->now();
        REQUIRE(time2.time_since_epoch().count() >= time1.time_since_epoch().count());
    }
}
