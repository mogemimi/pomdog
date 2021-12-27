// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/spin_lock.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if defined(POMDOG_PLATFORM_LINUX) && (defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64))
#include <emmintrin.h>
#else
#include <thread>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

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

} // namespace pomdog::detail
