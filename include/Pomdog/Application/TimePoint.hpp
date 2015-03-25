// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TIMEPOINT_CEF9FF84_2A9C_4B46_85E3_6309933C9162_HPP
#define POMDOG_TIMEPOINT_CEF9FF84_2A9C_4B46_85E3_6309933C9162_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Duration.hpp"
#include <chrono>

namespace Pomdog {

class GameClock;
using TimePoint = std::chrono::time_point<GameClock, Duration>;

}// namespace Pomdog

#endif // !defined(POMDOG_TIMEPOINT_CEF9FF84_2A9C_4B46_85E3_6309933C9162_HPP)
