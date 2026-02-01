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
    f64 nanoSeconds = static_cast<f64>(timeBase.numer) / timeBase.denom;

    constexpr f64 nanoScale = (1.0 / 1000000000LL);
    secondsPerTick_ = nanoScale * nanoSeconds;
}

TimePoint TimeSourceApple::now() const noexcept
{
    const auto currentSeconds = mach_absolute_time() * secondsPerTick_;
    return TimePoint{Duration{currentSeconds}};
}

} // namespace pomdog::detail::apple
