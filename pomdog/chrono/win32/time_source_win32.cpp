// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/chrono/win32/time_source_win32.hpp"
#include "pomdog/platform/win32/prerequisites_win32.hpp"
#include "pomdog/utility/assert.hpp"

namespace pomdog::detail::win32 {

TimeSourceWin32::TimeSourceWin32() noexcept
{
    ::LARGE_INTEGER frequency;
    ::QueryPerformanceFrequency(&frequency);

    POME_ASSERT(frequency.QuadPart != 0);

    secondsPerTick_ = 1.0 / static_cast<double>(frequency.QuadPart);
}

TimePoint TimeSourceWin32::Now() const noexcept
{
    ::LARGE_INTEGER time;
    ::QueryPerformanceCounter(&time);
    auto currentSeconds = static_cast<double>(time.QuadPart) * secondsPerTick_;
    return TimePoint{Duration{currentSeconds}};
}

} // namespace pomdog::detail::win32
