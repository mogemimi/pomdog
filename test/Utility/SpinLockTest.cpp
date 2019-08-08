// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/detail/SpinLock.hpp"
#include "catch.hpp"
#include <array>
#include <memory>
#include <mutex>
#include <thread>

using Pomdog::Detail::SpinLock;

template <class Mutex>
struct Task final {
    Mutex lock;
    int result = 0;

    void run()
    {
        for (int i = 0; i < 1000; ++i) {
            [[maybe_unused]] std::lock_guard<Mutex> scoped{lock};
            ++result;
        }
    }
};

TEST_CASE("SpinLock", "[SpinLock]")
{
    std::array<std::thread, 2> threads;

    Task<SpinLock> task;

    for (auto& t : threads) {
        t = std::thread{[&]{ task.run(); }};
    }
    for (auto& t : threads) {
        t.join();
    }

    REQUIRE(task.result == 2000);
}
