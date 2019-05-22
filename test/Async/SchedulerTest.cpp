// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Async/ImmediateScheduler.hpp"
#include "Pomdog/Async/QueuedScheduler.hpp"
#include "catch.hpp"
#include <thread>

using Pomdog::Concurrency::QueuedScheduler;

TEST_CASE("Schedule_Simply", "[Scheduler]")
{
    std::vector<std::string> output;

    auto scheduler = std::make_shared<QueuedScheduler>();
    auto task = [&]{ output.push_back("hello"); };
    scheduler->Schedule(std::move(task), std::chrono::milliseconds(60));

    auto wait = [&](std::chrono::milliseconds millis) {
        std::this_thread::sleep_for(millis);
        scheduler->Update();
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

TEST_CASE("Schedule_Nested", "[Scheduler]")
{
    std::vector<std::string> output;

    auto scheduler = std::make_shared<QueuedScheduler>();
    scheduler->Schedule([&] {
        output.push_back("ok");
        scheduler->Schedule([&]{ output.push_back("hello"); });
    });

    auto wait = [&](std::chrono::milliseconds millis) {
        std::this_thread::sleep_for(millis);
        scheduler->Update();
    };

    REQUIRE(output.empty());
    wait(std::chrono::milliseconds(10));
    REQUIRE(output.size() == 1);
    REQUIRE(output.back() == "ok");
    wait(std::chrono::milliseconds(7));
    REQUIRE(output.size() == 2);
    REQUIRE(output.back() == "hello");
}

TEST_CASE("Schedule_NestedDelay", "[Scheduler]")
{
    std::vector<std::string> output;

    auto scheduler = std::make_shared<QueuedScheduler>();
    scheduler->Schedule([&] {
        output.push_back("ok");
        scheduler->Schedule([&]{ output.push_back("hello"); });
    }, std::chrono::milliseconds(20));

    auto wait = [&](std::chrono::milliseconds millis) {
        std::this_thread::sleep_for(millis);
        scheduler->Update();
    };

    REQUIRE(output.empty());
    wait(std::chrono::milliseconds(1));
    REQUIRE(output.empty());
    wait(std::chrono::milliseconds(24));
    REQUIRE(output.size() == 1);
    REQUIRE(output.back() == "ok");
    wait(std::chrono::milliseconds(1));
    REQUIRE(output.size() == 2);
    REQUIRE(output.back() == "hello");
}

TEST_CASE("Schedule_NestedDelay2", "[Scheduler]")
{
    std::vector<std::string> output;

    auto scheduler = std::make_shared<QueuedScheduler>();
    scheduler->Schedule([&] {
        output.push_back("ok");
        scheduler->Schedule(
            [&]{ output.push_back("hello"); },
            std::chrono::milliseconds(30));
    }, std::chrono::milliseconds(20));

    auto wait = [&](std::chrono::milliseconds millis) {
        std::this_thread::sleep_for(millis);
        scheduler->Update();
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
