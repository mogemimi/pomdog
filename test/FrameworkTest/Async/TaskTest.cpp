// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include <Pomdog/Async/Task.hpp>
#include <Pomdog/Async/Scheduler.hpp>
#include <Pomdog/Async/ImmediateScheduler.hpp>
#include <Pomdog/Async/QueuedScheduler.hpp>
#include <Pomdog/Async/Helpers.hpp>
#include <gtest/iutest_switch.hpp>
#include <thread>

using Pomdog::Duration;
using Pomdog::Concurrency::Task;
using Pomdog::Concurrency::TaskCompletionSource;
using Pomdog::Concurrency::ImmediateScheduler;
using Pomdog::Concurrency::QueuedScheduler;
namespace Concurrency = Pomdog::Concurrency;

struct TaskTest : public ::testing::Test
{
    std::shared_ptr<QueuedScheduler> scheduler;
    std::function<void(std::chrono::duration<double>)> wait;

    void SetUp() override
    {
        scheduler = std::make_shared<QueuedScheduler>();
        wait = [&](const Pomdog::Duration& duration) {
            std::this_thread::sleep_for(
                std::chrono::duration_cast<std::chrono::nanoseconds>(duration));
            scheduler->Update();
        };
    }
};

TEST_F(TaskTest, Delay)
{
    Task<void> task = Concurrency::Delay(std::chrono::milliseconds(20), scheduler);
    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::milliseconds(30));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
}

TEST_F(TaskTest, CreateTask)
{
    auto task = Concurrency::CreateTask([]{}, scheduler);

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
}

TEST_F(TaskTest, CreateTask_ThrowException)
{
    auto task = Concurrency::CreateTask([] {
        throw std::domain_error("Bug");
    }, scheduler);

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());
}

TEST_F(TaskTest, CreateTask_ReturnTask)
{
    auto task = Concurrency::CreateTask([&] {
        return Concurrency::FromResult(42, scheduler);
    }, scheduler);

    int result = 0;
    task.Then([&](int x) { result = x; }, scheduler);

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_EQ(42, result);
}

TEST_F(TaskTest, CreateTask_ReturnRejectedTask)
{
    auto task = Concurrency::CreateTask([&] {
        return Concurrency::CreateTask([] {
            throw std::domain_error("FUS RO DAH");
        }, scheduler);
    }, scheduler);

    std::string result;
    task.Catch([&](const std::exception_ptr& p) {
        try {
            std::rethrow_exception(p);
        }
        catch (const std::domain_error& e) {
            result = e.what();
        }
    });

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    wait(std::chrono::nanoseconds(1));
    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_EQ("FUS RO DAH", result);
}

TEST_F(TaskTest, Then_Deferred)
{
    auto task = Concurrency::CreateTask([]{}, scheduler);
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
    auto task = Concurrency::CreateTask([]{}, scheduler);

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

TEST_F(TaskTest, Then_WithScheduler)
{
    auto task = Concurrency::CreateTask([]{}, scheduler);

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());

    auto task2 = task.Then([]{}, scheduler);
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
}

TEST_F(TaskTest, Then_Results)
{
    std::vector<std::string> results;
    auto task = Concurrency::CreateTask([&]()-> std::string {
        results.push_back("chuck");
        return "chuck";
    }, scheduler);
    auto task2 = task.Then([&](const std::string& name)->std::string {
        EXPECT_EQ("chuck", name);
        results.push_back(name + " ");
        return name + " ";
    });
    auto task3 = task2.Then([&](const std::string& name)->std::string {
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
    auto task = Concurrency::FromResult<int>(42, scheduler)
        .Then([](int x){ return std::to_string(x); })
        .Then([&](const std::string& s){ result = s; });

    wait(std::chrono::nanoseconds(1));
    EXPECT_FALSE(task.IsDone());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_EQ("42", result);
}

TEST_F(TaskTest, Then_ReturnTask)
{
    std::vector<std::string> result;

    auto task1 = Concurrency::FromResult(scheduler);
    auto task2 = task1.Then([&] {
        result.push_back("Delay");
        return Concurrency::Delay(std::chrono::milliseconds(40), scheduler);
    });
    auto task3 = task2.Then([&] {
        result.push_back("FromResult");
        return Concurrency::FromResult(42, scheduler);
    });
    auto task4 = task3.Then([&](int x) {
        result.push_back(std::to_string(x));
    });

    EXPECT_TRUE(task1.IsDone());
    EXPECT_FALSE(task2.IsDone());

    wait(std::chrono::nanoseconds(1));
    ASSERT_EQ(1, result.size());
    EXPECT_EQ("Delay", result.back());
    EXPECT_FALSE(task2.IsDone());

    wait(std::chrono::milliseconds(60));
    EXPECT_FALSE(task2.IsDone());

    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task3.IsDone());

    wait(std::chrono::nanoseconds(1));
    ASSERT_EQ(2, result.size());
    EXPECT_EQ("FromResult", result.back());
    EXPECT_FALSE(task3.IsDone());

    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task3.IsDone());
    EXPECT_FALSE(task4.IsDone());

    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task4.IsDone());
    ASSERT_EQ(3, result.size());
    EXPECT_EQ("42", result.back());
}

TEST_F(TaskTest, Then_ReturnRejectedTask)
{
    std::vector<std::string> result;

    auto task1 = Concurrency::FromResult(scheduler);
    auto task2 = task1.Then([&] {
        result.push_back("fromResult.then");
        return Concurrency::CreateTask([&] {
            result.push_back("fromResult.then.createTask");
            throw std::domain_error("FUS RO DAH");
        }, scheduler);
    });
    auto task3 = task2.Catch([&](const std::exception_ptr& p) {
        try {
            std::rethrow_exception(p);
        }
        catch (const std::domain_error& e) {
            result.push_back(e.what());
        }
    }, scheduler);

    EXPECT_TRUE(task1.IsDone());
    EXPECT_FALSE(task2.IsDone());

    wait(std::chrono::nanoseconds(1));
    ASSERT_EQ(1, result.size());
    EXPECT_EQ("fromResult.then", result.back());
    EXPECT_FALSE(task2.IsDone());

    wait(std::chrono::nanoseconds(1));
    EXPECT_FALSE(task2.IsDone());

    wait(std::chrono::nanoseconds(1));
    ASSERT_EQ(2, result.size());
    EXPECT_EQ("fromResult.then.createTask", result.back());
    EXPECT_TRUE(task2.IsDone());
    EXPECT_TRUE(task2.IsRejected());
    EXPECT_FALSE(task3.IsDone());

    wait(std::chrono::nanoseconds(1));
    ASSERT_EQ(3, result.size());
    EXPECT_EQ("FUS RO DAH", result.back());
    EXPECT_TRUE(task3.IsDone());
    EXPECT_FALSE(task3.IsRejected());
}

TEST_F(TaskTest, Catch_ExceptionPtr)
{
    auto task = Concurrency::CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    }, scheduler);

    std::vector<std::string> result;
    task.Catch([&](const std::exception_ptr& e) {
        try {
            std::rethrow_exception(e);
        }
        catch (const std::domain_error& ex) {
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
    auto task = Concurrency::CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    }, scheduler);

    std::vector<std::string> result;
    task.Catch([&](const std::domain_error& e) {
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

TEST_F(TaskTest, Catch_Then)
{
    auto task = Concurrency::CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    }, scheduler);

    std::vector<std::string> result;

    auto task2 = task.Then([]{});
    task2.Catch([&](const std::domain_error& e) {
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
    auto task1 = Concurrency::CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    }, scheduler);
    auto task2 = Concurrency::CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    }, scheduler);

    std::vector<std::string> result;

    std::vector<Task<void>> tasks = {task1, task2};
    auto whenAll = Concurrency::WhenAll(tasks, scheduler);
    whenAll.Catch([&](const std::domain_error& e) {
        result.push_back(e.what());
    }, scheduler);

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
    auto task1 = Concurrency::CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    }, scheduler);
    auto task2 = Concurrency::CreateTask([] {
        throw std::domain_error(
            "When Chuck Norris throws exceptions, it's across the room.");
    }, scheduler);

    std::vector<std::string> result;
    std::vector<Task<void>> tasks = {task1, task2};

    auto whenAny = Concurrency::WhenAny(std::move(tasks), scheduler);
    whenAny.Catch([&](const std::domain_error& e) {
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

TEST_F(TaskTest, ContinueWith_Deferred)
{
    auto task = Concurrency::CreateTask([]{}, scheduler);
    auto task2 = task.ContinueWith([](const Task<void>&){});

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

TEST_F(TaskTest, ContinueWith_Immediate)
{
    auto task = Concurrency::CreateTask([]{}, scheduler);

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());

    auto task2 = task.ContinueWith([](const Task<void>&){});
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
}

TEST_F(TaskTest, ContinueWith_CatchException)
{
    std::string result;
    auto task = Concurrency::CreateTask([] {
        throw std::domain_error{"fus ro dah"};
    }, scheduler);

    auto task2 = task.ContinueWith([&](const Task<void>& t) {
        if (t.IsRejected()) {
            result = "rejected";
        }
        else {
            result = "done";
        }
    });

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());

    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());
    wait(std::chrono::nanoseconds(1));
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    EXPECT_EQ("rejected", result);
}

TEST_F(TaskTest, WhenAny)
{
    auto task1 = Concurrency::Delay(std::chrono::milliseconds(100), scheduler);
    auto task2 = Concurrency::Delay(std::chrono::milliseconds(200), scheduler);
    auto task3 = Concurrency::Delay(std::chrono::milliseconds(300), scheduler);

    std::vector<Task<void>> tasks = {task1, task2, task3};
    auto whenAny = Concurrency::WhenAny(tasks, scheduler);

    EXPECT_FALSE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());

    wait(std::chrono::nanoseconds(1));
    ASSERT_FALSE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_FALSE(whenAny.IsDone());
    wait(std::chrono::milliseconds(100));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_FALSE(whenAny.IsDone());
    wait(std::chrono::milliseconds(100));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_TRUE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());
    wait(std::chrono::milliseconds(100));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_TRUE(task3.IsDone());
}

TEST_F(TaskTest, WhenAny_Result)
{
    auto task1 = Concurrency::FromResult(42, scheduler);
    auto task2 = Concurrency::FromResult(42, scheduler);

    std::vector<Task<int>> tasks;
    tasks.push_back(task1);
    tasks.push_back(task2);

    std::vector<int> results;

    auto whenAny = Concurrency::WhenAny(std::move(tasks), scheduler);
    whenAny.Then([&](int x) {
        results.push_back(x);
    });

    EXPECT_FALSE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());

    wait(std::chrono::nanoseconds(1));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_TRUE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());
    ASSERT_TRUE(results.empty());
    wait(std::chrono::milliseconds(1));
    ASSERT_EQ(1, results.size());
    EXPECT_EQ(42, results.front());
}

TEST_F(TaskTest, WhenAll)
{
    auto task1 = Concurrency::CreateTask([]{}, scheduler);
    auto task2 = Concurrency::CreateTask([]{}, scheduler);
    auto task3 = Concurrency::CreateTask([]{}, scheduler);

    std::vector<Task<void>> tasks = {task1, task2, task3};
    auto whenAll = Concurrency::WhenAll(tasks, scheduler);

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

TEST_F(TaskTest, WhenAll_Result)
{
    auto task1 = Concurrency::FromResult(42, scheduler);
    auto task2 = Concurrency::FromResult(42, scheduler);

    std::vector<Task<int>> tasks = {task1, task2};
    std::vector<int> results;

    auto whenAll = Concurrency::WhenAll(std::move(tasks), scheduler);
    whenAll.Then([&](const std::vector<int>& v) {
        results = v;
    });

    EXPECT_FALSE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());

    wait(std::chrono::nanoseconds(1));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_TRUE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
    ASSERT_TRUE(results.empty());
    wait(std::chrono::milliseconds(1));
    ASSERT_EQ(2, results.size());
    EXPECT_EQ(42, results[0]);
    EXPECT_EQ(42, results[1]);
}

TEST_F(TaskTest, WhenAll_WithDelay)
{
    auto task1 = Concurrency::Delay(std::chrono::milliseconds(50), scheduler);
    auto task2 = Concurrency::Delay(std::chrono::milliseconds(100), scheduler);
    auto task3 = Concurrency::Delay(std::chrono::milliseconds(150), scheduler);

    std::vector<Task<void>> tasks = {task1, task2, task3};
    auto whenAll = Concurrency::WhenAll(tasks, scheduler);

    wait(std::chrono::nanoseconds(1));
    ASSERT_FALSE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_FALSE(whenAll.IsDone());
    wait(std::chrono::milliseconds(50));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_FALSE(whenAll.IsDone());
    wait(std::chrono::milliseconds(50));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_FALSE(whenAll.IsDone());
    wait(std::chrono::milliseconds(50));
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_TRUE(task3.IsDone());

    EXPECT_FALSE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
    wait(std::chrono::milliseconds(1));
    EXPECT_TRUE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
}

TEST_F(TaskTest, FromResult_Void)
{
    int number = 0;
    auto task = Concurrency::FromResult(scheduler);
    auto task2 = task.Then([&] { number = 42; });

    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_FALSE(task2.IsDone());
    ASSERT_EQ(0, number);
    wait(std::chrono::milliseconds(20));
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    EXPECT_EQ(42, number);
}

TEST_F(TaskTest, FromResult_Int)
{
    int number = 0;
    auto task = Concurrency::FromResult(42, scheduler);
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

TEST_F(TaskTest, Wait_AlreadyDone)
{
    auto task = Concurrency::CreateTask([]{}, scheduler);

    EXPECT_FALSE(task.IsDone());
    wait(std::chrono::nanoseconds::zero());
    task.Wait();
    EXPECT_TRUE(task.IsDone());
}

//TEST_F(TaskTest, Wait_DeadlockCase)
//{
//    auto task = Concurrency::CreateTask([]{}, scheduler);
//
//    EXPECT_FALSE(task.IsDone());
//    task.Wait();
//    wait(std::chrono::nanoseconds::zero());
//    EXPECT_TRUE(task.IsDone());
//}

TEST_F(TaskTest, Wait_FromResult)
{
    auto task = Concurrency::FromResult(42, scheduler);

    EXPECT_TRUE(task.IsDone());
    task.Wait();
    EXPECT_TRUE(task.IsDone());
}

TEST_F(TaskTest, Consturctor_Default)
{
    Task<int> task;
    task = Concurrency::FromResult(42, scheduler);

    EXPECT_TRUE(task.IsDone());
    EXPECT_EQ(scheduler, task.GetScheduler());
}

TEST_F(TaskTest, Get_Result)
{
    auto task = Concurrency::FromResult(42, scheduler);
    EXPECT_EQ(42, Concurrency::Get(task));
}

TEST_F(TaskTest, Get_Exception)
{
    auto task = Concurrency::CreateTask([] {
        throw std::domain_error("Megumi Hayashibara");
    }, scheduler);
    wait(std::chrono::nanoseconds::zero());

    std::string message;
    try {
        Concurrency::Get(task);
    }
    catch (const std::domain_error& e) {
        message = e.what();
    }

    EXPECT_EQ("Megumi Hayashibara", message);
}

TEST(Task, Scheduler_ImmediateExecutor)
{
    auto scheduler = std::make_shared<ImmediateScheduler>();

    auto task = Concurrency::CreateTask([]{
        throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
    }, scheduler);

    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());

    std::vector<std::string> result;

    auto task2 = task.Then([&] {
        result.push_back("ok");
    });

    EXPECT_TRUE(task2.IsDone());
    EXPECT_TRUE(task2.IsRejected());

    task2.Catch([&](const std::domain_error& e) {
        result.push_back(e.what());
    });

    ASSERT_EQ(1, result.size());
    EXPECT_EQ("When Chuck Norris throws exceptions, it's across the room.", result.back());
}

TEST(Task, ChainingSuchAsPromise_1)
{
    auto scheduler = std::make_shared<ImmediateScheduler>();
    std::vector<std::string> result;

    auto task = Concurrency::CreateTask([&]{ result.push_back("start"); }, scheduler);
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
    auto scheduler = std::make_shared<ImmediateScheduler>();
    std::vector<std::string> result;

    auto task = Concurrency::CreateTask([&]{ result.push_back("start"); }, scheduler);
    task.Then([&] {
            result.push_back("task A");
            throw std::domain_error("throw error at Task A");
        })
        .Then([&]{ result.push_back("task B"); })
        .Catch([&](const std::domain_error& e){result.push_back(e.what()); })
        .Then([&]{ result.push_back("task C"); });

    ASSERT_EQ(4, result.size());
    EXPECT_EQ("start", result[0]);
    EXPECT_EQ("task A", result[1]);
    EXPECT_EQ("throw error at Task A", result[2]);
    EXPECT_EQ("task C", result[3]);
}

TEST(Task, ChainingSuchAsPromise_3)
{
    auto scheduler = std::make_shared<ImmediateScheduler>();
    std::vector<std::string> result;

    auto task = Concurrency::CreateTask([&] { result.push_back("start"); }, scheduler);
    task.Then([&] {
        result.push_back("Delay");
        return Concurrency::Delay(std::chrono::milliseconds(10), scheduler);
    })
    .Then([&] {
        result.push_back("FromResult");
        return Concurrency::FromResult(42, scheduler);
    })
    .Then([&](int x) {
        result.push_back(std::to_string(x));
    });

    ASSERT_EQ(4, result.size());
    EXPECT_EQ("start", result[0]);
    EXPECT_EQ("Delay", result[1]);
    EXPECT_EQ("FromResult", result[2]);
    EXPECT_EQ("42", result[3]);
}

TEST(TaskCompletionSource, SetResult_Void)
{
    auto scheduler = std::make_shared<ImmediateScheduler>();
    std::vector<int> result;

    TaskCompletionSource<void> tcs(scheduler);
    auto task = Concurrency::CreateTask(tcs);

    task.Then([&] {
        result.push_back(42);
    });

    ASSERT_TRUE(result.empty());
    tcs.SetResult();

    ASSERT_EQ(1, result.size());
    EXPECT_EQ(42, result.front());
}

TEST(TaskCompletionSource, SetResult_Int)
{
    auto scheduler = std::make_shared<ImmediateScheduler>();
    std::vector<int> result;

    TaskCompletionSource<int> tcs(scheduler);
    auto task = Concurrency::CreateTask(tcs);

    task.Then([&](int x) {
        result.push_back(x);
    });

    ASSERT_TRUE(result.empty());
    tcs.SetResult(42);

    ASSERT_EQ(1, result.size());
    EXPECT_EQ(42, result.front());
}

TEST(TaskCompletionSource, SetException)
{
    auto scheduler = std::make_shared<ImmediateScheduler>();
    std::vector<std::string> result;

    TaskCompletionSource<int> tcs(scheduler);
    auto task = Concurrency::CreateTask(tcs);

    task.Catch([&](const std::domain_error& e) {
        result.push_back(e.what());
    });

    ASSERT_TRUE(result.empty());
    try {
        throw std::domain_error("FUS RO DAH");
    }
    catch (const std::domain_error& e) {
        tcs.SetException(std::make_exception_ptr(e));
    }

    ASSERT_EQ(1, result.size());
    EXPECT_EQ("FUS RO DAH", result.front());
}

TEST(TaskCompletionSource, WithSignal)
{
    using Pomdog::Signal;
    using Pomdog::Signals::ConnectSingleShot;

    auto scheduler = std::make_shared<ImmediateScheduler>();
    TaskCompletionSource<std::string> tcs(scheduler);

    Signal<void(std::string const&)> nameChanged;

    ConnectSingleShot(nameChanged, [tcs](std::string const& n) {
        tcs.SetResult(n);
    });

    std::vector<std::string> result;

    auto task = Concurrency::CreateTask(tcs);
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
    using Pomdog::Signal;

    auto scheduler = std::make_shared<ImmediateScheduler>();
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
