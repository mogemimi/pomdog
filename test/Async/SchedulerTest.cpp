// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Async/ImmediateScheduler.hpp>
#include <Pomdog/Async/QueuedScheduler.hpp>
#include <gtest/iutest_switch.hpp>
#include <thread>

using Pomdog::Concurrency::QueuedScheduler;

TEST(Scheduler, Schedule_Simply)
{
    std::vector<std::string> output;

    auto scheduler = std::make_shared<QueuedScheduler>();
    auto task = [&]{ output.push_back("hello"); };
    scheduler->Schedule(std::move(task), std::chrono::milliseconds(30));

    auto wait = [&](std::chrono::milliseconds millis) {
        std::this_thread::sleep_for(millis);
        scheduler->Update();
    };

    ASSERT_TRUE(output.empty());
    wait(std::chrono::milliseconds(1));
    EXPECT_TRUE(output.empty());
    wait(std::chrono::milliseconds(2));
    EXPECT_TRUE(output.empty());
    wait(std::chrono::milliseconds(35));
    ASSERT_EQ(1, output.size());
    EXPECT_EQ("hello", output.back());
}

TEST(Scheduler, Schedule_Nested)
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

    ASSERT_TRUE(output.empty());
    wait(std::chrono::milliseconds(10));
    ASSERT_EQ(1, output.size());
    EXPECT_EQ("ok", output.back());
    wait(std::chrono::milliseconds(7));
    ASSERT_EQ(2, output.size());
    EXPECT_EQ("hello", output.back());
}

TEST(Scheduler, Schedule_NestedDelay)
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

    ASSERT_TRUE(output.empty());
    wait(std::chrono::milliseconds(1));
    EXPECT_TRUE(output.empty());
    wait(std::chrono::milliseconds(24));
    ASSERT_EQ(1, output.size());
    EXPECT_EQ("ok", output.back());
    wait(std::chrono::milliseconds(1));
    ASSERT_EQ(2, output.size());
    EXPECT_EQ("hello", output.back());
}

TEST(Scheduler, Schedule_NestedDelay2)
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

    ASSERT_TRUE(output.empty());
    wait(std::chrono::milliseconds(1));
    EXPECT_TRUE(output.empty());
    wait(std::chrono::milliseconds(20));
    ASSERT_EQ(1, output.size());
    EXPECT_EQ("ok", output.back());
    wait(std::chrono::milliseconds(1));
    EXPECT_EQ(1, output.size());
    wait(std::chrono::milliseconds(30));
    EXPECT_EQ(2, output.size());
    EXPECT_EQ("hello", output.back());
}
