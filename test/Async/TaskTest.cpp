// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Async/Task.hpp>
#include <Pomdog/Async/Helpers.hpp>
#include <gtest/iutest_switch.hpp>
#include <thread>

using Pomdog::Concurrency::Task;
using Pomdog::Concurrency::TaskCompletionSource;
namespace Concurrency = Pomdog::Concurrency;

//POMDOG_EXPORT
//Task<void> Delay(const Duration& dueTime, const std::shared_ptr<Scheduler>& scheduler)
//{
//    POMDOG_ASSERT(scheduler);
//    TaskCompletionSource<void> tcs;
//    scheduler->Schedule([tcs] { tcs.SetResult(); }, dueTime);
//    Task<void> task(std::move(tcs));
//    return task;
//}

//TEST(Task, Delay)
//{
//    Task<void> task = Concurrency::Delay(std::chrono::milliseconds(20), scheduler);
//    EXPECT_FALSE(task.IsDone());
//    EXPECT_FALSE(task.IsRejected());
//    wait(std::chrono::nanoseconds(1));
//    EXPECT_FALSE(task.IsDone());
//    EXPECT_FALSE(task.IsRejected());
//    wait(std::chrono::milliseconds(30));
//    EXPECT_TRUE(task.IsDone());
//    EXPECT_FALSE(task.IsRejected());
//}

TEST(Task, CreateTask)
{
    auto task = Concurrency::CreateTask<void>([](auto tcs) {
        tcs.SetResult();
    });
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
}

TEST(Task, CreateTask_Defer)
{
    std::function<void()> defer;
    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        defer = [tcs]{ tcs.SetResult(); };
    });

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    defer();
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
}

TEST(Task, CreateTask_ThrowException)
{
    auto task = Concurrency::CreateTask<void>([](auto tcs) {
        try {
            throw std::domain_error("FUS RO DAH");
        } catch (...) {
            tcs.SetException(std::current_exception());
        }
    });

    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());

    std::string result;
    task.Catch([&](const std::exception_ptr& p) {
        try {
            std::rethrow_exception(p);
        }
        catch (const std::domain_error& e) {
            result = e.what();
        }
    });

    ASSERT_TRUE(task.IsDone());
    ASSERT_TRUE(task.IsRejected());
    EXPECT_EQ("FUS RO DAH", result);
}

TEST(Task, CreateTask_ThrowException_Defer)
{
    std::function<void()> defer;
    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        defer = [tcs] {
            try {
                throw std::domain_error("FUS RO DAH");
            } catch (...) {
                tcs.SetException(std::current_exception());
            }
        };
    });

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());

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
    EXPECT_TRUE(result.empty());
    defer();
    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());
    EXPECT_EQ("FUS RO DAH", result);
}

TEST(Task, Then_Deferred)
{
    std::function<void()> defer;
    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        defer = [tcs]{ tcs.SetResult(); };
    });
    auto task2 = task.Then([]{});

    EXPECT_FALSE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    defer();
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
}

TEST(Task, Then_Immediate)
{
    auto task = Concurrency::CreateTask<void>([](auto tcs) {
        tcs.SetResult();
    });

    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());

    auto task2 = task.Then([]{});
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
}

TEST(Task, Then_Results)
{
    std::function<void()> defer;
    std::vector<std::string> results;
    auto task = Concurrency::CreateTask<std::string>([&](auto tcs) {
        defer = [&results, tcs] {
            results.push_back("chuck");
            tcs.SetResult("chuck");
        };
    });
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
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    EXPECT_FALSE(task3.IsDone());
    EXPECT_FALSE(task3.IsRejected());
    defer();
    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    EXPECT_TRUE(task3.IsDone());
    EXPECT_FALSE(task3.IsRejected());

    ASSERT_EQ(3, results.size());
    EXPECT_EQ("chuck", results[0]);
    EXPECT_EQ("chuck ", results[1]);
    EXPECT_EQ("chuck norris", results[2]);
}

TEST(Task, Then_MethodChaining)
{
    std::string result;
    auto task = Concurrency::FromResult<int>(42)
        .Then([](int x){ return std::to_string(x); })
        .Then([&](const std::string& s){ result = s; });

    EXPECT_TRUE(task.IsDone());
    EXPECT_FALSE(task.IsRejected());
    EXPECT_EQ("42", result);
}

TEST(Task, Then_ReturnTask)
{
    std::function<void()> defer1;
    std::function<void()> defer2;
    std::function<void()> defer3;
    std::vector<std::string> result;

    auto task1 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer1 = [tcs] { tcs.SetResult(); };
    });
    auto task2 = task1.Then([&] {
        result.push_back("A");
        return Concurrency::CreateTask<void>([&](auto tcs) {
            result.push_back("B");
            defer2 = [&result, tcs] {
                result.push_back("C");
                tcs.SetResult();
            };
        });
    });
    auto task3 = task2.Then([&] {
        result.push_back("D");
        return Concurrency::CreateTask<int>([&](auto tcs) {
            result.push_back("E");
            defer3 = [&result, tcs] {
                result.push_back("F");
                tcs.SetResult(42);
            };
        });
    });
    auto task4 = task3.Then([&](int x) {
        result.push_back("G");
        result.push_back(std::to_string(x));
    });

    EXPECT_FALSE(task1.IsDone());
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task3.IsDone());
    EXPECT_FALSE(task4.IsDone());
    ASSERT_TRUE(result.empty());
    defer1();
    ASSERT_EQ(2, result.size());
    EXPECT_EQ("A", result[0]);
    EXPECT_EQ("B", result[1]);
    EXPECT_TRUE(task1.IsDone());
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task3.IsDone());
    EXPECT_FALSE(task4.IsDone());
    defer2();
    ASSERT_EQ(5, result.size());
    EXPECT_EQ("C", result[2]);
    EXPECT_EQ("D", result[3]);
    EXPECT_EQ("E", result[4]);
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task3.IsDone());
    EXPECT_FALSE(task4.IsDone());
    defer3();
    ASSERT_EQ(8, result.size());
    EXPECT_EQ("F", result[5]);
    EXPECT_EQ("G", result[6]);
    EXPECT_EQ("42", result[7]);
    EXPECT_TRUE(task3.IsDone());
    EXPECT_TRUE(task4.IsDone());
}

TEST(Task, Then_ReturnRejectedTask)
{
    std::function<void()> defer1;
    std::function<void()> defer2;
    std::vector<std::string> result;

    auto task1 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer1 = [&result, tcs] {
            result.push_back("A");
            tcs.SetResult();
        };
    });
    auto task2 = task1.Then([&] {
        result.push_back("B");
        return Concurrency::CreateTask<void>([&](auto tcs) {
            result.push_back("C");
            defer2 = [&result, tcs]{
                try {
                    result.push_back("D");
                    throw std::domain_error("FUS RO DAH");
                } catch (...) {
                    tcs.SetException(std::current_exception());
                }
            };
        });
    });
    auto task3 = task2.Catch([&](const std::exception_ptr& p) {
        try {
            result.push_back("E");
            std::rethrow_exception(p);
        }
        catch (const std::domain_error& e) {
            result.push_back(e.what());
        }
    });

    EXPECT_FALSE(task1.IsDone());
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task3.IsDone());
    defer1();
    ASSERT_EQ(3, result.size());
    EXPECT_EQ("A", result[0]);
    EXPECT_EQ("B", result[1]);
    EXPECT_EQ("C", result[2]);
    EXPECT_TRUE(task1.IsDone());
    EXPECT_FALSE(task1.IsRejected());
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    EXPECT_FALSE(task3.IsDone());
    defer2();
    ASSERT_EQ(6, result.size());
    EXPECT_EQ("D", result[3]);
    EXPECT_EQ("E", result[4]);
    EXPECT_EQ("FUS RO DAH", result[5]);
    EXPECT_TRUE(task2.IsDone());
    EXPECT_TRUE(task2.IsRejected());
    EXPECT_TRUE(task3.IsDone());
    EXPECT_FALSE(task3.IsRejected());
}

TEST(Task, Catch_ExceptionPtr)
{
    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        try {
            throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
        } catch (...) {
            tcs.SetException(std::current_exception());
        }
    });

    std::string result;
    task.Catch([&](const std::exception_ptr& p) {
        try {
            std::rethrow_exception(p);
        }
        catch (const std::domain_error& e) {
            result = e.what();
        }
    });

    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());
    EXPECT_EQ("When Chuck Norris throws exceptions, it's across the room.", result);
}

TEST(Task, Catch_ExceptionType)
{
    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        try {
            throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
        } catch (...) {
            tcs.SetException(std::current_exception());
        }
    });

    std::string result;
    task.Catch([&](const std::domain_error& e) {
        result = e.what();
    });

    EXPECT_TRUE(task.IsDone());
    EXPECT_TRUE(task.IsRejected());
    EXPECT_EQ("When Chuck Norris throws exceptions, it's across the room.", result);
}

TEST(Task, Catch_Then)
{
    std::function<void()> defer;
    std::vector<std::string> result;

    auto task1 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer = [&result, tcs] {
            try {
                result.push_back("A");
                throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
            }
            catch (...) {
                tcs.SetException(std::current_exception());
            }
        };
    });

    auto task2 = task1.Then([&] {
        result.push_back("B");
    });
    auto task3 = task2.Catch([&](const std::domain_error& e) {
        result.push_back("C");
        result.push_back(e.what());
    });

    EXPECT_TRUE(result.empty());
    EXPECT_FALSE(task1.IsDone());
    EXPECT_FALSE(task1.IsRejected());
    EXPECT_TRUE(result.empty());
    defer();
    ASSERT_EQ(3, result.size());
    EXPECT_EQ("A", result[0]);
    EXPECT_EQ("C", result[1]);
    EXPECT_EQ("When Chuck Norris throws exceptions, it's across the room.", result[2]);
    EXPECT_TRUE(task1.IsDone());
    EXPECT_TRUE(task1.IsRejected());
    EXPECT_TRUE(task2.IsDone());
    EXPECT_TRUE(task2.IsRejected());
    EXPECT_TRUE(task3.IsDone());
    EXPECT_FALSE(task3.IsRejected());
}

TEST(Task, Catch_WhenAll)
{
    std::function<void()> defer1;
    std::function<void()> defer2;

    auto task1 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer1 = [tcs] {
            try {
                throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
            } catch (...) {
                tcs.SetException(std::current_exception());
            }
        };
    });
    auto task2 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer2 = [tcs] {
            try {
                throw std::domain_error("FUS RO DAH");
            } catch (...) {
                tcs.SetException(std::current_exception());
            }
        };
    });

    std::vector<std::string> result;

    std::vector<Task<void>> tasks = {task1, task2};
    auto whenAll = Concurrency::WhenAll(tasks);
    whenAll.Catch([&](const std::domain_error& e) {
        result.push_back(e.what());
    });

    ASSERT_FALSE(task1.IsDone());
    ASSERT_FALSE(task1.IsRejected());
    EXPECT_FALSE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
    EXPECT_TRUE(result.empty());
    defer1();
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task1.IsRejected());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task2.IsRejected());
    EXPECT_TRUE(whenAll.IsDone());
    EXPECT_TRUE(whenAll.IsRejected());
    ASSERT_EQ(1, result.size());
    EXPECT_EQ("When Chuck Norris throws exceptions, it's across the room.", result.back());
    defer2();
    ASSERT_TRUE(task2.IsDone());
    ASSERT_TRUE(task2.IsRejected());
    ASSERT_EQ(1, result.size());
}

TEST(Task, Catch_WhenAny)
{
    std::function<void()> defer1;
    std::function<void()> defer2;

    auto task1 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer1 = [tcs] {
            try {
                throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
            } catch (...) {
                tcs.SetException(std::current_exception());
            }
        };
    });
    auto task2 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer2 = [tcs] {
            try {
                throw std::domain_error("FUS RO DAH");
            } catch (...) {
                tcs.SetException(std::current_exception());
            }
        };
    });

    std::vector<std::string> result;

    std::vector<Task<void>> tasks = {task1, task2};
    auto whenAny = Concurrency::WhenAny(tasks);
    whenAny.Catch([&](const std::domain_error& e) {
        result.push_back(e.what());
    });

    ASSERT_FALSE(task1.IsDone());
    ASSERT_FALSE(task1.IsRejected());
    EXPECT_FALSE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());
    EXPECT_TRUE(result.empty());
    defer1();
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task1.IsRejected());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task2.IsRejected());
    EXPECT_TRUE(whenAny.IsDone());
    EXPECT_TRUE(whenAny.IsRejected());
    ASSERT_EQ(1, result.size());
    EXPECT_EQ("When Chuck Norris throws exceptions, it's across the room.", result.back());
    defer2();
    ASSERT_TRUE(task2.IsDone());
    ASSERT_TRUE(task2.IsRejected());
    ASSERT_EQ(1, result.size());
}

TEST(Task, ContinueWith_Deferred)
{
    std::function<void()> defer;
    auto task1 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer = [tcs] { tcs.SetResult(); };
    });
    auto task2 = task1.ContinueWith([&](const Task<void>&) {
    });

    EXPECT_FALSE(task1.IsDone());
    EXPECT_FALSE(task1.IsRejected());
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    defer();
    EXPECT_TRUE(task1.IsDone());
    EXPECT_FALSE(task1.IsRejected());
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
}

TEST(Task, ContinueWith_Immediate)
{
    auto task1 = Concurrency::CreateTask<void>([](auto tcs) {
        tcs.SetResult();
    });
    EXPECT_TRUE(task1.IsDone());
    EXPECT_FALSE(task1.IsRejected());
    auto task2 = task1.ContinueWith([](const Task<void>&){});
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
}

TEST(Task, ContinueWith_CatchException)
{
    std::function<void()> defer;
    std::string result;

    auto task1 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer = [tcs] {
            try {
                throw std::domain_error("fus ro dah");
            }
            catch (...) {
                tcs.SetException(std::current_exception());
            }
        };
    });
    auto task2 = task1.ContinueWith([&](const Task<void>& t) {
        if (t.IsRejected()) {
            result = "rejected";
        }
        else {
            result = "done";
        }
    });

    EXPECT_FALSE(task1.IsDone());
    EXPECT_FALSE(task1.IsRejected());
    EXPECT_FALSE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    defer();
    EXPECT_TRUE(task1.IsDone());
    EXPECT_TRUE(task1.IsRejected());
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    EXPECT_EQ("rejected", result);
}

TEST(Task, WhenAny)
{
    std::function<void()> defer1;
    std::function<void()> defer2;
    std::function<void()> defer3;
    auto task1 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer1 = [tcs] { tcs.SetResult(); };
    });
    auto task2 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer2 = [tcs] { tcs.SetResult(); };
    });
    auto task3 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer3 = [tcs] { tcs.SetResult(); };
    });

    std::vector<Task<void>> tasks = {task1, task2, task3};
    auto whenAny = Concurrency::WhenAny(tasks);

    EXPECT_FALSE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());

    ASSERT_FALSE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_FALSE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());
    defer1();
    ASSERT_TRUE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_TRUE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());
    defer2();
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_TRUE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());
    defer3();
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_TRUE(task3.IsDone());
    EXPECT_TRUE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());
}

TEST(Task, WhenAny_Result)
{
    std::function<void()> defer1;
    std::function<void()> defer2;
    auto task1 = Concurrency::CreateTask<int>([&](auto tcs) {
        defer1 = [tcs] { tcs.SetResult(42); };
    });
    auto task2 = Concurrency::CreateTask<int>([&](auto tcs) {
        defer2 = [tcs] { tcs.SetResult(43); };
    });

    std::vector<int> results;
    std::vector<Task<int>> tasks = { task1, task2 };

    auto whenAny = Concurrency::WhenAny(std::move(tasks));
    whenAny.Then([&](int x) {
        results.push_back(x);
    });

    EXPECT_FALSE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());
    defer1();
    ASSERT_TRUE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    EXPECT_TRUE(whenAny.IsDone());
    EXPECT_FALSE(whenAny.IsRejected());
    ASSERT_EQ(1, results.size());
    EXPECT_EQ(42, results.back());
    defer2();
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_EQ(1, results.size());
    ASSERT_EQ(42, results.back());
}

TEST(Task, WhenAll)
{
    std::function<void()> defer1;
    std::function<void()> defer2;
    std::function<void()> defer3;
    auto task1 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer1 = [tcs] { tcs.SetResult(); };
    });
    auto task2 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer2 = [tcs] { tcs.SetResult(); };
    });
    auto task3 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer3 = [tcs] { tcs.SetResult(); };
    });

    std::vector<Task<void>> tasks = {task1, task2, task3};
    auto whenAll = Concurrency::WhenAll(tasks);

    ASSERT_FALSE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    defer1();
    ASSERT_TRUE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    defer2();
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_FALSE(task3.IsDone());
    EXPECT_FALSE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
    defer3();
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    ASSERT_TRUE(task3.IsDone());
    EXPECT_TRUE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
}

TEST(Task, WhenAll_Result)
{
    std::function<void()> defer1;
    std::function<void()> defer2;
    auto task1 = Concurrency::CreateTask<int>([&](auto tcs) {
        defer1 = [tcs] { tcs.SetResult(42); };
    });
    auto task2 = Concurrency::CreateTask<int>([&](auto tcs) {
        defer2 = [tcs] { tcs.SetResult(43); };
    });

    std::vector<int> results;
    std::vector<Task<int>> tasks = { task1, task2 };

    auto whenAll = Concurrency::WhenAll(std::move(tasks));
    whenAll.Then([&](const std::vector<int>& v) {
        results = v;
    });

    EXPECT_FALSE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
    defer1();
    ASSERT_TRUE(task1.IsDone());
    ASSERT_FALSE(task2.IsDone());
    EXPECT_FALSE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
    ASSERT_TRUE(results.empty());
    defer2();
    ASSERT_TRUE(task1.IsDone());
    ASSERT_TRUE(task2.IsDone());
    EXPECT_TRUE(whenAll.IsDone());
    EXPECT_FALSE(whenAll.IsRejected());
    ASSERT_EQ(2, results.size());
    EXPECT_EQ(42, results[0]);
    EXPECT_EQ(43, results[1]);
}

TEST(Task, FromResult_Int)
{
    int number = 0;
    auto task1 = Concurrency::FromResult(42);
    auto task2 = task1.Then([&](int x){ number = x; });

    EXPECT_TRUE(task1.IsDone());
    EXPECT_FALSE(task1.IsRejected());
    EXPECT_TRUE(task2.IsDone());
    EXPECT_FALSE(task2.IsRejected());
    EXPECT_EQ(42, number);
}

TEST(Task, Scheduler_ImmediateExecutor)
{
    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        try {
            throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
        } catch (...) {
            tcs.SetException(std::current_exception());
        }
    });

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
    std::vector<std::string> result;

    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        result.push_back("start");
        tcs.SetResult();
    });
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
    std::vector<std::string> result;

    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        result.push_back("start");
        tcs.SetResult();
    });
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
    std::function<void()> defer;
    std::vector<std::string> result;

    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        result.push_back("start");
        tcs.SetResult();
    });
    task.Then([&] {
        result.push_back("Delay");
        return Concurrency::CreateTask<void>([&](auto tcs) {
            defer = [tcs] { tcs.SetResult(); };
        });
    })
    .Then([&] {
        result.push_back("FromResult");
        return Concurrency::FromResult(42);
    })
    .Then([&](int x) {
        result.push_back(std::to_string(x));
    });

    defer();
    ASSERT_EQ(4, result.size());
    EXPECT_EQ("start", result[0]);
    EXPECT_EQ("Delay", result[1]);
    EXPECT_EQ("FromResult", result[2]);
    EXPECT_EQ("42", result[3]);
}

TEST(TaskCompletionSource, SetResult_Void)
{
    std::vector<int> result;

    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        EXPECT_TRUE(result.empty());
        tcs.SetResult();
        EXPECT_TRUE(result.empty());
    });
    EXPECT_TRUE(result.empty());
    task.Then([&] {
        result.push_back(42);
    });
    ASSERT_EQ(1, result.size());
    EXPECT_EQ(42, result.front());
}

TEST(TaskCompletionSource, SetResult_Int)
{
    std::vector<int> result;

    auto task = Concurrency::CreateTask<int>([&](auto tcs) {
        EXPECT_TRUE(result.empty());
        tcs.SetResult(42);
        EXPECT_TRUE(result.empty());
    });
    EXPECT_TRUE(result.empty());
    task.Then([&](int x) {
        result.push_back(x);
    });
    ASSERT_EQ(1, result.size());
    EXPECT_EQ(42, result.front());
}

TEST(TaskCompletionSource, SetException)
{
    std::vector<std::string> result;

    auto task = Concurrency::CreateTask<int>([&](auto tcs) {
        try {
            throw std::domain_error("FUS RO DAH");
        }
        catch (...) {
            EXPECT_TRUE(result.empty());
            tcs.SetException(std::current_exception());
            EXPECT_TRUE(result.empty());
        }
    });
    EXPECT_TRUE(result.empty());
    task.Catch([&](const std::domain_error& e) {
        result.push_back(e.what());
    });
    ASSERT_EQ(1, result.size());
    EXPECT_EQ("FUS RO DAH", result.front());
}

TEST(TaskCompletionSource, WithSignal)
{
    using Pomdog::Signal;
    using Pomdog::Signals::ConnectSingleShot;

    Signal<void(std::string const&)> nameChanged;

    std::vector<std::string> result;

    auto task = Concurrency::CreateTask<std::string>([&](auto tcs) {
        ConnectSingleShot(nameChanged, [tcs](std::string const& n) {
            tcs.SetResult(n);
        });
    });
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

    Signal<void(std::string const&)> nameChanged;

    std::vector<std::string> result;

    auto task = Concurrency::FromSingleShotSignal(nameChanged);
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
