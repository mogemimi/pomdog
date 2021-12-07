// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/platform/apple/time_source_apple.hpp"
#include "pomdog/utility/assert.hpp"
#include <mach/mach_time.h>

namespace Pomdog::Detail::Apple {

TimeSourceApple::TimeSourceApple()
{
    mach_timebase_info_data_t timeBase;
    mach_timebase_info(&timeBase);

    POMDOG_ASSERT(0 != timeBase.denom);
    double nanoSeconds = static_cast<double>(timeBase.numer) / timeBase.denom;

    constexpr double nanoScale = (1.0 / 1000000000LL);
    secondsPerTick = nanoScale * nanoSeconds;
}

TimePoint TimeSourceApple::Now() const
{
    auto currentSeconds = mach_absolute_time() * secondsPerTick;
    return TimePoint(Duration(currentSeconds));
}

} // namespace Pomdog::Detail::Apple
