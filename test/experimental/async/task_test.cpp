// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/async/helpers.h"
#include "pomdog/experimental/async/task.h"
#include <catch_amalgamated.hpp>
#include <thread>

using pomdog::concurrency::Task;
using pomdog::concurrency::TaskCompletionSource;
namespace Concurrency = pomdog::concurrency;

//POMDOG_EXPORT
//Task<void> Delay(const Duration& dueTime, const std::shared_ptr<Scheduler>& scheduler)
//{
//    POMDOG_ASSERT(scheduler);
//    TaskCompletionSource<void> tcs;
//    scheduler->Schedule([tcs] { tcs.SetResult(); }, dueTime);
//    Task<void> task(std::move(tcs));
//    return task;
//}

//TEST_CASE("Task::Delay", "[Task]")
//{
//    Task<void> task = Concurrency::Delay(std::chrono::milliseconds(20), scheduler);
//    REQUIRE_FALSE(task.IsDone());
//    REQUIRE_FALSE(task.IsRejected());
//    wait(std::chrono::nanoseconds(1));
//    REQUIRE_FALSE(task.IsDone());
//    REQUIRE_FALSE(task.IsRejected());
//    wait(std::chrono::milliseconds(30));
//    REQUIRE(task.IsDone());
//    REQUIRE_FALSE(task.IsRejected());
//}

TEST_CASE("Task::CreateTask", "[Task]")
{
    auto task = Concurrency::CreateTask<void>([](auto tcs) {
        tcs.SetResult();
    });
    REQUIRE(task.IsDone());
    REQUIRE_FALSE(task.IsRejected());
}

TEST_CASE("Task::CreateTask_Defer", "[Task]")
{
    std::function<void()> defer;
    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        defer = [tcs] { tcs.SetResult(); };
    });

    REQUIRE_FALSE(task.IsDone());
    REQUIRE_FALSE(task.IsRejected());
    defer();
    REQUIRE(task.IsDone());
    REQUIRE_FALSE(task.IsRejected());
}

TEST_CASE("Task::CreateTask_ThrowException", "[Task]")
{
    auto task = Concurrency::CreateTask<void>([](auto tcs) {
        try {
            throw std::domain_error("FUS RO DAH");
        }
        catch (...) {
            tcs.SetException(std::current_exception());
        }
    });

    REQUIRE(task.IsDone());
    REQUIRE(task.IsRejected());

    std::string result;
    task.Catch([&](const std::exception_ptr& p) {
        try {
            std::rethrow_exception(p);
        }
        catch (const std::domain_error& e) {
            result = e.what();
        }
    });

    REQUIRE(task.IsDone());
    REQUIRE(task.IsRejected());
    REQUIRE(result == "FUS RO DAH");
}

TEST_CASE("Task::CreateTask_ThrowException_Defer", "[Task]")
{
    std::function<void()> defer;
    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        defer = [tcs] {
            try {
                throw std::domain_error("FUS RO DAH");
            }
            catch (...) {
                tcs.SetException(std::current_exception());
            }
        };
    });

    REQUIRE_FALSE(task.IsDone());
    REQUIRE_FALSE(task.IsRejected());

    std::string result;
    task.Catch([&](const std::exception_ptr& p) {
        try {
            std::rethrow_exception(p);
        }
        catch (const std::domain_error& e) {
            result = e.what();
        }
    });

    REQUIRE_FALSE(task.IsDone());
    REQUIRE_FALSE(task.IsRejected());
    REQUIRE(result.empty());
    defer();
    REQUIRE(task.IsDone());
    REQUIRE(task.IsRejected());
    REQUIRE(result == "FUS RO DAH");
}

TEST_CASE("Task::Then_Deferred", "[Task]")
{
    std::function<void()> defer;
    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        defer = [tcs] { tcs.SetResult(); };
    });
    auto task2 = task.Then([] {});

    REQUIRE_FALSE(task.IsDone());
    REQUIRE_FALSE(task.IsRejected());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
    defer();
    REQUIRE(task.IsDone());
    REQUIRE_FALSE(task.IsRejected());
    REQUIRE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
}

TEST_CASE("Task::Then_Immediate", "[Task]")
{
    auto task = Concurrency::CreateTask<void>([](auto tcs) {
        tcs.SetResult();
    });

    REQUIRE(task.IsDone());
    REQUIRE_FALSE(task.IsRejected());

    auto task2 = task.Then([] {});
    REQUIRE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
}

TEST_CASE("Task::Then_Results", "[Task]")
{
    std::function<void()> defer;
    std::vector<std::string> results;
    auto task = Concurrency::CreateTask<std::string>([&](auto tcs) {
        defer = [&results, tcs] {
            results.push_back("chuck");
            tcs.SetResult("chuck");
        };
    });
    auto task2 = task.Then([&](const std::string& name) -> std::string {
        REQUIRE(name == "chuck");
        results.push_back(name + " ");
        return name + " ";
    });
    auto task3 = task2.Then([&](const std::string& name) -> std::string {
        REQUIRE(name == "chuck ");
        results.push_back(name + "norris");
        return name + "norris";
    });

    REQUIRE_FALSE(task.IsDone());
    REQUIRE_FALSE(task.IsRejected());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
    REQUIRE_FALSE(task3.IsDone());
    REQUIRE_FALSE(task3.IsRejected());
    defer();
    REQUIRE(task.IsDone());
    REQUIRE_FALSE(task.IsRejected());
    REQUIRE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
    REQUIRE(task3.IsDone());
    REQUIRE_FALSE(task3.IsRejected());

    REQUIRE(results.size() == 3);
    REQUIRE(results[0] == "chuck");
    REQUIRE(results[1] == "chuck ");
    REQUIRE(results[2] == "chuck norris");
}

TEST_CASE("Task::Then_MethodChaining", "[Task]")
{
    std::string result;
    auto task = Concurrency::FromResult<int>(42)
                    .Then([](int x) { return std::to_string(x); })
                    .Then([&](const std::string& s) { result = s; });

    REQUIRE(task.IsDone());
    REQUIRE_FALSE(task.IsRejected());
    REQUIRE(result == "42");
}

TEST_CASE("Task::Then_ReturnTask", "[Task]")
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

    REQUIRE_FALSE(task1.IsDone());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task3.IsDone());
    REQUIRE_FALSE(task4.IsDone());
    REQUIRE(result.empty());
    defer1();
    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == "A");
    REQUIRE(result[1] == "B");
    REQUIRE(task1.IsDone());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task3.IsDone());
    REQUIRE_FALSE(task4.IsDone());
    defer2();
    REQUIRE(result.size() == 5);
    REQUIRE(result[2] == "C");
    REQUIRE(result[3] == "D");
    REQUIRE(result[4] == "E");
    REQUIRE(task2.IsDone());
    REQUIRE_FALSE(task3.IsDone());
    REQUIRE_FALSE(task4.IsDone());
    defer3();
    REQUIRE(result.size() == 8);
    REQUIRE(result[5] == "F");
    REQUIRE(result[6] == "G");
    REQUIRE(result[7] == "42");
    REQUIRE(task3.IsDone());
    REQUIRE(task4.IsDone());
}

TEST_CASE("Task::Then_ReturnRejectedTask", "[Task]")
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
            defer2 = [&result, tcs] {
                try {
                    result.push_back("D");
                    throw std::domain_error("FUS RO DAH");
                }
                catch (...) {
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

    REQUIRE_FALSE(task1.IsDone());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task3.IsDone());
    defer1();
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == "A");
    REQUIRE(result[1] == "B");
    REQUIRE(result[2] == "C");
    REQUIRE(task1.IsDone());
    REQUIRE_FALSE(task1.IsRejected());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
    REQUIRE_FALSE(task3.IsDone());
    defer2();
    REQUIRE(result.size() == 6);
    REQUIRE(result[3] == "D");
    REQUIRE(result[4] == "E");
    REQUIRE(result[5] == "FUS RO DAH");
    REQUIRE(task2.IsDone());
    REQUIRE(task2.IsRejected());
    REQUIRE(task3.IsDone());
    REQUIRE_FALSE(task3.IsRejected());
}

TEST_CASE("Task::Catch_ExceptionPtr", "[Task]")
{
    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        try {
            throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
        }
        catch (...) {
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

    REQUIRE(task.IsDone());
    REQUIRE(task.IsRejected());
    REQUIRE(result == "When Chuck Norris throws exceptions, it's across the room.");
}

TEST_CASE("Task::Catch_ExceptionType", "[Task]")
{
    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        try {
            throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
        }
        catch (...) {
            tcs.SetException(std::current_exception());
        }
    });

    std::string result;
    task.Catch([&](const std::domain_error& e) {
        result = e.what();
    });

    REQUIRE(task.IsDone());
    REQUIRE(task.IsRejected());
    REQUIRE(result == "When Chuck Norris throws exceptions, it's across the room.");
}

TEST_CASE("Task::Catch_Then", "[Task]")
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

    REQUIRE(result.empty());
    REQUIRE_FALSE(task1.IsDone());
    REQUIRE_FALSE(task1.IsRejected());
    REQUIRE(result.empty());
    defer();
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == "A");
    REQUIRE(result[1] == "C");
    REQUIRE(result[2] == "When Chuck Norris throws exceptions, it's across the room.");
    REQUIRE(task1.IsDone());
    REQUIRE(task1.IsRejected());
    REQUIRE(task2.IsDone());
    REQUIRE(task2.IsRejected());
    REQUIRE(task3.IsDone());
    REQUIRE_FALSE(task3.IsRejected());
}

TEST_CASE("Task::Catch_WhenAll", "[Task]")
{
    std::function<void()> defer1;
    std::function<void()> defer2;

    auto task1 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer1 = [tcs] {
            try {
                throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
            }
            catch (...) {
                tcs.SetException(std::current_exception());
            }
        };
    });
    auto task2 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer2 = [tcs] {
            try {
                throw std::domain_error("FUS RO DAH");
            }
            catch (...) {
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

    REQUIRE_FALSE(task1.IsDone());
    REQUIRE_FALSE(task1.IsRejected());
    REQUIRE_FALSE(whenAll.IsDone());
    REQUIRE_FALSE(whenAll.IsRejected());
    REQUIRE(result.empty());
    defer1();
    REQUIRE(task1.IsDone());
    REQUIRE(task1.IsRejected());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
    REQUIRE(whenAll.IsDone());
    REQUIRE(whenAll.IsRejected());
    REQUIRE(result.size() == 1);
    REQUIRE(result.back() == "When Chuck Norris throws exceptions, it's across the room.");
    defer2();
    REQUIRE(task2.IsDone());
    REQUIRE(task2.IsRejected());
    REQUIRE(result.size() == 1);
}

TEST_CASE("Task::Catch_WhenAny", "[Task]")
{
    std::function<void()> defer1;
    std::function<void()> defer2;

    auto task1 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer1 = [tcs] {
            try {
                throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
            }
            catch (...) {
                tcs.SetException(std::current_exception());
            }
        };
    });
    auto task2 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer2 = [tcs] {
            try {
                throw std::domain_error("FUS RO DAH");
            }
            catch (...) {
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

    REQUIRE_FALSE(task1.IsDone());
    REQUIRE_FALSE(task1.IsRejected());
    REQUIRE_FALSE(whenAny.IsDone());
    REQUIRE_FALSE(whenAny.IsRejected());
    REQUIRE(result.empty());
    defer1();
    REQUIRE(task1.IsDone());
    REQUIRE(task1.IsRejected());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
    REQUIRE(whenAny.IsDone());
    REQUIRE(whenAny.IsRejected());
    REQUIRE(result.size() == 1);
    REQUIRE(result.back() == "When Chuck Norris throws exceptions, it's across the room.");
    defer2();
    REQUIRE(task2.IsDone());
    REQUIRE(task2.IsRejected());
    REQUIRE(result.size() == 1);
}

TEST_CASE("Task::ContinueWith_Deferred", "[Task]")
{
    std::function<void()> defer;
    auto task1 = Concurrency::CreateTask<void>([&](auto tcs) {
        defer = [tcs] { tcs.SetResult(); };
    });
    auto task2 = task1.ContinueWith([&](const Task<void>&) {
    });

    REQUIRE_FALSE(task1.IsDone());
    REQUIRE_FALSE(task1.IsRejected());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
    defer();
    REQUIRE(task1.IsDone());
    REQUIRE_FALSE(task1.IsRejected());
    REQUIRE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
}

TEST_CASE("Task::ContinueWith_Immediate", "[Task]")
{
    auto task1 = Concurrency::CreateTask<void>([](auto tcs) {
        tcs.SetResult();
    });
    REQUIRE(task1.IsDone());
    REQUIRE_FALSE(task1.IsRejected());
    auto task2 = task1.ContinueWith([](const Task<void>&) {});
    REQUIRE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
}

TEST_CASE("Task::ContinueWith_CatchException", "[Task]")
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

    REQUIRE_FALSE(task1.IsDone());
    REQUIRE_FALSE(task1.IsRejected());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
    defer();
    REQUIRE(task1.IsDone());
    REQUIRE(task1.IsRejected());
    REQUIRE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
    REQUIRE(result == "rejected");
}

TEST_CASE("Task::WhenAny", "[Task]")
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

    REQUIRE_FALSE(whenAny.IsDone());
    REQUIRE_FALSE(whenAny.IsRejected());

    REQUIRE_FALSE(task1.IsDone());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task3.IsDone());
    REQUIRE_FALSE(whenAny.IsDone());
    REQUIRE_FALSE(whenAny.IsRejected());
    defer1();
    REQUIRE(task1.IsDone());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task3.IsDone());
    REQUIRE(whenAny.IsDone());
    REQUIRE_FALSE(whenAny.IsRejected());
    defer2();
    REQUIRE(task1.IsDone());
    REQUIRE(task2.IsDone());
    REQUIRE_FALSE(task3.IsDone());
    REQUIRE(whenAny.IsDone());
    REQUIRE_FALSE(whenAny.IsRejected());
    defer3();
    REQUIRE(task1.IsDone());
    REQUIRE(task2.IsDone());
    REQUIRE(task3.IsDone());
    REQUIRE(whenAny.IsDone());
    REQUIRE_FALSE(whenAny.IsRejected());
}

TEST_CASE("Task::WhenAny_Result", "[Task]")
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
    std::vector<Task<int>> tasks = {task1, task2};

    auto whenAny = Concurrency::WhenAny(std::move(tasks));
    whenAny.Then([&](int x) {
        results.push_back(x);
    });

    REQUIRE_FALSE(whenAny.IsDone());
    REQUIRE_FALSE(whenAny.IsRejected());
    defer1();
    REQUIRE(task1.IsDone());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE(whenAny.IsDone());
    REQUIRE_FALSE(whenAny.IsRejected());
    REQUIRE(results.size() == 1);
    REQUIRE(results.back() == 42);
    defer2();
    REQUIRE(task1.IsDone());
    REQUIRE(task2.IsDone());
    REQUIRE(results.size() == 1);
    REQUIRE(results.back() == 42);
}

TEST_CASE("Task::WhenAll", "[Task]")
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

    REQUIRE_FALSE(task1.IsDone());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task3.IsDone());
    defer1();
    REQUIRE(task1.IsDone());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(task3.IsDone());
    defer2();
    REQUIRE(task1.IsDone());
    REQUIRE(task2.IsDone());
    REQUIRE_FALSE(task3.IsDone());
    REQUIRE_FALSE(whenAll.IsDone());
    REQUIRE_FALSE(whenAll.IsRejected());
    defer3();
    REQUIRE(task1.IsDone());
    REQUIRE(task2.IsDone());
    REQUIRE(task3.IsDone());
    REQUIRE(whenAll.IsDone());
    REQUIRE_FALSE(whenAll.IsRejected());
}

TEST_CASE("Task::WhenAll_Result", "[Task]")
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
    std::vector<Task<int>> tasks = {task1, task2};

    auto whenAll = Concurrency::WhenAll(std::move(tasks));
    whenAll.Then([&](const std::vector<int>& v) {
        results = v;
    });

    REQUIRE_FALSE(whenAll.IsDone());
    REQUIRE_FALSE(whenAll.IsRejected());
    defer1();
    REQUIRE(task1.IsDone());
    REQUIRE_FALSE(task2.IsDone());
    REQUIRE_FALSE(whenAll.IsDone());
    REQUIRE_FALSE(whenAll.IsRejected());
    REQUIRE(results.empty());
    defer2();
    REQUIRE(task1.IsDone());
    REQUIRE(task2.IsDone());
    REQUIRE(whenAll.IsDone());
    REQUIRE_FALSE(whenAll.IsRejected());
    REQUIRE(results.size() == 2);
    REQUIRE(results[0] == 42);
    REQUIRE(results[1] == 43);
}

TEST_CASE("Task::FromResult_Int", "[Task]")
{
    int number = 0;
    auto task1 = Concurrency::FromResult(42);
    auto task2 = task1.Then([&](int x) { number = x; });

    REQUIRE(task1.IsDone());
    REQUIRE_FALSE(task1.IsRejected());
    REQUIRE(task2.IsDone());
    REQUIRE_FALSE(task2.IsRejected());
    REQUIRE(number == 42);
}

TEST_CASE("Task::Scheduler_ImmediateExecutor", "[Task]")
{
    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        try {
            throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
        }
        catch (...) {
            tcs.SetException(std::current_exception());
        }
    });

    REQUIRE(task.IsDone());
    REQUIRE(task.IsRejected());

    std::vector<std::string> result;

    auto task2 = task.Then([&] {
        result.push_back("ok");
    });

    REQUIRE(task2.IsDone());
    REQUIRE(task2.IsRejected());

    task2.Catch([&](const std::domain_error& e) {
        result.push_back(e.what());
    });

    REQUIRE(result.size() == 1);
    REQUIRE(result.back() == "When Chuck Norris throws exceptions, it's across the room.");
}

TEST_CASE("Task::ChainingSuchAsPromise_1", "[Task]")
{
    std::vector<std::string> result;

    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        result.push_back("start");
        tcs.SetResult();
    });
    task.Then([&] { result.push_back("task A"); })
        .Then([&] { result.push_back("task B"); })
        .Catch([&](std::exception_ptr) { result.push_back("onRejected"); })
        .Then([&] { result.push_back("task C"); });

    REQUIRE(result.size() == 4);
    REQUIRE(result[0] == "start");
    REQUIRE(result[1] == "task A");
    REQUIRE(result[2] == "task B");
    REQUIRE(result[3] == "task C");
}

TEST_CASE("Task::ChainingSuchAsPromise_2", "[Task]")
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
        .Then([&] { result.push_back("task B"); })
        .Catch([&](const std::domain_error& e) { result.push_back(e.what()); })
        .Then([&] { result.push_back("task C"); });

    REQUIRE(result.size() == 4);
    REQUIRE(result[0] == "start");
    REQUIRE(result[1] == "task A");
    REQUIRE(result[2] == "throw error at Task A");
    REQUIRE(result[3] == "task C");
}

TEST_CASE("Task::ChainingSuchAsPromise_3", "[Task]")
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
    REQUIRE(result.size() == 4);
    REQUIRE(result[0] == "start");
    REQUIRE(result[1] == "Delay");
    REQUIRE(result[2] == "FromResult");
    REQUIRE(result[3] == "42");
}

TEST_CASE("TaskCompletionSource::SetResult_Void", "[TaskCompletionSource]")
{
    std::vector<int> result;

    auto task = Concurrency::CreateTask<void>([&](auto tcs) {
        REQUIRE(result.empty());
        tcs.SetResult();
        REQUIRE(result.empty());
    });
    REQUIRE(result.empty());
    task.Then([&] {
        result.push_back(42);
    });
    REQUIRE(result.size() == 1);
    REQUIRE(result.front() == 42);
}

TEST_CASE("TaskCompletionSource::SetResult_Int", "[TaskCompletionSource]")
{
    std::vector<int> result;

    auto task = Concurrency::CreateTask<int>([&](auto tcs) {
        REQUIRE(result.empty());
        tcs.SetResult(42);
        REQUIRE(result.empty());
    });
    REQUIRE(result.empty());
    task.Then([&](int x) {
        result.push_back(x);
    });
    REQUIRE(result.size() == 1);
    REQUIRE(result.front() == 42);
}

TEST_CASE("TaskCompletionSource::SetException", "[TaskCompletionSource]")
{
    std::vector<std::string> result;

    auto task = Concurrency::CreateTask<int>([&](auto tcs) {
        try {
            throw std::domain_error("FUS RO DAH");
        }
        catch (...) {
            REQUIRE(result.empty());
            tcs.SetException(std::current_exception());
            REQUIRE(result.empty());
        }
    });
    REQUIRE(result.empty());
    task.Catch([&](const std::domain_error& e) {
        result.push_back(e.what());
    });
    REQUIRE(result.size() == 1);
    REQUIRE(result.front() == "FUS RO DAH");
}

TEST_CASE("TaskCompletionSource::WithSignal", "[TaskCompletionSource]")
{
    using pomdog::Signal;
    using pomdog::Signals::ConnectSingleShot;

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

    REQUIRE(result.empty());

    nameChanged("chuck");
    REQUIRE(result.size() == 1);
    REQUIRE(result.back() == "chuck");

    nameChanged("norris");
    REQUIRE(result.size() == 1);
}

TEST_CASE("TaskCompletionSource::FromSingleShotSignal", "[TaskCompletionSource]")
{
    using pomdog::Signal;

    Signal<void(std::string const&)> nameChanged;

    std::vector<std::string> result;

    auto task = Concurrency::FromSingleShotSignal(nameChanged);
    task.Then([&](std::string const& n) {
        result.push_back(n);
    });

    REQUIRE(result.empty());

    nameChanged("chuck");
    REQUIRE(result.size() == 1);
    REQUIRE(result.back() == "chuck");

    nameChanged("norris");
    REQUIRE(result.size() == 1);
}
