// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"

// FIXME: Suppress warnings for WhenAnyPromise and WhenAllPromise classes
POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4625)
POMDOG_MSVC_SUPPRESS_WARNING(4626)
POMDOG_MSVC_SUPPRESS_WARNING(5026)
POMDOG_MSVC_SUPPRESS_WARNING(5027)
#include "pomdog/experimental/async/helpers.h"
#include "pomdog/experimental/async/task.h"
POMDOG_MSVC_SUPPRESS_WARNING_POP

#include "pomdog/signals/signal.h"
#include "pomdog/signals/signal_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::concurrency::Task;
using pomdog::concurrency::TaskCompletionSource;
namespace concurrency = pomdog::concurrency;

TEST_CASE("Task")
{
    SUBCASE("createTask")
    {
        auto task = concurrency::createTask<void>([](auto tcs) {
            tcs.setResult();
        });
        REQUIRE(task.isDone());
        REQUIRE_FALSE(task.isRejected());
    }
    SUBCASE("createTask with defer")
    {
        std::function<void()> defer;
        auto task = concurrency::createTask<void>([&](auto tcs) {
            defer = [tcs] { tcs.setResult(); };
        });

        REQUIRE_FALSE(task.isDone());
        REQUIRE_FALSE(task.isRejected());
        defer();
        REQUIRE(task.isDone());
        REQUIRE_FALSE(task.isRejected());
    }
    SUBCASE("createTask throwing exception")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        auto task = concurrency::createTask<void>([](auto tcs) {
            try {
                throw std::domain_error("FUS RO DAH");
            }
            catch (...) {
                tcs.setException(std::current_exception());
            }
        });

        REQUIRE(task.isDone());
        REQUIRE(task.isRejected());

        std::string result;
        task.Catch([&](const std::exception_ptr& p) {
            try {
                std::rethrow_exception(p);
            }
            catch (const std::domain_error& e) {
                result = e.what();
            }
        });

        REQUIRE(task.isDone());
        REQUIRE(task.isRejected());
        REQUIRE(result == "FUS RO DAH");
#endif
    }
    SUBCASE("createTask throwing exception with defer")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        std::function<void()> defer;
        auto task = concurrency::createTask<void>([&](auto tcs) {
            defer = [tcs] {
                try {
                    throw std::domain_error("FUS RO DAH");
                }
                catch (...) {
                    tcs.setException(std::current_exception());
                }
            };
        });

        REQUIRE_FALSE(task.isDone());
        REQUIRE_FALSE(task.isRejected());

        std::string result;
        task.Catch([&](const std::exception_ptr& p) {
            try {
                std::rethrow_exception(p);
            }
            catch (const std::domain_error& e) {
                result = e.what();
            }
        });

        REQUIRE_FALSE(task.isDone());
        REQUIRE_FALSE(task.isRejected());
        REQUIRE(result.empty());
        defer();
        REQUIRE(task.isDone());
        REQUIRE(task.isRejected());
        REQUIRE(result == "FUS RO DAH");
#endif
    }
    SUBCASE("then deferred")
    {
        std::function<void()> defer;
        auto task = concurrency::createTask<void>([&](auto tcs) {
            defer = [tcs] { tcs.setResult(); };
        });
        auto task2 = task.then([] {});

        REQUIRE_FALSE(task.isDone());
        REQUIRE_FALSE(task.isRejected());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
        defer();
        REQUIRE(task.isDone());
        REQUIRE_FALSE(task.isRejected());
        REQUIRE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
    }
    SUBCASE("then immediate")
    {
        auto task = concurrency::createTask<void>([](auto tcs) {
            tcs.setResult();
        });

        REQUIRE(task.isDone());
        REQUIRE_FALSE(task.isRejected());

        auto task2 = task.then([] {});
        REQUIRE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
    }
    SUBCASE("then with results")
    {
        std::function<void()> defer;
        std::vector<std::string> results;
        auto task = concurrency::createTask<std::string>([&](auto tcs) {
            defer = [&results, tcs] {
                results.push_back("chuck");
                tcs.setResult("chuck");
            };
        });
        auto task2 = task.then([&](const std::string& name) -> std::string {
            REQUIRE(name == "chuck");
            results.push_back(name + " ");
            return name + " ";
        });
        auto task3 = task2.then([&](const std::string& name) -> std::string {
            REQUIRE(name == "chuck ");
            results.push_back(name + "norris");
            return name + "norris";
        });

        REQUIRE_FALSE(task.isDone());
        REQUIRE_FALSE(task.isRejected());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
        REQUIRE_FALSE(task3.isDone());
        REQUIRE_FALSE(task3.isRejected());
        defer();
        REQUIRE(task.isDone());
        REQUIRE_FALSE(task.isRejected());
        REQUIRE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
        REQUIRE(task3.isDone());
        REQUIRE_FALSE(task3.isRejected());

        REQUIRE(results.size() == 3);
        REQUIRE(results[0] == "chuck");
        REQUIRE(results[1] == "chuck ");
        REQUIRE(results[2] == "chuck norris");
    }
    SUBCASE("then method chaining")
    {
        std::string result;
        auto task = concurrency::fromResult<int>(42)
                        .then([](int x) { return std::to_string(x); })
                        .then([&](const std::string& s) { result = s; });

        REQUIRE(task.isDone());
        REQUIRE_FALSE(task.isRejected());
        REQUIRE(result == "42");
    }
    SUBCASE("then returning Task")
    {
        std::function<void()> defer1;
        std::function<void()> defer2;
        std::function<void()> defer3;
        std::vector<std::string> result;

        auto task1 = concurrency::createTask<void>([&](auto tcs) {
            defer1 = [tcs] { tcs.setResult(); };
        });
        auto task2 = task1.then([&] {
            result.push_back("A");
            return concurrency::createTask<void>([&](auto tcs) {
                result.push_back("B");
                defer2 = [&result, tcs] {
                    result.push_back("C");
                    tcs.setResult();
                };
            });
        });
        auto task3 = task2.then([&] {
            result.push_back("D");
            return concurrency::createTask<int>([&](auto tcs) {
                result.push_back("E");
                defer3 = [&result, tcs] {
                    result.push_back("F");
                    tcs.setResult(42);
                };
            });
        });
        auto task4 = task3.then([&](int x) {
            result.push_back("G");
            result.push_back(std::to_string(x));
        });

        REQUIRE_FALSE(task1.isDone());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task3.isDone());
        REQUIRE_FALSE(task4.isDone());
        REQUIRE(result.empty());
        defer1();
        REQUIRE(result.size() == 2);
        REQUIRE(result[0] == "A");
        REQUIRE(result[1] == "B");
        REQUIRE(task1.isDone());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task3.isDone());
        REQUIRE_FALSE(task4.isDone());
        defer2();
        REQUIRE(result.size() == 5);
        REQUIRE(result[2] == "C");
        REQUIRE(result[3] == "D");
        REQUIRE(result[4] == "E");
        REQUIRE(task2.isDone());
        REQUIRE_FALSE(task3.isDone());
        REQUIRE_FALSE(task4.isDone());
        defer3();
        REQUIRE(result.size() == 8);
        REQUIRE(result[5] == "F");
        REQUIRE(result[6] == "G");
        REQUIRE(result[7] == "42");
        REQUIRE(task3.isDone());
        REQUIRE(task4.isDone());
    }
    SUBCASE("then returning rejected Task")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        std::function<void()> defer1;
        std::function<void()> defer2;
        std::vector<std::string> result;

        auto task1 = concurrency::createTask<void>([&](auto tcs) {
            defer1 = [&result, tcs] {
                result.push_back("A");
                tcs.setResult();
            };
        });
        auto task2 = task1.then([&] {
            result.push_back("B");
            return concurrency::createTask<void>([&](auto tcs) {
                result.push_back("C");
                defer2 = [&result, tcs] {
                    try {
                        result.push_back("D");
                        throw std::domain_error("FUS RO DAH");
                    }
                    catch (...) {
                        tcs.setException(std::current_exception());
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

        REQUIRE_FALSE(task1.isDone());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task3.isDone());
        defer1();
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == "A");
        REQUIRE(result[1] == "B");
        REQUIRE(result[2] == "C");
        REQUIRE(task1.isDone());
        REQUIRE_FALSE(task1.isRejected());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
        REQUIRE_FALSE(task3.isDone());
        defer2();
        REQUIRE(result.size() == 6);
        REQUIRE(result[3] == "D");
        REQUIRE(result[4] == "E");
        REQUIRE(result[5] == "FUS RO DAH");
        REQUIRE(task2.isDone());
        REQUIRE(task2.isRejected());
        REQUIRE(task3.isDone());
        REQUIRE_FALSE(task3.isRejected());
#endif
    }
    SUBCASE("Catch with exception_ptr")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        auto task = concurrency::createTask<void>([&](auto tcs) {
            try {
                throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
            }
            catch (...) {
                tcs.setException(std::current_exception());
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

        REQUIRE(task.isDone());
        REQUIRE(task.isRejected());
        REQUIRE(result == "When Chuck Norris throws exceptions, it's across the room.");
#endif
    }
    SUBCASE("Catch with exception type")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        auto task = concurrency::createTask<void>([&](auto tcs) {
            try {
                throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
            }
            catch (...) {
                tcs.setException(std::current_exception());
            }
        });

        std::string result;
        task.Catch([&](const std::domain_error& e) {
            result = e.what();
        });

        REQUIRE(task.isDone());
        REQUIRE(task.isRejected());
        REQUIRE(result == "When Chuck Norris throws exceptions, it's across the room.");
#endif
    }
    SUBCASE("Catch then")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        std::function<void()> defer;
        std::vector<std::string> result;

        auto task1 = concurrency::createTask<void>([&](auto tcs) {
            defer = [&result, tcs] {
                try {
                    result.push_back("A");
                    throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
                }
                catch (...) {
                    tcs.setException(std::current_exception());
                }
            };
        });

        auto task2 = task1.then([&] {
            result.push_back("B");
        });
        auto task3 = task2.Catch([&](const std::domain_error& e) {
            result.push_back("C");
            result.push_back(e.what());
        });

        REQUIRE(result.empty());
        REQUIRE_FALSE(task1.isDone());
        REQUIRE_FALSE(task1.isRejected());
        REQUIRE(result.empty());
        defer();
        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == "A");
        REQUIRE(result[1] == "C");
        REQUIRE(result[2] == "When Chuck Norris throws exceptions, it's across the room.");
        REQUIRE(task1.isDone());
        REQUIRE(task1.isRejected());
        REQUIRE(task2.isDone());
        REQUIRE(task2.isRejected());
        REQUIRE(task3.isDone());
        REQUIRE_FALSE(task3.isRejected());
#endif
    }
    SUBCASE("Catch with whenAll")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        std::function<void()> defer1;
        std::function<void()> defer2;

        auto task1 = concurrency::createTask<void>([&](auto tcs) {
            defer1 = [tcs] {
                try {
                    throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
                }
                catch (...) {
                    tcs.setException(std::current_exception());
                }
            };
        });
        auto task2 = concurrency::createTask<void>([&](auto tcs) {
            defer2 = [tcs] {
                try {
                    throw std::domain_error("FUS RO DAH");
                }
                catch (...) {
                    tcs.setException(std::current_exception());
                }
            };
        });

        std::vector<std::string> result;

        std::vector<Task<void>> tasks = {task1, task2};
        auto whenAll = concurrency::whenAll(tasks);
        whenAll.Catch([&](const std::domain_error& e) {
            result.push_back(e.what());
        });

        REQUIRE_FALSE(task1.isDone());
        REQUIRE_FALSE(task1.isRejected());
        REQUIRE_FALSE(whenAll.isDone());
        REQUIRE_FALSE(whenAll.isRejected());
        REQUIRE(result.empty());
        defer1();
        REQUIRE(task1.isDone());
        REQUIRE(task1.isRejected());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
        REQUIRE(whenAll.isDone());
        REQUIRE(whenAll.isRejected());
        REQUIRE(result.size() == 1);
        REQUIRE(result.back() == "When Chuck Norris throws exceptions, it's across the room.");
        defer2();
        REQUIRE(task2.isDone());
        REQUIRE(task2.isRejected());
        REQUIRE(result.size() == 1);
#endif
    }
    SUBCASE("Catch with whenAny")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        std::function<void()> defer1;
        std::function<void()> defer2;

        auto task1 = concurrency::createTask<void>([&](auto tcs) {
            defer1 = [tcs] {
                try {
                    throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
                }
                catch (...) {
                    tcs.setException(std::current_exception());
                }
            };
        });
        auto task2 = concurrency::createTask<void>([&](auto tcs) {
            defer2 = [tcs] {
                try {
                    throw std::domain_error("FUS RO DAH");
                }
                catch (...) {
                    tcs.setException(std::current_exception());
                }
            };
        });

        std::vector<std::string> result;

        std::vector<Task<void>> tasks = {task1, task2};
        auto whenAny = concurrency::whenAny(tasks);
        whenAny.Catch([&](const std::domain_error& e) {
            result.push_back(e.what());
        });

        REQUIRE_FALSE(task1.isDone());
        REQUIRE_FALSE(task1.isRejected());
        REQUIRE_FALSE(whenAny.isDone());
        REQUIRE_FALSE(whenAny.isRejected());
        REQUIRE(result.empty());
        defer1();
        REQUIRE(task1.isDone());
        REQUIRE(task1.isRejected());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
        REQUIRE(whenAny.isDone());
        REQUIRE(whenAny.isRejected());
        REQUIRE(result.size() == 1);
        REQUIRE(result.back() == "When Chuck Norris throws exceptions, it's across the room.");
        defer2();
        REQUIRE(task2.isDone());
        REQUIRE(task2.isRejected());
        REQUIRE(result.size() == 1);
#endif
    }
    SUBCASE("continueWith deferred")
    {
        std::function<void()> defer;
        auto task1 = concurrency::createTask<void>([&](auto tcs) {
            defer = [tcs] { tcs.setResult(); };
        });
        auto task2 = task1.continueWith([&](const Task<void>&) {
        });

        REQUIRE_FALSE(task1.isDone());
        REQUIRE_FALSE(task1.isRejected());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
        defer();
        REQUIRE(task1.isDone());
        REQUIRE_FALSE(task1.isRejected());
        REQUIRE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
    }
    SUBCASE("continueWith immediate")
    {
        auto task1 = concurrency::createTask<void>([](auto tcs) {
            tcs.setResult();
        });
        REQUIRE(task1.isDone());
        REQUIRE_FALSE(task1.isRejected());
        auto task2 = task1.continueWith([](const Task<void>&) {});
        REQUIRE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
    }
    SUBCASE("continueWith catching exception")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        std::function<void()> defer;
        std::string result;

        auto task1 = concurrency::createTask<void>([&](auto tcs) {
            defer = [tcs] {
                try {
                    throw std::domain_error("fus ro dah");
                }
                catch (...) {
                    tcs.setException(std::current_exception());
                }
            };
        });
        auto task2 = task1.continueWith([&](const Task<void>& t) {
            if (t.isRejected()) {
                result = "rejected";
            }
            else {
                result = "done";
            }
        });

        REQUIRE_FALSE(task1.isDone());
        REQUIRE_FALSE(task1.isRejected());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
        defer();
        REQUIRE(task1.isDone());
        REQUIRE(task1.isRejected());
        REQUIRE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
        REQUIRE(result == "rejected");
#endif
    }
    SUBCASE("whenAny")
    {
        std::function<void()> defer1;
        std::function<void()> defer2;
        std::function<void()> defer3;
        auto task1 = concurrency::createTask<void>([&](auto tcs) {
            defer1 = [tcs] { tcs.setResult(); };
        });
        auto task2 = concurrency::createTask<void>([&](auto tcs) {
            defer2 = [tcs] { tcs.setResult(); };
        });
        auto task3 = concurrency::createTask<void>([&](auto tcs) {
            defer3 = [tcs] { tcs.setResult(); };
        });

        std::vector<Task<void>> tasks = {task1, task2, task3};
        auto whenAny = concurrency::whenAny(tasks);

        REQUIRE_FALSE(whenAny.isDone());
        REQUIRE_FALSE(whenAny.isRejected());

        REQUIRE_FALSE(task1.isDone());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task3.isDone());
        REQUIRE_FALSE(whenAny.isDone());
        REQUIRE_FALSE(whenAny.isRejected());
        defer1();
        REQUIRE(task1.isDone());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task3.isDone());
        REQUIRE(whenAny.isDone());
        REQUIRE_FALSE(whenAny.isRejected());
        defer2();
        REQUIRE(task1.isDone());
        REQUIRE(task2.isDone());
        REQUIRE_FALSE(task3.isDone());
        REQUIRE(whenAny.isDone());
        REQUIRE_FALSE(whenAny.isRejected());
        defer3();
        REQUIRE(task1.isDone());
        REQUIRE(task2.isDone());
        REQUIRE(task3.isDone());
        REQUIRE(whenAny.isDone());
        REQUIRE_FALSE(whenAny.isRejected());
    }
    SUBCASE("whenAny with result")
    {
        std::function<void()> defer1;
        std::function<void()> defer2;
        auto task1 = concurrency::createTask<int>([&](auto tcs) {
            defer1 = [tcs] { tcs.setResult(42); };
        });
        auto task2 = concurrency::createTask<int>([&](auto tcs) {
            defer2 = [tcs] { tcs.setResult(43); };
        });

        std::vector<int> results;
        std::vector<Task<int>> tasks = {task1, task2};

        auto whenAny = concurrency::whenAny(std::move(tasks));
        whenAny.then([&](int x) {
            results.push_back(x);
        });

        REQUIRE_FALSE(whenAny.isDone());
        REQUIRE_FALSE(whenAny.isRejected());
        defer1();
        REQUIRE(task1.isDone());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE(whenAny.isDone());
        REQUIRE_FALSE(whenAny.isRejected());
        REQUIRE(results.size() == 1);
        REQUIRE(results.back() == 42);
        defer2();
        REQUIRE(task1.isDone());
        REQUIRE(task2.isDone());
        REQUIRE(results.size() == 1);
        REQUIRE(results.back() == 42);
    }
    SUBCASE("whenAll")
    {
        std::function<void()> defer1;
        std::function<void()> defer2;
        std::function<void()> defer3;
        auto task1 = concurrency::createTask<void>([&](auto tcs) {
            defer1 = [tcs] { tcs.setResult(); };
        });
        auto task2 = concurrency::createTask<void>([&](auto tcs) {
            defer2 = [tcs] { tcs.setResult(); };
        });
        auto task3 = concurrency::createTask<void>([&](auto tcs) {
            defer3 = [tcs] { tcs.setResult(); };
        });

        std::vector<Task<void>> tasks = {task1, task2, task3};
        auto whenAll = concurrency::whenAll(tasks);

        REQUIRE_FALSE(task1.isDone());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task3.isDone());
        defer1();
        REQUIRE(task1.isDone());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(task3.isDone());
        defer2();
        REQUIRE(task1.isDone());
        REQUIRE(task2.isDone());
        REQUIRE_FALSE(task3.isDone());
        REQUIRE_FALSE(whenAll.isDone());
        REQUIRE_FALSE(whenAll.isRejected());
        defer3();
        REQUIRE(task1.isDone());
        REQUIRE(task2.isDone());
        REQUIRE(task3.isDone());
        REQUIRE(whenAll.isDone());
        REQUIRE_FALSE(whenAll.isRejected());
    }
    SUBCASE("whenAll with result")
    {
        std::function<void()> defer1;
        std::function<void()> defer2;
        auto task1 = concurrency::createTask<int>([&](auto tcs) {
            defer1 = [tcs] { tcs.setResult(42); };
        });
        auto task2 = concurrency::createTask<int>([&](auto tcs) {
            defer2 = [tcs] { tcs.setResult(43); };
        });

        std::vector<int> results;
        std::vector<Task<int>> tasks = {task1, task2};

        auto whenAll = concurrency::whenAll(std::move(tasks));
        whenAll.then([&](const std::vector<int>& v) {
            results = v;
        });

        REQUIRE_FALSE(whenAll.isDone());
        REQUIRE_FALSE(whenAll.isRejected());
        defer1();
        REQUIRE(task1.isDone());
        REQUIRE_FALSE(task2.isDone());
        REQUIRE_FALSE(whenAll.isDone());
        REQUIRE_FALSE(whenAll.isRejected());
        REQUIRE(results.empty());
        defer2();
        REQUIRE(task1.isDone());
        REQUIRE(task2.isDone());
        REQUIRE(whenAll.isDone());
        REQUIRE_FALSE(whenAll.isRejected());
        REQUIRE(results.size() == 2);
        REQUIRE(results[0] == 42);
        REQUIRE(results[1] == 43);
    }
    SUBCASE("fromResult with int")
    {
        int number = 0;
        auto task1 = concurrency::fromResult(42);
        auto task2 = task1.then([&](int x) { number = x; });

        REQUIRE(task1.isDone());
        REQUIRE_FALSE(task1.isRejected());
        REQUIRE(task2.isDone());
        REQUIRE_FALSE(task2.isRejected());
        REQUIRE(number == 42);
    }
    SUBCASE("scheduler with ImmediateScheduler")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        auto task = concurrency::createTask<void>([&](auto tcs) {
            try {
                throw std::domain_error("When Chuck Norris throws exceptions, it's across the room.");
            }
            catch (...) {
                tcs.setException(std::current_exception());
            }
        });

        REQUIRE(task.isDone());
        REQUIRE(task.isRejected());

        std::vector<std::string> result;

        auto task2 = task.then([&] {
            result.push_back("ok");
        });

        REQUIRE(task2.isDone());
        REQUIRE(task2.isRejected());

        task2.Catch([&](const std::domain_error& e) {
            result.push_back(e.what());
        });

        REQUIRE(result.size() == 1);
        REQUIRE(result.back() == "When Chuck Norris throws exceptions, it's across the room.");
#endif
    }
    SUBCASE("chaining like promise 1")
    {
        std::vector<std::string> result;

        auto task = concurrency::createTask<void>([&](auto tcs) {
            result.push_back("start");
            tcs.setResult();
        });
        task.then([&] { result.push_back("task A"); })
            .then([&] { result.push_back("task B"); })
            .Catch([&](std::exception_ptr) { result.push_back("onRejected"); })
            .then([&] { result.push_back("task C"); });

        REQUIRE(result.size() == 4);
        REQUIRE(result[0] == "start");
        REQUIRE(result[1] == "task A");
        REQUIRE(result[2] == "task B");
        REQUIRE(result[3] == "task C");
    }
    SUBCASE("chaining like promise 2")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        std::vector<std::string> result;

        auto task = concurrency::createTask<void>([&](auto tcs) {
            result.push_back("start");
            tcs.setResult();
        });
        task.then([&] {
                result.push_back("task A");
                throw std::domain_error("throw error at Task A");
            })
            .then([&] { result.push_back("task B"); })
            .Catch([&](const std::domain_error& e) { result.push_back(e.what()); })
            .then([&] { result.push_back("task C"); });

        REQUIRE(result.size() == 4);
        REQUIRE(result[0] == "start");
        REQUIRE(result[1] == "task A");
        REQUIRE(result[2] == "throw error at Task A");
        REQUIRE(result[3] == "task C");
#endif
    }
    SUBCASE("chaining like promise 3")
    {
        std::function<void()> defer;
        std::vector<std::string> result;

        auto task = concurrency::createTask<void>([&](auto tcs) {
            result.push_back("start");
            tcs.setResult();
        });
        task.then([&] {
                result.push_back("Delay");
                return concurrency::createTask<void>([&](auto tcs) {
                    defer = [tcs] { tcs.setResult(); };
                });
            })
            .then([&] {
                result.push_back("FromResult");
                return concurrency::fromResult(42);
            })
            .then([&](int x) {
                result.push_back(std::to_string(x));
            });

        defer();
        REQUIRE(result.size() == 4);
        REQUIRE(result[0] == "start");
        REQUIRE(result[1] == "Delay");
        REQUIRE(result[2] == "FromResult");
        REQUIRE(result[3] == "42");
    }
}

TEST_CASE("TaskCompletionSource")
{
    SUBCASE("setResult void")
    {
        std::vector<int> result;

        auto task = concurrency::createTask<void>([&](auto tcs) {
            REQUIRE(result.empty());
            tcs.setResult();
            REQUIRE(result.empty());
        });
        REQUIRE(result.empty());
        task.then([&] {
            result.push_back(42);
        });
        REQUIRE(result.size() == 1);
        REQUIRE(result.front() == 42);
    }
    SUBCASE("setResult int")
    {
        std::vector<int> result;

        auto task = concurrency::createTask<int>([&](auto tcs) {
            REQUIRE(result.empty());
            tcs.setResult(42);
            REQUIRE(result.empty());
        });
        REQUIRE(result.empty());
        task.then([&](int x) {
            result.push_back(x);
        });
        REQUIRE(result.size() == 1);
        REQUIRE(result.front() == 42);
    }
    SUBCASE("setException")
    {
#if POMDOG_TESTING_ENABLE_THROW_EXCEPTIONS
        std::vector<std::string> result;

        auto task = concurrency::createTask<int>([&](auto tcs) {
            try {
                throw std::domain_error("FUS RO DAH");
            }
            catch (...) {
                REQUIRE(result.empty());
                tcs.setException(std::current_exception());
                REQUIRE(result.empty());
            }
        });
        REQUIRE(result.empty());
        task.Catch([&](const std::domain_error& e) {
            result.push_back(e.what());
        });
        REQUIRE(result.size() == 1);
        REQUIRE(result.front() == "FUS RO DAH");
#endif
    }
    SUBCASE("with Signal")
    {
        using pomdog::Signal;
        using pomdog::signals::connectSingleShot;

        Signal<void(std::string const&)> nameChanged;

        std::vector<std::string> result;

        auto task = concurrency::createTask<std::string>([&](auto tcs) {
            connectSingleShot(nameChanged, [tcs](std::string const& n) {
                tcs.setResult(n);
            });
        });
        task.then([&](std::string const& n) {
            result.push_back(n);
        });

        REQUIRE(result.empty());

        nameChanged("chuck");
        REQUIRE(result.size() == 1);
        REQUIRE(result.back() == "chuck");

        nameChanged("norris");
        REQUIRE(result.size() == 1);
    }
    SUBCASE("fromSingleShotSignal")
    {
        using pomdog::Signal;

        Signal<void(std::string const&)> nameChanged;

        std::vector<std::string> result;

        auto task = concurrency::fromSingleShotSignal(nameChanged);
        task.then([&](std::string const& n) {
            result.push_back(n);
        });

        REQUIRE(result.empty());

        nameChanged("chuck");
        REQUIRE(result.size() == 1);
        REQUIRE(result.back() == "chuck");

        nameChanged("norris");
        REQUIRE(result.size() == 1);
    }
}
