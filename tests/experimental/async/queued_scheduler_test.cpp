// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/experimental/async/queued_scheduler.h"
#include "pomdog/experimental/async/scheduler.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
#include <thread>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Duration;
using pomdog::concurrency::QueuedScheduler;

TEST_CASE("QueuedScheduler")
{
    SUBCASE("schedule simply")
    {
        std::vector<std::string> output;

        auto scheduler = std::make_shared<QueuedScheduler>();
        auto task = [&] { output.push_back("hello"); };
        scheduler->schedule(std::move(task), std::chrono::milliseconds(60));

        auto wait = [&](std::chrono::milliseconds millis) {
            std::this_thread::sleep_for(millis);
            scheduler->update();
        };

        REQUIRE(output.empty());
        wait(std::chrono::milliseconds(1));
        REQUIRE(output.empty());
        wait(std::chrono::milliseconds(2));
        REQUIRE(output.empty());
        wait(std::chrono::milliseconds(65));
        REQUIRE(output.size() == 1);
        REQUIRE(output.back() == "hello");
    }
    SUBCASE("schedule nested")
    {
        std::vector<std::string> output;

        auto scheduler = std::make_shared<QueuedScheduler>();
        scheduler->schedule([&] {
            output.push_back("ok");
            scheduler->schedule([&] { output.push_back("hello"); });
        });

        auto wait = [&](std::chrono::milliseconds millis) {
            std::this_thread::sleep_for(millis);
            scheduler->update();
        };

        REQUIRE(output.empty());
        wait(std::chrono::milliseconds(10));
        REQUIRE(output.size() == 1);
        REQUIRE(output.back() == "ok");
        wait(std::chrono::milliseconds(7));
        REQUIRE(output.size() == 2);
        REQUIRE(output.back() == "hello");
    }
    SUBCASE("schedule nested delay")
    {
        std::vector<std::string> output;

        auto scheduler = std::make_shared<QueuedScheduler>();
        scheduler->schedule([&] {
            output.push_back("ok");
            scheduler->schedule([&] { output.push_back("hello"); });
        },
            std::chrono::milliseconds(200));

        auto wait = [&](std::chrono::milliseconds millis) {
            std::this_thread::sleep_for(millis);
            scheduler->update();
        };

        REQUIRE(output.empty());
        wait(std::chrono::milliseconds(10));
        REQUIRE(output.empty());
        wait(std::chrono::milliseconds(240));
        REQUIRE(output.size() == 1);
        REQUIRE(output.back() == "ok");
        wait(std::chrono::milliseconds(10));
        REQUIRE(output.size() == 2);
        REQUIRE(output.back() == "hello");
    }
    SUBCASE("schedule nested delay 2")
    {
        std::vector<std::string> output;

        auto scheduler = std::make_shared<QueuedScheduler>();
        scheduler->schedule([&] {
            output.push_back("ok");
            scheduler->schedule(
                [&] { output.push_back("hello"); },
                std::chrono::milliseconds(30));
        },
            std::chrono::milliseconds(20));

        auto wait = [&](std::chrono::milliseconds millis) {
            std::this_thread::sleep_for(millis);
            scheduler->update();
        };

        REQUIRE(output.empty());
        wait(std::chrono::milliseconds(1));
        REQUIRE(output.empty());
        wait(std::chrono::milliseconds(20));
        REQUIRE(output.size() == 1);
        REQUIRE(output.back() == "ok");
        wait(std::chrono::milliseconds(1));
        REQUIRE(output.size() == 1);
        wait(std::chrono::milliseconds(30));
        REQUIRE(output.size() == 2);
        REQUIRE(output.back() == "hello");
    }
}
