// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/chrono/linux/time_source_linux.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <ctime>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::linux {

TimePoint TimeSourceLinux::Now() const noexcept
{
    struct timespec now;
    [[maybe_unused]] const auto result = clock_gettime(CLOCK_MONOTONIC, &now);
    POMDOG_ASSERT(result == 0);

    constexpr double nanoScale = (1.0 / 1000000000LL);

    return TimePoint{Duration{
        static_cast<double>(now.tv_sec) +
        static_cast<double>(now.tv_nsec) * nanoScale}};
}

} // namespace pomdog::detail::linux
