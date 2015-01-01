//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "TimeSourceCocoa.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <mach/mach_time.h>

namespace Pomdog {
namespace Details {
namespace Cocoa {

TimeSourceCocoa::TimeSourceCocoa()
{
	mach_timebase_info_data_t timeBase;
	mach_timebase_info(&timeBase);
	
	POMDOG_ASSERT(0 != timeBase.denom);
	double const nanoSecondsPerTick = static_cast<double>(timeBase.numer) / timeBase.denom;
	
	constexpr double nanoScale = (1.0 / 1000000000UL);
	secondsPerTick = nanoScale * nanoSecondsPerTick;
}
//-----------------------------------------------------------------------
TimePointSeconds TimeSourceCocoa::Now() const
{
	auto currentSeconds = mach_absolute_time() * secondsPerTick;
	return TimePointSeconds(TimePointSeconds{} + DurationSeconds(currentSeconds));
}

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog
