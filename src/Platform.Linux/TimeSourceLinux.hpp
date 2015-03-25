// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TIMESOURCELINUX_60ECC32A_9ADC_4A84_A672_FD0A7EEE9E16_HPP
#define POMDOG_TIMESOURCELINUX_60ECC32A_9ADC_4A84_A672_FD0A7EEE9E16_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Application/TimePoint.hpp"

namespace Pomdog {
namespace Detail {
namespace Linux {

class TimeSourceLinux {
public:
    TimeSourceLinux() = default;

    TimePoint Now() const;
};

}// namespace Linux
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_TIMESOURCELINUX_60ECC32A_9ADC_4A84_A672_FD0A7EEE9E16_HPP)
