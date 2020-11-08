// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/Detail/SpinLock.hpp"
#include "Pomdog/Basic/Platform.hpp"

#if defined(POMDOG_PLATFORM_LINUX) && (defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64))
#include <emmintrin.h>
#else
#include <thread>
#endif

namespace Pomdog::Detail {

void SpinLock::lock() noexcept
{
    while (flag.test_and_set(std::memory_order_acquire)) {
#if defined(POMDOG_PLATFORM_LINUX) && (defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64))
        // NOTE: For both performance and power consumption, we use the
        // pause instruction in the loop instead of fully spinning.
        // https://software.intel.com/en-us/articles/benefitting-power-and-performance-sleep-loops/
        // http://open-std.org/JTC1/SC22/WG21/docs/papers/2016/p0514r0.pdf
        _mm_pause();
#else
        // NOTE: It is much faster to use `std::this_thread::yield()` than to
        // call `_mm_pause()` directly on Windows and macOS.
        std::this_thread::yield();
#endif
    }
}

void SpinLock::unlock() noexcept
{
    flag.clear(std::memory_order_release);
}

} // namespace Pomdog::Detail
