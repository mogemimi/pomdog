// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/spin_lock.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <mutex>
#include <thread>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::SpinLock;

TEST_CASE("SpinLock")
{
    SUBCASE("lock and unlock")
    {
        SpinLock spinLock;
        spinLock.lock();
        spinLock.unlock();
    }
    SUBCASE("multiple lock and unlock")
    {
        SpinLock spinLock;
        for (int i = 0; i < 100; ++i) {
            spinLock.lock();
            spinLock.unlock();
        }
    }
    SUBCASE("thread safe counter increment")
    {
        SpinLock spinLock;
        int counter = 0;
        constexpr int numThreads = 4;
        constexpr int iterationsPerThread = 1000;

        std::vector<std::thread> threads;
        threads.reserve(numThreads);

        for (int t = 0; t < numThreads; ++t) {
            threads.emplace_back([&spinLock, &counter]() {
                for (int i = 0; i < iterationsPerThread; ++i) {
                    std::lock_guard<SpinLock> lock{spinLock};
                    ++counter;
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        REQUIRE(counter == numThreads * iterationsPerThread);
    }
}
