// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/spin_lock.h"

namespace pomdog::detail {

SpinLock::SpinLock() noexcept = default;

void SpinLock::lock() noexcept
{
    while (flag_.test_and_set(std::memory_order_acquire)) {
        // NOTE: Instead of a busy loop, a wait instruction is inserted
        //       to put the CPU into low power consumption mode.
        //       When implementing spin, it is possible to call the __mm_pause(),
        //       _umwait(), TPAUSE, or std::this_thread::yield(), but here wait() is called for portability.
        //
        //       https://software.intel.com/en-us/articles/benefitting-power-and-performance-sleep-loops/
        //       http://open-std.org/JTC1/SC22/WG21/docs/papers/2016/p0514r0.pdf
        flag_.wait(true, std::memory_order_relaxed);
    }
}

void SpinLock::unlock() noexcept
{
    flag_.clear(std::memory_order_release);
    flag_.notify_one();
}

} // namespace pomdog::detail
