// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_TIMESOURCELINUX_60ECC32A_HPP
#define POMDOG_TIMESOURCELINUX_60ECC32A_HPP

#include "Pomdog/Application/TimePoint.hpp"

namespace Pomdog {
namespace Detail {
namespace Linux {

class TimeSourceLinux {
public:
    TimeSourceLinux() = default;

    TimePoint Now() const;
};

} // namespace Linux
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_TIMESOURCELINUX_60ECC32A_HPP
