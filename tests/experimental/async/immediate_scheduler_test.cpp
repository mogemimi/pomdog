// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/experimental/async/immediate_scheduler.h"
#include "pomdog/experimental/async/scheduler.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <thread>
#include <vector>
#include <string>
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Duration;
using pomdog::concurrency::ImmediateScheduler;

TEST_CASE("ImmediateScheduler")
{
    SUBCASE("schedule simply")
    {
        auto scheduler = std::make_shared<ImmediateScheduler>();
        bool executed = false;

        scheduler->schedule([&executed]() {
            executed = true;
        }, Duration::zero());

        REQUIRE(executed);
    }
    SUBCASE("schedule multiple")
    {
        auto scheduler = std::make_shared<ImmediateScheduler>();
        int counter = 0;

        scheduler->schedule([&counter]() { counter += 1; }, Duration::zero());
        scheduler->schedule([&counter]() { counter += 2; }, Duration::zero());
        scheduler->schedule([&counter]() { counter += 3; }, Duration::zero());

        REQUIRE(counter == 6);
    }
    SUBCASE("schedule order")
    {
        auto scheduler = std::make_shared<ImmediateScheduler>();
        std::vector<int> order;

        scheduler->schedule([&order]() { order.push_back(1); }, Duration::zero());
        scheduler->schedule([&order]() { order.push_back(2); }, Duration::zero());
        scheduler->schedule([&order]() { order.push_back(3); }, Duration::zero());

        REQUIRE(order.size() == 3);
        REQUIRE(order[0] == 1);
        REQUIRE(order[1] == 2);
        REQUIRE(order[2] == 3);
    }
}
