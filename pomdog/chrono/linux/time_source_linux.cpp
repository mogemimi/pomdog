// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/chrono/linux/time_source_linux.hpp"
#include "pomdog/utility/exception.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <ctime>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::linux {

TimePoint TimeSourceLinux::Now() const noexcept
{
    struct timespec now;
    if (0 != clock_gettime(CLOCK_MONOTONIC, &now)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "FUS RO DAH");
    }

    constexpr double nanoScale = (1.0 / 1000000000LL);

    return TimePoint{Duration{
        static_cast<double>(now.tv_sec) +
        static_cast<double>(now.tv_nsec) * nanoScale}};
}

} // namespace pomdog::detail::linux
