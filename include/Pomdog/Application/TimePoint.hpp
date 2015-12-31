// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include <chrono>

namespace Pomdog {

class GameClock;
using TimePoint = std::chrono::time_point<GameClock, Duration>;

} // namespace Pomdog
