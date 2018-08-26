// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include <chrono>

namespace Pomdog {

class GameClock;

/// Time unit that is describe instant in GameClock.
using TimePoint = std::chrono::time_point<GameClock, Duration>;

} // namespace Pomdog
