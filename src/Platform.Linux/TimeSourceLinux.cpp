// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "TimeSourceLinux.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <ctime>

namespace Pomdog::Detail::Linux {

TimePoint TimeSourceLinux::Now() const
{
    struct timespec now;
    if (0 != clock_gettime(CLOCK_MONOTONIC, &now)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "FUS RO DAH");
    }

    constexpr double nanoScale = (1.0 / 1000000000LL);

    return TimePoint { Duration {
        static_cast<double>(now.tv_sec) +
        static_cast<double>(now.tv_nsec) * nanoScale}};
}

} // namespace Pomdog::Detail::Linux
