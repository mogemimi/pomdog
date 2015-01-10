//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_TIMESOURCELINUX_60ECC32A_9ADC_4A84_A672_FD0A7EEE9E16_HPP
#define POMDOG_TIMESOURCELINUX_60ECC32A_9ADC_4A84_A672_FD0A7EEE9E16_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Application/DurationSeconds.hpp"

namespace Pomdog {
namespace Details {
namespace Linux {

class TimeSourceLinux {
public:
	TimeSourceLinux() = default;

	TimePointSeconds Now() const;
};

}// namespace Linux
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_TIMESOURCELINUX_60ECC32A_9ADC_4A84_A672_FD0A7EEE9E16_HPP)
