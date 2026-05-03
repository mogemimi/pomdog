// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/win32/windows_headers.h"
#include "pomdog/chrono/duration.h"

namespace pomdog::detail::win32 {

/// Provides high-precision sleep using the Win32 waitable timer API.
///
/// On Windows 10 version 1803 and later the timer is created with
/// `CREATE_WAITABLE_TIMER_HIGH_RESOLUTION` so that nanosecond-resolution
/// wakeups are available without raising the system timer period.
/// On older systems the implementation falls back to
/// `timeBeginPeriod(1)` + `std::this_thread::sleep_for()`.
class PreciseSleeper final {
private:
    HANDLE timerHandle_ = nullptr;
    bool highResSupported_ = false;

public:
    PreciseSleeper() noexcept;

    ~PreciseSleeper() noexcept;

    PreciseSleeper(const PreciseSleeper&) = delete;
    PreciseSleeper& operator=(const PreciseSleeper&) = delete;
    PreciseSleeper(PreciseSleeper&&) = delete;
    PreciseSleeper& operator=(PreciseSleeper&&) = delete;

    /// Sleeps for at least `duration`.
    /// If `duration` is zero or negative the function returns immediately.
    void sleep(Duration duration) noexcept;
};

} // namespace pomdog::detail::win32
