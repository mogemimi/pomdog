// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <atomic>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Represents a simple spin lock that can be used for synchronization in multi-threaded environments.
/// A spin lock is a type of mutex that uses busy-waiting to acquire the lock.
/// It is typically used in scenarios where the lock is expected to be held for a very short duration,
/// as it can lead to high CPU usage if the lock is held for a long time.
///
/// Note that this spin lock does not support features like try_lock() or recursive locking,
/// and it is not fair (i.e., it does not guarantee that waiting threads will acquire the lock in the order they requested it).
/// It is important to use this spin lock with caution, as it can lead to performance issues if not used correctly.
/// For example, it should not be used in situations where the lock may be held for a long time or
/// where there is a high contention for the lock, as it can lead to excessive CPU usage and reduced performance.
/// In such cases, it may be more appropriate to use a more traditional mutex
/// or other synchronization primitives that are designed to handle such scenarios.
class POMDOG_EXPORT SpinLock final {
    std::atomic_flag flag_;
#if defined(_MSC_VER) && defined(_MSVC_LANG)
    // NOTE: https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
    static_assert(_MSVC_LANG >= 202002L, "ATOMIC_FLAG_INIT was deprecated in C++20, https://cplusplus.github.io/LWG/issue3659");
#else
    static_assert(__cplusplus >= 202002L, "ATOMIC_FLAG_INIT was deprecated in C++20, https://cplusplus.github.io/LWG/issue3659");
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

} // namespace pomdog
