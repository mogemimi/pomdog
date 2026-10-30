// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/platform.h"
#include "pomdog/utility/diagnostic_queue.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <memory>
#include <string>
#include <vector>
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include <thread>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Error;
using pomdog::detail::DiagnosticQueue;
namespace errors = pomdog::errors;

TEST_CASE("DiagnosticQueue")
{
    SUBCASE("emit delivers diagnostics in enqueue order")
    {
        DiagnosticQueue queue;
        queue.enqueue(errors::make("first"));
        queue.enqueue(errors::make("second"));

        std::vector<std::string> results;
        queue.emit([&results](std::unique_ptr<Error> diagnostic) {
            REQUIRE(diagnostic != nullptr);
            results.push_back(diagnostic->toString());
        });

        REQUIRE(results.size() == 2);
        REQUIRE(results[0] == "first");
        REQUIRE(results[1] == "second");
    }
    SUBCASE("emit drains the queue")
    {
        DiagnosticQueue queue;
        queue.enqueue(errors::make("only"));

        int count = 0;
        const auto func = [&count]([[maybe_unused]] std::unique_ptr<Error> diagnostic) {
            ++count;
        };

        queue.emit(func);
        REQUIRE(count == 1);

        queue.emit(func);
        REQUIRE(count == 1);
    }
    SUBCASE("diagnostics enqueued during emit are delivered on the next emit")
    {
        DiagnosticQueue queue;
        std::vector<std::string> results;

        const auto func = [&queue, &results](std::unique_ptr<Error> diagnostic) {
            results.push_back(diagnostic->toString());
            if (results.size() == 1) {
                queue.enqueue(errors::make("from callback"));
            }
        };

        queue.enqueue(errors::make("first"));

        queue.emit(func);
        REQUIRE(results.size() == 1);
        REQUIRE(results[0] == "first");

        queue.emit(func);
        REQUIRE(results.size() == 2);
        REQUIRE(results[1] == "from callback");
    }
    SUBCASE("queues do not share diagnostics")
    {
        DiagnosticQueue queueA;
        DiagnosticQueue queueB;
        queueA.enqueue(errors::make("a"));

        int countB = 0;
        queueB.emit([&countB]([[maybe_unused]] std::unique_ptr<Error> diagnostic) {
            ++countB;
        });
        REQUIRE(countB == 0);

        int countA = 0;
        queueA.emit([&countA]([[maybe_unused]] std::unique_ptr<Error> diagnostic) {
            ++countA;
        });
        REQUIRE(countA == 1);
    }
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
    SUBCASE("enqueue from another thread")
    {
        DiagnosticQueue queue;

        std::thread producer([&queue] {
            for (int i = 0; i < 100; ++i) {
                queue.enqueue(errors::make(std::to_string(i)));
            }
        });
        producer.join();

        std::vector<std::string> results;
        queue.emit([&results](std::unique_ptr<Error> diagnostic) {
            results.push_back(diagnostic->toString());
        });

        REQUIRE(results.size() == 100);
        REQUIRE(results.front() == "0");
        REQUIRE(results.back() == "99");
    }
#endif
}
