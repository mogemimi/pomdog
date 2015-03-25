// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TIMESOURCECOCOA_3820AACF_C0B9_4A13_A493_C1164C3C19E7_HPP
#define POMDOG_TIMESOURCECOCOA_3820AACF_C0B9_4A13_A493_C1164C3C19E7_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Application/TimePoint.hpp"

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class TimeSourceCocoa {
public:
    TimeSourceCocoa();

    TimePoint Now() const;

private:
    double secondsPerTick;
};

}// namespace Cocoa
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_TIMESOURCECOCOA_3820AACF_C0B9_4A13_A493_C1164C3C19E7_HPP)
