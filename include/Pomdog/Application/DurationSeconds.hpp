//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DURATIONSECONDS_7361380F_B525_46D6_A6B7_51EB8AD523C0_HPP
#define POMDOG_DURATIONSECONDS_7361380F_B525_46D6_A6B7_51EB8AD523C0_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <chrono>

namespace Pomdog {

class GameClock;

using DurationSeconds = std::chrono::duration<double, std::ratio<1>>;
using TimePointSeconds = std::chrono::time_point<GameClock, DurationSeconds>;

}// namespace Pomdog

#endif // !defined(POMDOG_DURATIONSECONDS_7361380F_B525_46D6_A6B7_51EB8AD523C0_HPP)
