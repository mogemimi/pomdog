// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/win32/precise_sleeper_win32.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <timeapi.h>
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {
namespace {

#if !defined(CREATE_WAITABLE_TIMER_HIGH_RESOLUTION)
#error "CREATE_WAITABLE_TIMER_HIGH_RESOLUTION requires Windows SDK with Windows 10 version 1803 or later support."
#endif

static_assert(
    CREATE_WAITABLE_TIMER_HIGH_RESOLUTION == 0x00000002,
    "unexpected CREATE_WAITABLE_TIMER_HIGH_RESOLUTION value");

} // namespace

PreciseSleeper::PreciseSleeper() noexcept
{
    // NOTE: Attempt to create a high-resolution waitable timer available on
    // Windows 10 version 1803 and later. If the flag is unsupported
    // (older SDK or OS), `CreateWaitableTimerExW` returns nullptr and we fall back
    // to the `timeBeginPeriod()` path.
    timerHandle_ = ::CreateWaitableTimerExW(
        nullptr,
        nullptr,
        CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
        TIMER_ALL_ACCESS);

    if (timerHandle_ != nullptr) {
        highResSupported_ = true;
    }
}

PreciseSleeper::~PreciseSleeper() noexcept
{
    if (timerHandle_ != nullptr) {
        ::CloseHandle(timerHandle_);
        timerHandle_ = nullptr;
    }
}

void PreciseSleeper::sleep(Duration duration) noexcept
{
    if (duration <= Duration::zero()) {
        return;
    }

    if (highResSupported_) {
        POMDOG_ASSERT(timerHandle_ != nullptr);

        // NOTE: `SetWaitableTimer` uses 100-nanosecond intervals.
        const auto ns100 = std::chrono::duration_cast<
            std::chrono::duration<LONGLONG, std::ratio<1, 10'000'000>>>(duration);

        if (ns100.count() <= 0) {
            return;
        }

        ::LARGE_INTEGER dueTime = {};

        // NOTE: A negative value means a relative (non-periodic) delay.
        dueTime.QuadPart = -ns100.count();

        // NOTE: Add a small margin so the sleep does not overshoot the target.
        // 420 microseconds is chosen as a balance between precision and avoiding oversleeping.
        dueTime.QuadPart += 420 * 10;

        if (!::SetWaitableTimer(
                timerHandle_,
                &dueTime,
                0,       // non-periodic
                nullptr, // no APC completion routine
                nullptr, // no APC argument
                FALSE    // do not restore system power
                )) {
            // NOTE: If `SetWaitableTimer` fails, we return immediately
            // rather than sleeping for the requested duration.
            return;
        }

        // NOTE: Loop to handle APC wakeups (WAIT_IO_COMPLETION).
        // When an Asynchronous Procedure Call (APC) wakes this thread before the
        // timer fires, `WaitForSingleObjectEx` returns `WAIT_IO_COMPLETION`.
        // We re-enter the wait so the caller wakes at the intended time.
        DWORD result = 0;
        do {
            result = ::WaitForSingleObjectEx(
                timerHandle_,
                INFINITE,
                TRUE // alertable: allow APC wakeups
            );
        } while (result == WAIT_IO_COMPLETION);
    }
    else {
        // NOTE: Fallback for systems without high-resolution waitable timer
        // support. `timeBeginPeriod(1)` raises the system timer resolution to
        // 1 ms so that `sleep_for()` can sleep in ~1 ms increments.
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        if (ms.count() <= 0) {
            return;
        }

        // NOTE: `timeBeginPeriod` is process-local on Windows 10 version 2004
        // and later, but on older versions it is still global and can affect the
        // entire system, potentially impacting other applications.
        // It can also increase CPU load in the background and lead to higher
        // power consumption on laptops. For these reasons, using
        // `CREATE_WAITABLE_TIMER_HIGH_RESOLUTION` is preferable whenever possible.
        ::timeBeginPeriod(1);
        std::this_thread::sleep_for(ms);
        ::timeEndPeriod(1);
    }
}

} // namespace pomdog::detail::win32
