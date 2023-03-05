// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <atomic>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

class POMDOG_EXPORT SpinLock final {
#if defined(__EMSCRIPTEN__)
    // NOTE: for C++20
    std::atomic_flag flag;
#else
    // NOTE: for C++17
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
#endif

public:
    SpinLock() noexcept;
    SpinLock(const SpinLock&) noexcept = delete;
    SpinLock& operator=(const SpinLock&) noexcept = delete;

    /// Locks the mutex.
    void lock() noexcept;

    /// Unlocks the mutex.
    void unlock() noexcept;
};

} // namespace pomdog::detail
