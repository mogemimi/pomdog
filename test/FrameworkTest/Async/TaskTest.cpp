// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include <Pomdog/Async/Task.hpp>
#include <Pomdog/Async/Scheduler.hpp>
#include <Pomdog/Async/Helpers.hpp>
#include <gtest/iutest_switch.hpp>
#include <thread>

using Pomdog::Concurrency::Task;
using Pomdog::Concurrency::TaskFactory;
using Pomdog::Concurrency::TaskCompletionSource;
using Pomdog::Concurrency::TickedScheduler;
using Pomdog::Concurrency::ImmediateExecutor;

struct TaskTest : public ::testing::Test
{
    std::shared_ptr<TickedScheduler> scheduler;
    TaskFactory concurrency;
    std::function<void(Pomdog::Duration)> wait;

    void SetUp() override
    {
        scheduler = std::make_shared<TickedScheduler>();
        concurrency = TaskFactory{scheduler};
        wait = [&](Pomdog::Duration duration) {
            std::this_thread::sleep_for(
                std::chrono::duration_cast<std::chrono::nanoseconds>(duration));
            scheduler->Tick();
        };
    }
};

TEST_F(TaskTest, Delay)
{
    Task<void> task = concurrency.Delay(std::chrono::milliseconds(20));
    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::milliseconds(30));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());

    auto promise = concurrency;
}

TEST_F(TaskTest, StartNew)
{
    auto task = concurrency.CreateTask([]{});

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
}

TEST_F(TaskTest, StartNew_ThrowException)
{
    auto task = concurrency.CreateTask([] {
        throw std::domain_error("Bug");
    });

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());
}

TEST_F(TaskTest, Then_Deferred)
{
    auto task = concurrency.CreateTask([]{});
    auto task2 = task.Then([]{});

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());

    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
}

TEST_F(TaskTest, Then_Immediate)
{
    auto task = concurrency.CreateTask([]{});

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());

    auto task2 = task.Then([]{});
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
}

TEST_F(TaskTest, Then_Nested)
{
    auto task = concurrency.CreateTask([]{});

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());

    auto task2 = task.Then([]{});
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
}

TEST_F(TaskTest, Then_WithoutScheduler)
{
    auto task = concurrency.CreateTask([]{});

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());

    auto task2 = task.Then([]{});
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
}

TEST_F(TaskTest, Then_Results)
{
    std::vector<std::string> results;
    auto task = concurrency.CreateTask([&]()-> std::string {
        results.push_back("chuck");
        return "chuck";
    });
    auto task2 = task.Then([&](std::string const& name)->std::string {
        EXPECT_EQ("chuck", name);
        results.push_back(name + " ");
        return name + " ";
    });
    auto task3 = task2.Then([&](std::string const& name)->std::string {
        EXPECT_EQ("chuck ", name);
        results.push_back(name + "norris");
        return name + "norris";
    });

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());

    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());

    EXPECT_FALSE(task3.IsDone());
    EXPECT_FALSE(task3.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task3.IsDone());
    EXPECT_FALSE(task3.IsRejected());

    ASSERT_EQ(3, results.size());
    EXPECT_EQ("chuck", results[0]);
    EXPECT_EQ("chuck ", results[1]);
    EXPECT_EQ("chuck norris", results[2]);
}

TEST_F(TaskTest, Then_MethodChaining)
{
    std::string result;
    auto task = concurrency.FromResult<int>(42)
        .Then([](int x){ return std::to_string(x); })
        .Then([&](std::string const& s){ result = s; });

    wait(std::chrono::nanoseconds(1));
    EXPECT_FALSE(task.IsDone());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_EQ("42", result);
}

TEST_F(TaskTest, Catch_ExceptionPtr)
{
    auto task = concurrency.CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    });

    std::vector<std::string> result;
    task.Catch([&](std::exception_ptr const& e) {
        try {
            std::rethrow_exception(e);
        }
        catch (std::domain_error const& ex) {
            result.push_back(ex.what());
        }
    });

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_TRUE(result.empty());

    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());

    EXPECT_TRUE(result.empty());
    wait(std::chrono::nanoseconds(1));

    ASSERT_EQ(1, result.size());
    EXPECT_EQ("When Chuck Norris throws exceptions, it's across the room.", result.back());
}

TEST_F(TaskTest, Catch_ExceptionType)
{
    auto task = concurrency.CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    });

    std::vector<std::string> result;
    task.Catch([&](std::domain_error const& e) {
        result.push_back(e.what());
    });

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_TRUE(result.empty());

    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());

    EXPECT_TRUE(result.empty());
    wait(std::chrono::nanoseconds(1));

    ASSERT_EQ(1, result.size());
    EXPECT_EQ("When Chuck Norris throws exceptions, it's across the room.", result.back());
}

TEST_F(TaskTest, Catch_ContinueWith)
{
    auto task = concurrency.CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    });

    std::vector<std::string> result;

    auto task2 = task.Then([]{});
    task2.Catch([&](std::domain_error const& e) {
        result.push_back(e.what());
    });

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_TRUE(result.empty());

    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());

    EXPECT_TRUE(result.empty());
    wait(std::chrono::nanoseconds(1));

    EXPECT_TRUE(task2.IsDone());
    EXPECT_TRUE(task2.IsRejected());

    EXPECT_TRUE(result.empty());
    wait(std::chrono::nanoseconds(1));

    ASSERT_EQ(1, result.size());
    EXPECT_EQ("When Chuck Norris throws exceptions, it's across the room.", result.back());
}

TEST_F(TaskTest, Catch_WhenAll)
{
    auto task1 = concurrency.CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    });
    auto task2 = concurrency.CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    });

    std::vector<std::string> result;

    auto whenAll = concurrency.WhenAll({task1, task2});
    whenAll.Catch([&](std::domain_error const& e) {
        result.push_back(e.what());
    });

    wait(std::chrono::nanoseconds(1));
    EXPECT_FALSE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
    EXPECT_TRUE(result.empty());

    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(whenAll.IsDone());
    EXPECT_TRUE(whenAll.IsRejected());
    EXPECT_TRUE(result.empty());

    wait(std::chrono::nanoseconds(1));
    ASSERT_EQ(1, result.size());
    EXPECT_EQ("When Chuck Norris throws exceptions, it's across the room.", result.back());
}

TEST_F(TaskTest, Catch_WhenAny)
{
    auto task1 = concurrency.CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    });
    auto task2 = concurrency.CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    });

    std::vector<std::string> result;

    auto whenAny = concurrency.WhenAny({task1, task2});
    whenAny.Catch([&](std::domain_error const& e) {
        result.push_back(e.what());
    });

    wait(std::chrono::nanoseconds(1));
    EXPECT_FALSE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());
    EXPECT_TRUE(result.empty());

    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(whenAny.IsDone());
    EXPECT_TRUE(whenAny.IsRejected());
    EXPECT_TRUE(result.empty());

    wait(std::chrono::nanoseconds(1));
    ASSERT_EQ(1, result.size());
    EXPECT_EQ("When Chuck Norris throws exceptions, it's across the room.", result.back());
}

TEST_F(TaskTest, WhenAny)
{
    auto task1 = concurrency.Delay(std::chrono::milliseconds(20));
    auto task2 = concurrency.Delay(std::chrono::milliseconds(40));
    auto task3 = concurrency.Delay(std::chrono::milliseconds(60));

    auto whenAny = concurrency.WhenAny({task1, task2, task3});

    EXPECT_FALSE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());

    wait(std::chrono::nanoseconds(1));
    ASSERT_FALSE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_FALSE(whenAny.IsDone());
    wait(std::chrono::milliseconds(20));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_FALSE(whenAny.IsDone());
    wait(std::chrono::milliseconds(20));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_TRUE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());
    wait(std::chrono::milliseconds(20));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_TRUE(task3.IsDone());
}

TEST_F(TaskTest, WhenAll)
{
    auto task1 = concurrency.CreateTask([]{});
    auto task2 = concurrency.CreateTask([]{});
    auto task3 = concurrency.CreateTask([]{});

    auto whenAll = concurrency.WhenAll({task1, task2, task3});

    ASSERT_FALSE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    wait(std::chrono::nanoseconds(1));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_TRUE(task3.IsDone());

    EXPECT_FALSE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
}

TEST_F(TaskTest, WhenAll_WithDelay)
{
    auto task1 = concurrency.Delay(std::chrono::milliseconds(20));
    auto task2 = concurrency.Delay(std::chrono::milliseconds(40));
    auto task3 = concurrency.Delay(std::chrono::milliseconds(60));

    auto whenAll = concurrency.WhenAll({task1, task2, task3});

    wait(std::chrono::nanoseconds(1));
    ASSERT_FALSE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_FALSE(whenAll.IsDone());
    wait(std::chrono::milliseconds(20));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_FALSE(whenAll.IsDone());
    wait(std::chrono::milliseconds(20));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_FALSE(whenAll.IsDone());
    wait(std::chrono::milliseconds(20));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_TRUE(task3.IsDone());

    EXPECT_FALSE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
    wait(std::chrono::milliseconds(1));
    EXPECT_TRUE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
}

TEST_F(TaskTest, FromResult)
{
    int number = 0;
    auto task = concurrency.FromResult<int>(42);
    auto task2 = task.Then([&](int x){ number = x; });

    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_FALSE(task2.IsDone());
    ASSERT_EQ(0, number);
    wait(std::chrono::milliseconds(20));
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    EXPECT_EQ(42, number);
}

TEST(TaskCompletionSource, WithSignal)
{
    using namespace Pomdog;
    using namespace Pomdog::Signals;

    auto scheduler = std::make_shared<ImmediateExecutor>();
    auto tcs = std::make_shared<TaskCompletionSource<std::string>>(scheduler);

    Signal<void(std::string const&)> nameChanged;

    ConnectSingleShot(nameChanged, [tcs](std::string const& n) {
        tcs->SetResult(n);
    });

    std::vector<std::string> result;

    auto task = tcs->GetTask();
    task.Then([&](std::string const& n) {
        result.push_back(n);
    });

    ASSERT_TRUE(result.empty());

    nameChanged("chuck");
    EXPECT_EQ(1, result.size());
    EXPECT_EQ("chuck", result.back());

    nameChanged("norris");
    EXPECT_EQ(1, result.size());
}

TEST(TaskCompletionSource, FromSingleShotSignal)
{
    using namespace Pomdog;
    auto scheduler = std::make_shared<ImmediateExecutor>();
    Signal<void(std::string const&)> nameChanged;

    std::vector<std::string> result;

    auto task = Concurrency::FromSingleShotSignal(nameChanged, scheduler);
    task.Then([&](std::string const& n) {
        result.push_back(n);
    });

    ASSERT_TRUE(result.empty());

    nameChanged("chuck");
    EXPECT_EQ(1, result.size());
    EXPECT_EQ("chuck", result.back());

    nameChanged("norris");
    EXPECT_EQ(1, result.size());
}

TEST(Task, Scheduler_ImmediateExecutor)
{
    auto scheduler = std::make_shared<ImmediateExecutor>();
    TaskFactory concurrency{scheduler};

    auto task = concurrency.CreateTask([]{
        throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
    });

    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());

    std::vector<std::string> result;

    auto task2 = task.Then([&] {
        result.push_back("ok");
    });

    EXPECT_TRUE(task2.IsDone());
    EXPECT_TRUE(task2.IsRejected());

    task2.Catch([&](std::domain_error const& e) {
        result.push_back(e.what());
    });

    ASSERT_EQ(1, result.size());
    EXPECT_EQ("When Chuck Norris throws exceptions, it's across the room.", result.back());
}

TEST(Task, ChainingSuchAsPromise_1)
{
    auto scheduler = std::make_shared<ImmediateExecutor>();
    TaskFactory concurrency{scheduler};
    std::vector<std::string> result;

    auto task = concurrency.CreateTask([&]{ result.push_back("start"); });
    task.Then([&]{ result.push_back("task A"); })
        .Then([&]{ result.push_back("task B"); })
        .Catch([&](std::exception_ptr){ result.push_back("onRejected"); })
        .Then([&]{ result.push_back("task C"); });

    ASSERT_EQ(4, result.size());
    EXPECT_EQ("start", result[0]);
    EXPECT_EQ("task A", result[1]);
    EXPECT_EQ("task B", result[2]);
    EXPECT_EQ("task C", result[3]);
}

TEST(Task, ChainingSuchAsPromise_2)
{
    auto scheduler = std::make_shared<ImmediateExecutor>();
    TaskFactory concurrency{scheduler};
    std::vector<std::string> result;

    auto task = concurrency.CreateTask([&]{ result.push_back("start"); });
    task.Then([&] {
            result.push_back("task A");
            throw std::domain_error("throw error at Task A");
        })
        .Then([&]{ result.push_back("task B"); })
        .Catch([&](std::domain_error const& e){result.push_back(e.what()); })
        .Then([&]{ result.push_back("task C"); });

    ASSERT_EQ(4, result.size());
    EXPECT_EQ("start", result[0]);
    EXPECT_EQ("task A", result[1]);
    EXPECT_EQ("throw error at Task A", result[2]);
    EXPECT_EQ("task C", result[3]);
}
