// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/spin_lock.h"
#include <catch_amalgamated.hpp>
#include <array>
#include <memory>
#include <mutex>
#include <thread>

using pomdog::detail::SpinLock;

namespace {

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

} // namespace

TEST_CASE("SpinLock", "[SpinLock]")
{
    std::array<std::thread, 2> threads;

    Task<SpinLock> task;

    for (auto& t : threads) {
        t = std::thread{[&] { task.run(); }};
    }
    for (auto& t : threads) {
        t.join();
    }

    REQUIRE(task.result == 2000);
}
