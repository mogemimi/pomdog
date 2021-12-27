// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/chrono/apple/time_source_apple.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <mach/mach_time.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::apple {

TimeSourceApple::TimeSourceApple() noexcept
{
    mach_timebase_info_data_t timeBase;
    mach_timebase_info(&timeBase);

    POMDOG_ASSERT(0 != timeBase.denom);
    double nanoSeconds = static_cast<double>(timeBase.numer) / timeBase.denom;

    constexpr double nanoScale = (1.0 / 1000000000LL);
    secondsPerTick_ = nanoScale * nanoSeconds;
}

TimePoint TimeSourceApple::Now() const noexcept
{
    const auto currentSeconds = mach_absolute_time() * secondsPerTick_;
    return TimePoint{Duration{currentSeconds}};
}

} // namespace pomdog::detail::apple
