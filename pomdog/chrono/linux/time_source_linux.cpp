// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/chrono/linux/time_source_linux.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <ctime>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::linux {

TimePoint TimeSourceLinux::now() const noexcept
{
    struct timespec now;
    [[maybe_unused]] const auto result = clock_gettime(CLOCK_MONOTONIC, &now);
    POMDOG_ASSERT(result == 0);

    constexpr f64 nanoScale = (1.0 / 1000000000LL);

    return TimePoint{Duration{
        static_cast<f64>(now.tv_sec) +
        static_cast<f64>(now.tv_nsec) * nanoScale}};
}

} // namespace pomdog::detail::linux
