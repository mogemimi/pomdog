// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <atomic>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

class POMDOG_EXPORT SpinLock final {
    std::atomic_flag flag_;
    static_assert(__cplusplus >= 202002L, "ATOMIC_FLAG_INIT was deprecated in C++20, https://cplusplus.github.io/LWG/issue3659");

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
