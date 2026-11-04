// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/network/connect_timeout.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <chrono>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Duration;
using pomdog::network::detail::splitConnectTimeout;

namespace {

[[nodiscard]] Duration
seconds(double count) noexcept
{
    return Duration{count};
}

} // namespace

TEST_CASE("splitConnectTimeout gives the last address all the time left")
{
    REQUIRE(splitConnectTimeout(seconds(5.0), 1).count() == doctest::Approx(5.0));
    REQUIRE(splitConnectTimeout(seconds(0.5), 1).count() == doctest::Approx(0.5));
}

TEST_CASE("splitConnectTimeout shares the time left evenly")
{
    REQUIRE(splitConnectTimeout(seconds(5.0), 2).count() == doctest::Approx(2.5));
    REQUIRE(splitConnectTimeout(seconds(30.0), 3).count() == doctest::Approx(10.0));
}

TEST_CASE("splitConnectTimeout gives each address at least two seconds while they remain")
{
    REQUIRE(splitConnectTimeout(seconds(5.0), 4).count() == doctest::Approx(2.0));
    REQUIRE(splitConnectTimeout(seconds(1.5), 4).count() == doctest::Approx(1.5));
}
