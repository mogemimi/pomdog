//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "TimeSourceWin32.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Details {
namespace Win32 {

TimeSourceWin32::TimeSourceWin32()
{
	LARGE_INTEGER frequency;
	::QueryPerformanceFrequency(&frequency);

	POMDOG_ASSERT_MESSAGE(0 != frequency.QuadPart,
		"High-resolution performance counter not supported by installed hardware.");

	secondsPerTick = 1.0 / frequency.QuadPart;
}
//-----------------------------------------------------------------------
TimePointSeconds TimeSourceWin32::Now() const
{
	LARGE_INTEGER time;
	::QueryPerformanceCounter(&time);
	auto currentSeconds = time.QuadPart * secondsPerTick;
	return TimePointSeconds(TimePointSeconds{} + DurationSeconds(currentSeconds));
}

}// namespace Win32
}// namespace Details
}// namespace Pomdog
