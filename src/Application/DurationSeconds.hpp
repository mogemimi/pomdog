//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DURATIONSECONDS_36BC4E10_F87D_4CF9_98BC_41FB26901666_HPP
#define POMDOG_DURATIONSECONDS_36BC4E10_F87D_4CF9_98BC_41FB26901666_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <chrono>

namespace Pomdog {

class Clock;

using DurationSeconds = std::chrono::duration<double, std::ratio<1>>;
using TimePointSeconds = std::chrono::time_point<Clock, DurationSeconds>;

}// namespace Pomdog

#endif // !defined(POMDOG_DURATIONSECONDS_36BC4E10_F87D_4CF9_98BC_41FB26901666_HPP)
