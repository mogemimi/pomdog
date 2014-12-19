//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_TIMESOURCE_C4211975_ADF9_4F64_950B_B891B9E5C0BD_HPP
#define POMDOG_TIMESOURCE_C4211975_ADF9_4F64_950B_B891B9E5C0BD_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Basic/Platform.hpp"

#if defined(POMDOG_PLATFORM_MACOSX)
#	include "../Platform.Cocoa/TimeSourceCocoa.hpp"
#elif defined(POMDOG_PLATFORM_WIN32)
#	include "../Platform.Win32/TimeSourceWin32.hpp"
#else
#	include "Pomdog/Application/DurationSeconds.hpp"
#	include <chrono>
#	error "Platform undefined or not supported."
#endif

namespace Pomdog {
namespace Details {

#if defined(POMDOG_PLATFORM_MACOSX)
using TimeSource = Details::Cocoa::TimeSourceCocoa;
#elif defined(POMDOG_PLATFORM_WIN32)
using TimeSource = Details::Win32::TimeSourceWin32;
#else

class TimeSource {
public:
	DurationSeconds Now() const
	{
		auto timePoint = std::chrono::high_resolution_clock::now();
		return DurationSeconds(std::chrono::time_point_cast<std::chrono::duration<double>>(timePoint).time_since_epoch().count());
	}
};

#endif

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_TIMESOURCE_C4211975_ADF9_4F64_950B_B891B9E5C0BD_HPP)
