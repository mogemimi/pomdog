// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TIMEPOINT_CEF9FF84_HPP
#define POMDOG_TIMEPOINT_CEF9FF84_HPP

#include "Duration.hpp"
#include <chrono>

namespace Pomdog {

class GameClock;
using TimePoint = std::chrono::time_point<GameClock, Duration>;

} // namespace Pomdog

#endif // POMDOG_TIMEPOINT_CEF9FF84_HPP
