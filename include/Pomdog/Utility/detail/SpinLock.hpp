// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <atomic>

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
