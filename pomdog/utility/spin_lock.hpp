// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <atomic>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

class POMDOG_EXPORT SpinLock final {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    /// Locks the mutex.
    void lock() noexcept;

    /// Unlocks the mutex.
    void unlock() noexcept;
};

} // namespace Pomdog::Detail
