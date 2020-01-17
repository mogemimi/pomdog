// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "TimeSourceWin32.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog::Detail::Win32 {

TimeSourceWin32::TimeSourceWin32()
{
    LARGE_INTEGER frequency;
    ::QueryPerformanceFrequency(&frequency);

    POMDOG_ASSERT_MESSAGE(0 != frequency.QuadPart,
        "High-resolution performance counter not supported by installed hardware.");

    secondsPerTick = 1.0 / frequency.QuadPart;
}

TimePoint TimeSourceWin32::Now() const
{
    LARGE_INTEGER time;
    ::QueryPerformanceCounter(&time);
    auto currentSeconds = time.QuadPart * secondsPerTick;
    return TimePoint(Duration(currentSeconds));
}

} // namespace Pomdog::Detail::Win32
