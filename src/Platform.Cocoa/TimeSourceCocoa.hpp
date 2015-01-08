//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_TIMESOURCECOCOA_3820AACF_C0B9_4A13_A493_C1164C3C19E7_HPP
#define POMDOG_SRC_TIMESOURCECOCOA_3820AACF_C0B9_4A13_A493_C1164C3C19E7_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Application/DurationSeconds.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {

class TimeSourceCocoa {
public:
	TimeSourceCocoa();

	TimePointSeconds Now() const;
	
private:
	double secondsPerTick;
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_TIMESOURCECOCOA_3820AACF_C0B9_4A13_A493_C1164C3C19E7_HPP)
